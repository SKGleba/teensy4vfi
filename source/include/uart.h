#ifndef __UART_H__
#define __UART_H__

#include "aips.h"
#include "utils.h"

#define UART_1_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart1)
#define UART_2_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart2) 
#define UART_3_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart3) 
#define UART_4_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart4) 
#define UART_5_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart5) 
#define UART_6_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart6) 
#define UART_7_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart7) 
#define UART_8_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->lpuart8)

enum UART_BAUD_BITS {
    UART_BAUD_BITS_SBR = 0,
    UART_BAUD_BITS_SBNS = 13,
    UART_BAUD_BITS_RXEDGIE,
    UART_BAUD_BITS_LBKDIE,
    UART_BAUD_BITS_RESYNCDIS,
    UART_BAUD_BITS_BOTHEDGE,
    UART_BAUD_BITS_MATCFG,
    UART_BAUD_BITS_RDMAE = 21,
    UART_BAUD_BITS_TDMAE = 23,
    UART_BAUD_BITS_OSR,
    UART_BAUD_BITS_M10 = 29,
    UART_BAUD_BITS_MAEN2,
    UART_BAUD_BITS_MAEN1
};

enum UART_CTRL_BITS {
    UART_CTRL_BITS_PT = 0,
    UART_CTRL_BITS_PE,
    UART_CTRL_BITS_ILT,
    UART_CTRL_BITS_WAKE,
    UART_CTRL_BITS_M,
    UART_CTRL_BITS_RSRC,
    UART_CTRL_BITS_DOZEEN,
    UART_CTRL_BITS_LOOPS,
    UART_CTRL_BITS_IDLECFG,
    UART_CTRL_BITS_M7 = 11,
    UART_CTRL_BITS_MA2IE = 14,
    UART_CTRL_BITS_MA1IE,
    UART_CTRL_BITS_SBK,
    UART_CTRL_BITS_RWU,
    UART_CTRL_BITS_RE,
    UART_CTRL_BITS_TE,
    UART_CTRL_BITS_ILIE,
    UART_CTRL_BITS_RIE,
    UART_CTRL_BITS_TCIE,
    UART_CTRL_BITS_TIE,
    UART_CTRL_BITS_PEIE,
    UART_CTRL_BITS_FEIE,
    UART_CTRL_BITS_NEIE,
    UART_CTRL_BITS_ORIE,
    UART_CTRL_BITS_TXINV,
    UART_CTRL_BITS_TXDIR,
    UART_CTRL_BITS_R9T8,
    UART_CTRL_BITS_R8T9
};

enum UART_DATA_BITS {
    UART_DATA_BITS_DATA = 0,
    UART_DATA_BITS_IDLINE = 11,
    UART_DATA_BITS_RXEMPT,
    UART_DATA_BITS_FRETSC,
    UART_DATA_BITS_PARITYPE,
    UART_DATA_BITS_NOISY
};

enum UART_FIFO_BITS {
    UART_FIFO_BITS_RXFIFOSIZE = 0,
    UART_FIFO_BITS_RXFE = 3,
    UART_FIFO_BITS_TXFIFOSIZE,
    UART_FIFO_BITS_TXFE = 7,
    UART_FIFO_BITS_RXUFE,
    UART_FIFO_BITS_TXOFE,
    UART_FIFO_BITS_RXIDEN,
    UART_FIFO_BITS_RXFLUSH = 14,
    UART_FIFO_BITS_TXFLUSH,
    UART_FIFO_BITS_RXUF,
    UART_FIFO_BITS_TXOF,
    UART_FIFO_BITS_RXEMPT = 22,
    UART_FIFO_BITS_TXEMPT
};

enum UART_STAT_BITS {
    UART_STAT_BITS_MA2F = 14,
    UART_STAT_BITS_MA1F,
    UART_STAT_BITS_PF,
    UART_STAT_BITS_FE,
    UART_STAT_BITS_NF,
    UART_STAT_BITS_OR,
    UART_STAT_BITS_IDLE,
    UART_STAT_BITS_RDRF,
    UART_STAT_BITS_TC,
    UART_STAT_BITS_TDRE,
    UART_STAT_BITS_RAF,
    UART_STAT_BITS_LBKDE,
    UART_STAT_BITS_BRK13,
    UART_STAT_BITS_RWUID,
    UART_STAT_BITS_RXINV,
    UART_STAT_BITS_MSBF,
    UART_STAT_BITS_RXEDGIF,
    UART_STAT_BITS_LBKDIF
};

enum UART_WATER_BITS {
    UART_WATER_BITS_TXWATER = 0,
    UART_WATER_BITS_TXCOUNT = 8,
    UART_WATER_BITS_RXWATER = 16,
    UART_WATER_BITS_RXCOUNT = 24
};

#define UART_WATER_BITMASK_TXWATER 0b11
#define UART_WATER_BITMASK_TXCOUNT 0b111
#define UART_WATER_BITMASK_RXWATER 0b11
#define UART_WATER_BITMASK_RXCOUNT 0b111

#define UART_GLOBAL_BITS_RST 1

#define UART_BUS_1_GPIO_BUS 1
#define UART_BUS_2_GPIO_BUS 1
#define UART_BUS_3_GPIO_BUS 1
#define UART_BUS_4_GPIO_BUS 2
#define UART_BUS_5_GPIO_BUS 2
#define UART_BUS_6_GPIO_BUS 1
#define UART_BUS_7_GPIO_BUS 1


// BAUD = CLK / ((OSR+1) * SBR)
// pref OSR+1 close to 16 for ~1-2% tol
// CLK is by default 24MHz
enum UART_BAUD_PRECALC {
    UART_BAUD_115200 = (BITNVAL(UART_BAUD_BITS_OSR, 15) | BITNVAL(UART_BAUD_BITS_SBR, 13)), // OSR+1 * SBR = 208 vs 208.33
    UART_BAUD_38400 = (BITNVAL(UART_BAUD_BITS_OSR, 15) | BITNVAL(UART_BAUD_BITS_SBR, 39)), // OSR+1 * SBR = 624 vs 625
};

enum UART_INIT_FLAGS_BITS {
    UART_INIT_BITS_TX_FIFO_EN = 0, // enable TX FIFO
    UART_INIT_BITS_RX_FIFO_EN, // enable RX FIFO
    UART_INIT_BITS_TX_EN, // enable uart transmitter
    UART_INIT_BITS_RX_EN, // enable uart receiver
    UART_INIT_BITS_TX_INV, // invert tx data
    UART_INIT_BITS_RX_INV, // invert rx data
    UART_INIT_BITS_MSB_FIRST // reverse bit order
};

extern aips_lpuart* uart_regs[9];
extern uint8_t uart_ccg[9];

void uart_init(int bus, int baud, int flags, bool wait);
void uart_write(int bus, unsigned int data);
void uart_print(int bus, char* str);
void uart_printn(int bus, char* str, int n);
unsigned int uart_read(int bus, unsigned int timeout, bool wait);
int uart_scann(int bus, uint8_t* out, int outsize, unsigned int timeout);
int uart_scanns(int bus, char* out, int outsize, unsigned int timeout);
#define uart_rxfifo_flush(bus) (uart_regs[bus]->fifo |= BITN(UART_FIFO_BITS_RXFLUSH))
#define uart_txfifo_flush(bus) (uart_regs[bus]->fifo |= BITN(UART_FIFO_BITS_TXFLUSH))
#define uart_wait_tc(bus) while(!(uart_regs[bus]->stat & BITN(UART_STAT_BITS_TC))){};

#endif