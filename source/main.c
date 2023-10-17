#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/gpio.h"
#include "include/teensy.h"
#include "include/uart.h"
#include "include/debug.h"
#include "include/clib.h"
#include "include/ccm.h"
#include "include/rpc.h"
#include "include/glitch.h"

int main();
void test();

void init() {
    ccm_set_uart_clk(true, 0, true); // derive uart clock from 24mhz osc_clk, don't divide

#ifndef SILENT
    if (teensy_uart_init(
        DEBUG_UARTN,
        UART_BAUD_115200,
        BITN(UART_INIT_BITS_RX_EN) | BITN(UART_INIT_BITS_RX_FIFO_EN) | BITN(UART_INIT_BITS_TX_EN) | BITN(UART_INIT_BITS_TX_FIFO_EN),
        true
    ) >= 0) { // init default debug uart
        g_debug_uartn = DEBUG_UARTN; // print to default debug uart
        printf("\ninit teensy4vfi [%X], me @ %X\n", get_build_timestamp(), init);
    }
#endif

    ccm_set_core_clkf(DEFAULT_CLOCKSPEED, 0); // ARM & TCstuff clocks

    main();

    while (1)
        wfi();
}

int main() {

    test();

    // infiniblink
    teensy_set_pad_ctl(TEENSY_PAD_ORANGE_LED, -1, TEENSY_PAD_MODE_GPIO, true);
    teensy_pad_logic_mode(TEENSY_PAD_ORANGE_LED, true, true);
    while (true) {
        teensy_pad_logic_set(TEENSY_PAD_ORANGE_LED, true);
        delay(0x10000);
        teensy_pad_logic_clear(TEENSY_PAD_ORANGE_LED, true);
        delay(0x10000);
    }
}

void test() {
    printf("test test test\n");
    {
        rpc_loop();
    }
    printf("all tests done\n");
}