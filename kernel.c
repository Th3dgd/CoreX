// kernel/kernel.c

#include "include/kernel.h"

void kernel_entry() {
    k_main();
    while(1) {}
}
