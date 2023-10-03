#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "types.h"
#include "defs.h"
#include "teensy.h"

#ifdef SILENT

#define print(str)
#define printf
#define printn(str, n)
#define printx(x)
#define printp(x)
#define hexdump(addr, length, show_addr)

#else

#define print(str) teensy_uart_print(g_debug_uartn, (char *)(str))
#define printf debug_printFormat
#define printn(str, n) teensy_uart_printn(g_debug_uartn, (char *)(str), n)
#define printx(x) debug_printU32((uint32_t)(x), true)
#define printp(x) printf("%X: %X\n", (uint32_t)(x), vp (x))
#define hexdump(addr, length, show_addr) debug_printRange(addr, length, (int)show_addr)

// get a "\r\n" terminated string from debug uart
#define scans(string_buf, max_len) teensy_uart_scanns(g_debug_uartn, (char*)string_buf, max_len, 0)
#define scans_timeout(string_buf, max_len, timeout) teensy_uart_scanns(g_debug_uartn, (char*)string_buf, max_len, timeout)

// get [count] bytes from debug uart
#define scanb(bytes_buf, count) teensy_uart_scann(g_debug_uartn, (uint8_t*)bytes_buf, count, 0)
#define scanb_timeout(bytes_buf, count, timeout) teensy_uart_scann(g_debug_uartn, (uint8_t*)bytes_buf, count, timeout)

#define rxflush() teensy_uart_rxfifo_flush(g_debug_uartn)

#endif

extern volatile int g_debug_uartn;
void debug_printU32(uint32_t value, int add_nl);
void debug_printFormat(char* base, ...);
void debug_printRange(uint32_t addr, uint32_t size, int show_addr);

#endif