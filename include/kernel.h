// include/kernel.h

#ifndef KERNEL_H
#define KERNEL_H
#define WHITE_TXT 0x07 /* light gray on black text */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define NULL ((void*)0)

#define true ((void*)1)
#define false ((void*)0)

// Values
extern unsigned int line;

// Functions

// screen functions
void k_clear_screen();
unsigned int k_printf(char *message);
void k_print_keys(int line, char key, int col);
void k_printf_int(int num);

// keyboard
void k_get_input(char *buffer, int buffer_size, int col_lenght, void* key_typing);
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void update_cursor(unsigned int row, unsigned int col);
void keyboard_init();
char k_getchar();

// core
void k_main();


#endif // KERNEL_H
