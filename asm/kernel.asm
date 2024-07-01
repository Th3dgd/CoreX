; kernel/asm/kernel.asm

MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM

global kernel_entry
extern k_main

section .text
kernel_entry:
    ; Set up the stack
    mov esp, stack_top

    ; Call the main kernel function
    call k_main

    ; Enter an infinite loop
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 4
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:
