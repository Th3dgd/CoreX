// src/hal/keyboard.c

#include "../include/kernel.h"

void keyboard_init() {
}

void update_cursor(unsigned int row, unsigned int col) {
    unsigned short position = ((row - 1) * 80) + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

uint8_t inb(uint16_t port)
{
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t', /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0, /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`',   0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */
    'm', ',', '.', '/',   0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ', /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,  0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char getchar()
{
    uint8_t scancode;
    
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0);
    
    scancode = inb(KEYBOARD_DATA_PORT);
    
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) != 0);
    
    if (scancode & 0x80) {
        return 0;
    }
    
    return scancode < 128 ? scancode_to_ascii[scancode] : 0;
}

void k_get_input(char *buffer, int buffer_size) {
    char key;
    int index = 0;
    unsigned int col = 19;

    while (index < buffer_size - 1) {
        key = getchar();

        if (key == '\b') {
            if (index > 0) {
                index--;
                k_print_keys(line, '\0', col);
                col--;
            }
        }else {
            if (key == '\n' || key == '\r') {
                buffer[index] = '\0';
                break;
            }

            if (key != 0) {
                buffer[index] = key;
                index++;
                col++;
                k_print_keys(line, key, col);
            }
        }
        update_cursor(line, col);
    }

    buffer[index] = '\0';
}
