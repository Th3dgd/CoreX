// src/core/kmain.c

#include "../include/kernel.h"
#include "../include/k_string.h"

void k_main() {
    k_clear_screen();

    while (true){
        char input[256];
        
        k_printf("[th3dgd@corex ~/]$ ");

        k_get_input(input, sizeof(input));

        if (k_strcmp(input, "help") == 0){
            k_printf("help: print this message");
            k_printf("clear: clear the screen");
            k_printf("exit: exit the kernel");
        }else if(k_strcmp(input, "clear") == 0) {
            k_clear_screen();
        }else {
            k_printf("Unknown command");
            k_printf(input);
        }
    }
    //k_printf(input);
    // k_process_command(input);
}
