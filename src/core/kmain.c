// src/core/kmain.c

#include "../include/kernel.h"
#include "../include/k_string.h"
#include "../include/ui/xhell.h"
#include "../include/ui/auth.h"

extern char stack_top[];

void __stack_chk_fail_local() {
    k_println("FAIL IN __stack_chk_fail_local");
}

void k_main() {
    __asm__ volatile ("mov $stack_top, %esp");
    __asm__ volatile ("mov $stack_top, %ebp");

    // called to UI

    auth_ui();

    xhell_ui();
}
