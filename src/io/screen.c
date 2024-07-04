// src/io/screen.c

#include "../include/kernel.h"
#include "../include/k_string.h"

unsigned int line = 0;
unsigned int col = 0;
unsigned int col_print = 0;

void k_clear_screen() {
    line = 0;
    char *vidmem = (char *) 0xb8000;
    unsigned int i = 0;
    while(i < (80 * 25 * 2)) {
        vidmem[i] = '\0';
        i++;
        vidmem[i] = WHITE_TXT;
        i++;
    }
    update_cursor(0, 0);
}

void k_print_keys(int row, char key, int col) {
    char *vidmem = (char *) 0xb8000;
    unsigned int i = ((row - 1) * 80 + (col - 1)) * 2;
    vidmem[i] = key;
    vidmem[i + 1] = WHITE_TXT;
}

void k_println_int(int num) {
    char str[12];
    int_to_str(num, str, 1);
    k_println(str);
}

unsigned int k_print(char *message) {
    char *vidmem = (char *) 0xb8000;
    unsigned int i = ((line) * 80 + (col_print)) * 2;

    while (*message != '\0') {
        if (*message == '\n') {
            line++;
            col_print = 0;
            i = (line * 80 * 2);
            message++;
        } else {
            vidmem[i] = *message;
            message++;
            i++;
            vidmem[i] = WHITE_TXT;
            i++;
            col_print++;
        }
    }

    if (line >= 25) {
        line = 0;
    }
    
    update_cursor((line + 1), col_print);

    return 1;
}

unsigned int k_println(char *message) {
    char *vidmem = (char *) 0xb8000;
    unsigned int i = (line * 80 * 2);
    col = 0;

    while (*message != '\0') {
        if (*message == '\n') {
            line++;
            col = 0;
            i = (line * 80 * 2);
            message++;
        } else {
            vidmem[i] = *message;
            message++;
            i++;
            vidmem[i] = WHITE_TXT;
            i++;
            col++;
        }
    }

    line++;
    if (line >= 25) {
        line = 0;
    }
    
    update_cursor(line, col);

    return 1;
}

