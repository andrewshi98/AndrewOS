#include "boot/boot.h"

void kernel_start(void *boot_param) {
    *(int *)boot_param = 5;
}