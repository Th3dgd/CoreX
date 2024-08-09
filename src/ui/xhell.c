// src/ui/xhell.c

#include "../include/ui/xhell.h"
#include "../include/kernel.h"
#include "../include/fs/fat32.h"
#include "../include/fs/ext4.h"
#include "../include/k_string.h"
#include "../include/k_bios_called.h"

__attribute__((naked))
void ls_fat32_command_trampoline() {
    __asm__ volatile (
        "call ls_fat32_command\n"
        "ret\n"
    );
}

__attribute__((naked))
void ls_ext4_command_trampoline() {
    __asm__ volatile (
        "call ls_ext4_command\n"
        "ret\n"
    );
}

void xhell() {
    while (true){
        char input[256];

        k_println("[th3dgd@corex ~/]$ ");

        k_get_input(input, sizeof(input), 19, true);

        if (k_strcmp(input, "help") == 0){
            k_println("help: print this message");
            k_println("clear: clear the screen");
            k_println("exit: exit the kernel");
        }else if(k_strcmp(input, "clear") == 0) {
            k_clear_screen();
        }else if(k_strcmp(input, "clock") == 0){
            uint32_t time = getBIOSClock();
            uint8_t hours = (time >> 16) & 0xFF;
            uint8_t minutes = (time >> 8) & 0xFF;
            uint8_t seconds = time & 0xFF;
            
            char time_str[9];
            k_sprintf(time_str, "%02d:%02d:%02d", hours, minutes, seconds);
            k_println("time: ");
            k_println(time_str);

        }else if (k_strcmp(input, "lsfat32") == 0) {
            ls_fat32_command_trampoline();
        }else if (k_strcmp(input, "lsext4") == 0) {
            ls_ext4_command_trampoline(); // TODO fix lsext4 command
        }else {
            k_println("Unknown command");
            k_println(input);
        }
    }
}