#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/iomuxc.h"
#include "include/ccm.h"
#include "include/defs.h"

#include "include/uart.h"

aips_lpuart* uart_regs[9] = { // legacy support
    NULL, // no uart0
    UART_1_OFFSET,
    UART_2_OFFSET,
    UART_3_OFFSET,
    UART_4_OFFSET,
    UART_5_OFFSET,
    UART_6_OFFSET,
    UART_7_OFFSET,
    UART_8_OFFSET
};

uint8_t uart_ccg[9] = { // legacy support
    0, // no uart0
    CCM_CCG_LPUART1,
    CCM_CCG_LPUART2,
    CCM_CCG_LPUART3,
    CCM_CCG_LPUART4,
    CCM_CCG_LPUART5,
    CCM_CCG_LPUART6,
    CCM_CCG_LPUART7,
    CCM_CCG_LPUART8
};

volatile int g_uart_bus = 1; // should have !0 checks everywhere but meh

void uart_init(int bus, int baud, bool wait) {
    ccm_control_gate(uart_ccg[bus], CCM_CCG_ALWAYS_ON, wait); // open the uart bus clock gate
    uart_regs[bus]->fifo = BITN(UART_FIFO_BITS_TXFE); // TXFIFOEN
    uart_regs[bus]->baud = baud; // BAUD
    uart_regs[bus]->ctrl = BITN(UART_CTRL_BITS_RE) | BITN(UART_CTRL_BITS_TE); // TE
}

void uart_write(int bus, unsigned int data) {
    while (!(uart_regs[bus]->fifo & BITN(UART_BAUD_BITS_TDMAE))) {};

    uart_regs[bus]->data = data;
}

void uart_print(int bus, char* str) {
    while (*str) {
        if (*str == '\n')
            uart_write(bus, '\r');

        uart_write(bus, *str++);
    }
}

void uart_printn(int bus, char* str, int n) {
    char* z = str;

    while (n && *z) {
        if (*z == '\n')
            uart_write(bus, '\r');

        uart_write(bus, *z++);

        n--;
    }
}