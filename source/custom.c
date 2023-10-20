#include "include/types.h"
#include "include/debug.h"

#include "include/custom.h"

int custom_main(int a0, int a1, int a2, void* ax) {
    printf("custom func with args %X %X %X | %X\n", a0, a1, a2, ax);
    return 0;
}