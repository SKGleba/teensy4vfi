#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "types.h"
#include "defs.h"
#include "uart.h"

#ifdef SILENT

#define print(str)
#define printf
#define printn(str, n)
#define printx(x)
#define printp(x)
#define hexdump(addr, length, show_addr)

#else

#define print(str) uart_print(g_uart_bus, (char *)(str))
#define printf debug_printFormat
#define printn(str, n) uart_printn(g_uart_bus, (char *)(str), n)
#define printx(x) debug_printU32((uint32_t)(x), true)
#define printp(x) printf("%X: %X\n", (uint32_t)(x), vp (x))
#define hexdump(addr, length, show_addr) debug_printRange(addr, length, (int)show_addr)

#endif

void debug_printU32(uint32_t value, int add_nl);
void debug_printFormat(char* base, ...);
void debug_printRange(uint32_t addr, uint32_t size, int show_addr);

#endif