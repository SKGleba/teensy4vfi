#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/iomuxc.h"
#include "include/gpio.h"

#include "include/teensy.h"

const uint8_t teensy_pad_to_port[40] = {
    TEENSY_PAD_0_PORT, TEENSY_PAD_1_PORT, TEENSY_PAD_2_PORT, TEENSY_PAD_3_PORT,
    TEENSY_PAD_4_PORT, TEENSY_PAD_5_PORT, TEENSY_PAD_6_PORT, TEENSY_PAD_7_PORT,
    TEENSY_PAD_8_PORT, TEENSY_PAD_9_PORT, TEENSY_PAD_10_PORT, TEENSY_PAD_11_PORT,
    TEENSY_PAD_12_PORT, TEENSY_PAD_13_PORT, TEENSY_PAD_14_PORT, TEENSY_PAD_15_PORT,
    TEENSY_PAD_16_PORT, TEENSY_PAD_17_PORT, TEENSY_PAD_18_PORT, TEENSY_PAD_19_PORT,
    TEENSY_PAD_20_PORT, TEENSY_PAD_21_PORT, TEENSY_PAD_22_PORT, TEENSY_PAD_23_PORT,
    TEENSY_PAD_24_PORT, TEENSY_PAD_25_PORT, TEENSY_PAD_26_PORT, TEENSY_PAD_27_PORT,
    TEENSY_PAD_28_PORT, TEENSY_PAD_29_PORT, TEENSY_PAD_30_PORT, TEENSY_PAD_31_PORT,
    TEENSY_PAD_32_PORT, TEENSY_PAD_33_PORT, TEENSY_PAD_34_PORT, TEENSY_PAD_35_PORT,
    TEENSY_PAD_36_PORT, TEENSY_PAD_37_PORT, TEENSY_PAD_38_PORT, TEENSY_PAD_39_PORT
};

uint8_t teensy_pad_to_gpio_bus[40] = {
    TEENSY_PAD_0_BUS, TEENSY_PAD_1_BUS, TEENSY_PAD_2_BUS, TEENSY_PAD_3_BUS,
    TEENSY_PAD_4_BUS, TEENSY_PAD_5_BUS, TEENSY_PAD_6_BUS, TEENSY_PAD_7_BUS,
    TEENSY_PAD_8_BUS, TEENSY_PAD_9_BUS, TEENSY_PAD_10_BUS, TEENSY_PAD_11_BUS,
    TEENSY_PAD_12_BUS, TEENSY_PAD_13_BUS, TEENSY_PAD_14_BUS, TEENSY_PAD_15_BUS,
    TEENSY_PAD_16_BUS, TEENSY_PAD_17_BUS, TEENSY_PAD_18_BUS, TEENSY_PAD_19_BUS,
    TEENSY_PAD_20_BUS, TEENSY_PAD_21_BUS, TEENSY_PAD_22_BUS, TEENSY_PAD_23_BUS,
    TEENSY_PAD_24_BUS, TEENSY_PAD_25_BUS, TEENSY_PAD_26_BUS, TEENSY_PAD_27_BUS,
    TEENSY_PAD_28_BUS, TEENSY_PAD_29_BUS, TEENSY_PAD_30_BUS, TEENSY_PAD_31_BUS,
    TEENSY_PAD_32_BUS, TEENSY_PAD_33_BUS, TEENSY_PAD_34_BUS, TEENSY_PAD_35_BUS,
    TEENSY_PAD_36_BUS, TEENSY_PAD_37_BUS, TEENSY_PAD_38_BUS, TEENSY_PAD_39_BUS
};

uint8_t teensy_uart_to_imxbus_rx_tx[8][3] = {
    {0, 0, 0}, // no uart0
    {TEENSY_UART1_IMX_BUS, TEENSY_PAD_UART1_RX, TEENSY_PAD_UART1_TX},
    {TEENSY_UART2_IMX_BUS, TEENSY_PAD_UART2_RX, TEENSY_PAD_UART2_TX},
    {TEENSY_UART3_IMX_BUS, TEENSY_PAD_UART3_RX, TEENSY_PAD_UART3_TX},
    {TEENSY_UART4_IMX_BUS, TEENSY_PAD_UART4_RX, TEENSY_PAD_UART4_TX},
    {TEENSY_UART5_IMX_BUS, TEENSY_PAD_UART5_RX, TEENSY_PAD_UART5_TX},
    {TEENSY_UART6_IMX_BUS, TEENSY_PAD_UART6_RX, TEENSY_PAD_UART6_TX},
    {TEENSY_UART7_IMX_BUS, TEENSY_PAD_UART7_RX, TEENSY_PAD_UART7_TX}
};

void teensy_pad_logic_ctrl_tightness(int pad, bool tight, bool wait) {
    if ((tight && (teensy_get_pad_gpio_bus(pad) < GPIO_TCGPIO_OFFSET)) || (!tight && (teensy_get_pad_gpio_bus(pad) > GPIO_TCGPIO_OFFSET))) {
        teensy_get_pad_gpio_bus(pad) = gpio_get_bus_tightalt(teensy_get_pad_gpio_bus(pad));
        iomuxc_set_tcgpio(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), 0, wait);
    }
}

int teensy_uart_init(int teensy_uartn, int baud, bool init_tx, bool init_rx, bool wait) {
    if (teensy_uartn == 0 || teensy_uartn > 7)
        return -1;

    if (init_tx) {
        if (teensy_set_pad_ctl(teensy_uart_get_tx_pad(teensy_uartn), -1, TEENSY_PAD_MODE_UART, wait) < 0)
            return -2;
    }

    if (init_rx) {
        if (teensy_set_pad_ctl(teensy_uart_get_rx_pad(teensy_uartn), -1, TEENSY_PAD_MODE_UART, wait) < 0)
            return -3;
    }

    uart_init(teensy_uart_get_imx_bus(teensy_uartn), baud, wait);

    return 0;
}