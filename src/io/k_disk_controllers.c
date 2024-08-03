#include "../include/k_disk_controllers.h"
#include "../include/k_string.h"
#include "../include/kernel.h"

#include <stdint.h>

#define SECTOR_SIZE 512
#define ATA_PRIMARY_COMMAND_PORT 0x1F7
#define ATA_PRIMARY_CONTROL_PORT 0x3F6
#define ATA_PRIMARY_DATA_PORT    0x1F0

struct fat32_boot_sector {
    uint8_t  jump_boot[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  table_count;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t table_size_16;
    uint16_t sectors_per_track;
    uint16_t head_side_count;
    uint32_t hidden_sector_count;
    uint32_t total_sectors_32;
    uint32_t table_size_32;
    uint16_t extended_flags;
    uint16_t fat_version;
    uint32_t root_cluster;
    uint16_t fat_info;
    uint16_t backup_BS_sector;
    uint8_t  reserved_0[12];
    uint8_t  drive_number;
    uint8_t  reserved_1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  fat_type_label[8];
} __attribute__((packed));

struct fat32_dir_entry {
    uint8_t  name[11];
    uint8_t  attr;
    uint8_t  lcase;
    uint8_t  create_time_tenth;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed));

static inline void insw(uint16_t port, void *addr, uint32_t word_count) {
    asm volatile ("cld; rep insw"
                  : "+D" (addr), "+c" (word_count)
                  : "d" (port)
                  : "memory");
}

int read_sectors(uint32_t start_sector, uint32_t num_sectors, void* buffer) {
    uint8_t status;

    do {
        status = inb(ATA_PRIMARY_COMMAND_PORT);
    } while (status & 0x80);

    outb(ATA_PRIMARY_DATA_PORT + 2, (uint8_t) num_sectors);
    outb(ATA_PRIMARY_DATA_PORT + 3, (uint8_t) start_sector);
    outb(ATA_PRIMARY_DATA_PORT + 4, (uint8_t)(start_sector >> 8));
    outb(ATA_PRIMARY_DATA_PORT + 5, (uint8_t)(start_sector >> 16));
    outb(ATA_PRIMARY_DATA_PORT + 6, 0xE0);
    outb(ATA_PRIMARY_COMMAND_PORT, 0x20);

    do {
        status = inb(ATA_PRIMARY_COMMAND_PORT);
    } while (status & 0x80);

    if (status & 0x01) {
        k_println("Error de lectura de disco\n");
        return 0;
    }

    insw(ATA_PRIMARY_DATA_PORT, buffer, SECTOR_SIZE * num_sectors / 2);

    do {
        status = inb(ATA_PRIMARY_COMMAND_PORT);
    } while (status & 0x80);

    return 1;
}



uint32_t cluster_to_lba(uint32_t cluster, uint32_t cluster_begin_lba, uint8_t sectors_per_cluster) {
    return cluster_begin_lba + (cluster - 2) * sectors_per_cluster;
}

void ls_command() {
    struct fat32_boot_sector bs;
    uint32_t fat_begin_lba;
    uint32_t cluster_begin_lba;
    uint32_t root_dir_first_cluster;
    uint32_t current_cluster;
    uint32_t sector;

    if (!read_sectors(0, 1, &bs)) {
        k_println("Error al leer el sector de arranque\n");
        return;
    }

    fat_begin_lba = bs.reserved_sector_count;
    cluster_begin_lba = fat_begin_lba + bs.table_count * bs.table_size_32;
    root_dir_first_cluster = bs.root_cluster;

    current_cluster = root_dir_first_cluster;

    while (current_cluster < 0x0FFFFFF8) {
        sector = cluster_to_lba(current_cluster, cluster_begin_lba, bs.sectors_per_cluster);
        char buffer[SECTOR_SIZE];

        for (int i = 0; i < bs.sectors_per_cluster; i++) {
            if (!read_sectors(sector + i, 1, buffer)) {
                k_sprintf(buffer, "Error al leer el sector %d\n", sector + i);
                k_println(buffer);
                return;
            }

            for (unsigned int j = 0; j < SECTOR_SIZE / sizeof(struct fat32_dir_entry); j++) {
                struct fat32_dir_entry* entry = (struct fat32_dir_entry*)(buffer + j * sizeof(struct fat32_dir_entry));

                if (entry->name[0] == 0x00) {
                    break;
                }

                if (entry->name[0] == 0xE5) {
                    continue;
                }

                if (entry->attr & 0x0F) {
                    continue;
                }

                for (int k = 0; k < 8 && entry->name[k] != ' '; k++) {
                    k_print(ascii_to_string(entry->name[k]));
                }

                if (entry->name[8] != ' ') {
                    k_print(".");
                    for (int k = 8; k < 11 && entry->name[k] != ' '; k++) {
                        k_print(ascii_to_string(entry->name[k]));
                    }
                }
                k_print(" ");
            }

            if (buffer[0] == 0x00) {
                break;
            }
        }

        uint32_t fat_sector = fat_begin_lba + (current_cluster * 4 / SECTOR_SIZE);
        uint32_t fat_offset = (current_cluster * 4) % SECTOR_SIZE;
        uint8_t fat_buffer[SECTOR_SIZE];

        if (!read_sectors(fat_sector, 1, fat_buffer)) {
            k_println("Error al leer la FAT\n");
            return;
        }

        current_cluster = *((uint32_t*)((uint8_t*)fat_buffer + fat_offset)) & 0x0FFFFFFF;

        if (current_cluster >= 0x0FFFFFF8) {
            break;
        }
    }
    return;
}
