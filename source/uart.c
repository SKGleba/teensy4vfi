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

void uart_init(int bus, int baud, int flags, bool wait) {
    ccm_control_gate(uart_ccg[bus], CCM_CCG_ALWAYS_ON, wait); // open the uart dev clock gate
    uart_regs[bus]->global |= BITN(UART_GLOBAL_BITS_RST); // put uart dev in reset
    uart_regs[bus]->global &= ~(BITN(UART_GLOBAL_BITS_RST)); // pull uart dev out of reset, no min delay acc to ref man
    uart_regs[bus]->water = BITNVAL(UART_WATER_BITS_TXWATER, 3);
    uart_regs[bus]->baud = baud; // BAUD
    uart_regs[bus]->stat |= BITNVAL(UART_STAT_BITS_RXINV, !!(flags & BITN(UART_INIT_BITS_RX_INV)))
        | BITNVAL(UART_STAT_BITS_MSBF, !!(flags & BITN(UART_INIT_BITS_MSB_FIRST)))
    ; // RX inv, MSB first
    uart_regs[bus]->fifo |= BITNVAL(UART_FIFO_BITS_TXFE, !!(flags & BITN(UART_INIT_BITS_TX_FIFO_EN)))
        | BITNVAL(UART_FIFO_BITS_RXFE, !!(flags & BITN(UART_INIT_BITS_RX_FIFO_EN)))
    ; // RX/TX FIFO enable
    uart_regs[bus]->ctrl |= BITNVAL(UART_CTRL_BITS_TE, !!(flags & BITN(UART_INIT_BITS_TX_EN)))
        | BITNVAL(UART_CTRL_BITS_RE, !!(flags & BITN(UART_INIT_BITS_RX_EN)))
        | BITNVAL(UART_CTRL_BITS_TXINV, !!(flags & BITN(UART_INIT_BITS_TX_INV)))
    ; // TX inv, RX/TX enable
    while (wait && !(uart_regs[bus]->ctrl & (BITNVAL(UART_CTRL_BITS_TE, !!(flags & BITN(UART_INIT_BITS_TX_EN))) | BITNVAL(UART_CTRL_BITS_RE, !!(flags & BITN(UART_INIT_BITS_RX_EN)))))) {};
}

void uart_write(int bus, unsigned int data) {
    while (!(uart_regs[bus]->stat & BITN(UART_STAT_BITS_TDRE))) {};
    uart_regs[bus]->data = data;
}

// read [bus] rx reg until there is valid data or [timeout]+1 times
unsigned int uart_read(int bus, unsigned int timeout, bool wait) {
    unsigned int data;
    do {
        data = uart_regs[bus]->data;
    } while ((data & BITN(UART_DATA_BITS_RXEMPT)) && (wait || (timeout--, timeout + 1)));
    return data;
}

// TODO: uart_scan with IDLE-based break?

// setting [timeout] to 0 will make it wait indefinitely
int uart_scann(int bus, uint8_t* out, int outsize, unsigned int timeout) {
    uart_regs[bus]->stat = uart_regs[bus]->stat; // ack current status
    unsigned int data;
    for (int i = 0; i < outsize; i++) {
        data = uart_read(bus, timeout, !timeout);
        if (data & BITN(UART_DATA_BITS_RXEMPT))
            return -1;
        out[i] = (char)data;
    }
    return 0;
}

// setting [timeout] to 0 will make it wait indefinitely
int uart_scanns(int bus, char* out, int outsize, unsigned int timeout) {
    uart_regs[bus]->stat = uart_regs[bus]->stat; // ack current status
    unsigned int data;
    for (int i = 0; i < outsize; i++) {
        data = uart_read(bus, timeout, !timeout);
        if (data & BITN(UART_DATA_BITS_RXEMPT))
            return -1;
        out[i] = (char)data;
        if ((char)data == '\n') {
            if (i && out[i - 1] == '\r')
                return 0;
        }
    }
    return -1;
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