// src/ui/auth.c

#include "../include/ui/auth.h"
#include "../include/kernel.h"
#include "../include/k_string.h"

void auth_ui() {
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
}