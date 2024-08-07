// src/fs/ext4.c
#include "../include/fs/ext4.h"
#include "../include/k_string.h"
#include "../include/kernel.h"

#include <stdint.h>

#define ATA_DATA        0x1F0
#define ATA_FEATURES    0x1F1
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DEVICE      0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

#define STATUS_ERR  (1 << 0)
#define STATUS_DRQ  (1 << 3)
#define STATUS_SRV  (1 << 4)
#define STATUS_DF   (1 << 5)
#define STATUS_RDY  (1 << 6)
#define STATUS_BSY  (1 << 7)

#define MAX_RETRY   3
#define TIMEOUT     1000000 

struct ext4_super_block {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count_lo;
    uint32_t s_r_blocks_count_lo;
    uint32_t s_free_blocks_count_lo;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_cluster_size;
    uint32_t s_blocks_per_group;
    uint32_t s_clusters_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
};

struct ext4_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size_lo;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks_lo;
    uint32_t i_flags;
    uint32_t i_block[15];
};

struct ext4_dir_entry_2 {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    char     name[];
};


static inline void outb_ext4(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb_ext4(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw_ext4(uint16_t port) {
    uint16_t value;
    asm volatile ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint8_t read_sectors_ext4(uint32_t lba, uint8_t sector_count, void *buffer) {
    if (sector_count == 0) return -1;

    for (int retry = 0; retry < MAX_RETRY; retry++) {
        outb_ext4(ATA_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
        outb_ext4(ATA_FEATURES, 0);
        outb_ext4(ATA_SECTOR_COUNT, sector_count);
        outb_ext4(ATA_LBA_LOW, lba & 0xFF);
        outb_ext4(ATA_LBA_MID, (lba >> 8) & 0xFF);
        outb_ext4(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
        outb_ext4(ATA_COMMAND, 0x20);

        uint8_t status;
        while (((status = inb_ext4(ATA_STATUS)) & (STATUS_DRQ | STATUS_ERR | STATUS_BSY)) != STATUS_DRQ) {
            if (status & (STATUS_ERR | STATUS_DF)) return -2;
        }

        uint16_t* buf = (uint16_t*)buffer;
        for (int s = 0; s < sector_count; s++) {
            for (int i = 0; i < 256; i++) {
                buf[i] = inw_ext4(ATA_DATA);
            }
            buf += 256;
            if (s < sector_count - 1) {
                while (((status = inb_ext4(ATA_STATUS)) & (STATUS_DRQ | STATUS_ERR | STATUS_BSY)) != STATUS_DRQ) {
                    if (status & (STATUS_ERR | STATUS_DF)) return -2;
                }
            }
        }
        return 0;
    }

    return -3;
}

void ls_ext4_command() {
    uint32_t inode_number = 2;
    struct ext4_super_block sb;
    struct ext4_inode inode;
    struct ext4_dir_entry_2 *dir_entry;
    uint8_t buffer[4096];

    read_sectors_ext4(2, 1, &sb);

    uint32_t group = (inode_number - 1) / sb.s_inodes_per_group;
    uint32_t index = (inode_number - 1) % sb.s_inodes_per_group;
    uint32_t inode_table_block = sb.s_first_data_block + group * sb.s_blocks_per_group;

    read_sectors_ext4(inode_table_block * 8 + index * sizeof(struct ext4_inode) / 512, 1, &inode);

    for (int i = 0; i < 12 && inode.i_block[i]; i++) {
        read_sectors_ext4(inode.i_block[i] * 8, 8, buffer);
        
        dir_entry = (struct ext4_dir_entry_2 *)buffer;
        while ((uint8_t*)dir_entry < buffer + 4096) {
            if (dir_entry->inode != 0) {
                k_print("File: ");
                k_print(dir_entry->name);
                k_println("");
            }
            dir_entry = (struct ext4_dir_entry_2 *)((uint8_t*)dir_entry + dir_entry->rec_len);
        }
    }

    asm __volatile__ (
        "call xhell\n"
        "ret\n");
}
