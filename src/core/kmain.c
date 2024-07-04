// src/core/kmain.c

#include "../include/kernel.h"
#include "../include/k_string.h"
#include "../include/k_disk_controllers.h"
#include "../include/k_bios_called.h"

void k_main() {
    k_clear_screen();

    char input_username[256];
    char input_password[256];

    do {
        k_println("Login CoreX\n");

        k_println("Username: ");
        k_get_input(input_username, sizeof(input_username), 10, true);

        k_println("Password: ");
        k_get_input(input_password, sizeof(input_password), 10, false);

        k_clear_screen();
    }while((k_strcmp(input_username, "th3dgd") != 0) || (k_strcmp(input_password, "1234") != 0));

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

        }else if (k_strcmp(input, "ls") == 0) {
            ls_command();
        }else {
            k_println("Unknown command");
            k_println(input);
        }
    }
    // k_printf(input);
    // k_process_command(input);
}
