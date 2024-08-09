/*
 *
 * kernel.c - version 0.0.1
 * WryKun System
 * 
 */
#define WHITE_TXT 0x07 /* light gray on black text */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define NULL ((void*)0)

typedef struct MemoryBlock {
    uint32_t size;
    struct MemoryBlock *next;
    int free;
} MemoryBlock;

#define MEMORY_SIZE 1024 * 1024
static char memory[MEMORY_SIZE];

MemoryBlock *freeList;

void memory_init()
{
    freeList = (MemoryBlock *)memory;
    freeList->size = MEMORY_SIZE - sizeof(MemoryBlock);
    freeList->next = NULL;
    freeList->free = 1;
}

void *malloc(uint32_t size)
{
    MemoryBlock *current = freeList;
    MemoryBlock *previous = NULL;

    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            if (current->size > size + sizeof(MemoryBlock))
            {
                MemoryBlock *newBlock = (MemoryBlock *)((char *)current + sizeof(MemoryBlock) + size);
                newBlock->size = current->size - size - sizeof(MemoryBlock);
                newBlock->next = current->next;
                newBlock->free = 1;
                current->size = size;
                current->next = newBlock;
            }
            current->free = 0;
            return (void *)((char *)current + sizeof(MemoryBlock));
        }
        previous = current;
        current = current->next;
    }

    return NULL;
}

void free(void *ptr)
{
    if (ptr == NULL)
        return;

    MemoryBlock *block = (MemoryBlock *)((char *)ptr - sizeof(MemoryBlock));
    block->free = 1;

    MemoryBlock *current = freeList;
    while (current != NULL)
    {
        if (current->free && current->next != NULL && current->next->free)
        {
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
        }
        current = current->next;
    }
}

void k_clear_screen();
unsigned int k_printf(char *message, unsigned int line);
void k_get_input(char *buffer, int buffer_size);
char getchar();

void k_main() 
{
    memory_init();
    char input[256];
    
    k_clear_screen();
    k_printf("command#", 0);

    k_get_input(input, sizeof(input));

    k_printf("You entered: ", 1);
    k_printf(input, 2);

    //k_process_command(input);
}

void k_clear_screen()
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i=0;
    while(i < (80*25*2))
    {
        vidmem[i]=' ';
        i++;
        vidmem[i]=WHITE_TXT;
        i++;
    };
};

unsigned int k_printf(char *message, unsigned int line)
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i=0;

    i=(line*80*2);

    while(*message!=0)
    {
        if(*message=='\n')
        {
            line++;
            i=(line*80*2);
            *message++;
        } else {
            vidmem[i]=*message;
            *message++;
            i++;
            vidmem[i]=WHITE_TXT;
            i++;
        };
    };

    return(1);
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

char getchar()
{
    uint8_t status;
    char keycode;

    do {
        status = inb(KEYBOARD_STATUS_PORT);
    } while ((status & 0x01) == 0);

    keycode = inb(KEYBOARD_DATA_PORT);
    return keycode;
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

void k_get_input(char *buffer, int buffer_size)
{
    char key;
    int index = 0;

    while (1)
    {
        key = getchar();
        key = scancode_to_ascii[(unsigned char)key];

        if (key == '\n' || key == '\r')
        {
            buffer[index] = '\0';
            break;
        }

        if (key != 0)
        {
            buffer[index] = key;
            index++;

            if (index >= buffer_size - 1)
                break;
        }
    }
}
