// kernel/kernel.c
// Kernel developing for Th3dgd

#include "include/kernel.h"

void kernel_entry() {
    k_main();
    while(1) {
        k_printf(".");
    }
}
