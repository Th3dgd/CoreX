// src/core/kmain.c

#include "../include/kernel.h"
#include "../include/k_string.h"
#include "../include/ui/xhell.h"

extern char stack_top[];

void __stack_chk_fail_local() {
    k_println("FAIL IN __stack_chk_fail_local");
}

void k_main() {
    __asm__ volatile ("mov $stack_top, %esp");
    __asm__ volatile ("mov $stack_top, %ebp");

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

    xhell();

    // k_printf(input);
    // k_process_command(input);
}
