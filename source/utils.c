#include "include/types.h"
#include "include/compile_time.h"

#include "include/utils.h"

void delay(int n) {
    volatile int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 200; j++)
            ;
}

__attribute__((noinline))
uint32_t get_build_timestamp(void) {
    return (uint32_t)UNIX_TIMESTAMP;
}