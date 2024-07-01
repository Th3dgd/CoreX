// src/core/kmain.c

#include "../include/kernel.h"

int k_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

void k_main() {
    k_clear_screen();

    //k_test_print();

    while (true){
        char input[256];
        
        k_printf("[wrykun@kernel ~/]$ ");

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
