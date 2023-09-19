#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/gpio.h"
#include "include/teensy.h"
#include "include/uart.h"
#include "include/debug.h"
#include "include/clib.h"
#include "include/ccm.h"

void main();
void test();

void init() {
    ccm_set_uart_clk(true, 0, true); // derive uart clock from 24mhz osc_clk, don't divide

#ifndef SILENT
    if (teensy_uart_init(DEBUG_UARTN, UART_BAUD_115200, true, false, true) >= 0) { // init default debug uart, output only
        g_uart_bus = teensy_uart_get_imx_bus(DEBUG_UARTN); // print to default debug uart
        printf("\ninit teensy4vfi [%X], me @ %X\n", get_build_timestamp(), init);
    }
#endif

    ccm_set_core_clkf(DEFAULT_CLOCKSPEED, 0); // ARM & TCstuff clocks

    main();

    while (1)
        wfi();
}

void main() {

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

#define GLITCH_DRIVER_PAD 22
#define GLITCH_TRIGGER_PAD 23
void test() { // vfi crowbar method procedure concept
    printf("test test test\n");
    {
        ccm_set_core_clkf(CCM_ARM_CLKF_600MHZ, 0);

        // configure mosfet driver
        int driver_pad_ctl = IOMUXC_PORT_CTL_FIELD(true, IOMUXC_PORT_CTL_DSE_R0(6), IOMUXC_PORT_CTL_SPEED(4), false, true, false, 0, false);
        teensy_set_pad_ctl(GLITCH_DRIVER_PAD, driver_pad_ctl, TEENSY_PAD_MODE_GPIO, true);
        teensy_pad_logic_ctrl_tightness(GLITCH_DRIVER_PAD, true, true);
        teensy_pad_logic_mode(GLITCH_DRIVER_PAD, true, true);

        // configure glitch trigger
        int trigger_pad_ctl = IOMUXC_PORT_CTL_FIELD(true, IOMUXC_PORT_CTL_DSE_R0(6), IOMUXC_PORT_CTL_SPEED(4), false, true, true, 0, false);
        teensy_set_pad_ctl(GLITCH_TRIGGER_PAD, trigger_pad_ctl, TEENSY_PAD_MODE_GPIO, true);
        teensy_pad_logic_ctrl_tightness(GLITCH_TRIGGER_PAD, true, true);
        teensy_pad_logic_mode(GLITCH_TRIGGER_PAD, false, true);

        // signal that we are ready
        teensy_set_pad_ctl(TEENSY_PAD_ORANGE_LED, -1, TEENSY_PAD_MODE_GPIO, true);
        teensy_pad_logic_mode(TEENSY_PAD_ORANGE_LED, true, true);
        teensy_pad_logic_set(TEENSY_PAD_ORANGE_LED, true);

        // wait for trigger
        teensy_pad_logic_read(GLITCH_TRIGGER_PAD, true, true);

        // glitch
        teensy_pad_logic_set(GLITCH_DRIVER_PAD, true);
        delay(0x40000);
        teensy_pad_logic_clear(GLITCH_DRIVER_PAD, true);
    }
    printf("all tests done\n");
}