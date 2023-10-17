#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"
#include "include/iomuxc.h"
#include "include/gpio.h"

#include "include/teensy.h"

const uint8_t teensy_pad_to_port[TEENSY_PADS_COUNT] = {
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
#ifdef TARGET_TEENSY41
    , TEENSY_PAD_40_PORT, TEENSY_PAD_41_PORT, TEENSY_PAD_42_PORT, TEENSY_PAD_43_PORT,
    TEENSY_PAD_44_PORT, TEENSY_PAD_45_PORT, TEENSY_PAD_46_PORT, TEENSY_PAD_47_PORT,
    TEENSY_PAD_48_PORT, TEENSY_PAD_49_PORT, TEENSY_PAD_50_PORT, TEENSY_PAD_51_PORT,
    TEENSY_PAD_52_PORT, TEENSY_PAD_53_PORT, TEENSY_PAD_54_PORT
#endif
};

uint8_t teensy_pad_to_gpio_bus[TEENSY_PADS_COUNT] = {
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
#ifdef TARGET_TEENSY41
    , TEENSY_PAD_40_BUS, TEENSY_PAD_41_BUS, TEENSY_PAD_42_BUS, TEENSY_PAD_43_BUS,
    TEENSY_PAD_44_BUS, TEENSY_PAD_45_BUS, TEENSY_PAD_46_BUS, TEENSY_PAD_47_BUS,
    TEENSY_PAD_48_BUS, TEENSY_PAD_49_BUS, TEENSY_PAD_50_BUS, TEENSY_PAD_51_BUS,
    TEENSY_PAD_52_BUS, TEENSY_PAD_53_BUS, TEENSY_PAD_54_BUS
#endif
};

uint8_t teensy_uartn_to_imxbus_rx_tx[TEENSY_UARTN_COUNT + 1][3] = {
    {0, 0, 0}, // no uart0
    {TEENSY_UART1_IMX_BUS, TEENSY_PAD_UART1_RX, TEENSY_PAD_UART1_TX},
    {TEENSY_UART2_IMX_BUS, TEENSY_PAD_UART2_RX, TEENSY_PAD_UART2_TX},
    {TEENSY_UART3_IMX_BUS, TEENSY_PAD_UART3_RX, TEENSY_PAD_UART3_TX},
    {TEENSY_UART4_IMX_BUS, TEENSY_PAD_UART4_RX, TEENSY_PAD_UART4_TX},
    {TEENSY_UART5_IMX_BUS, TEENSY_PAD_UART5_RX, TEENSY_PAD_UART5_TX},
    {TEENSY_UART6_IMX_BUS, TEENSY_PAD_UART6_RX, TEENSY_PAD_UART6_TX},
    {TEENSY_UART7_IMX_BUS, TEENSY_PAD_UART7_RX, TEENSY_PAD_UART7_TX}
#ifdef TARGET_TEENSY41
    , {TEENSY_UART8_IMX_BUS, TEENSY_PAD_UART8_RX, TEENSY_PAD_UART8_TX}
#endif
};

void teensy_pad_logic_ctrl_tightness(int pad, bool tight, bool wait) {
    if ((tight && (teensy_get_pad_gpio_bus(pad) < GPIO_TCGPIO_OFFSET)) || (!tight && (teensy_get_pad_gpio_bus(pad) > GPIO_TCGPIO_OFFSET))) {
        teensy_get_pad_gpio_bus(pad) = gpio_get_bus_tightalt(teensy_get_pad_gpio_bus(pad));
        iomuxc_set_tcgpio(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), 0, wait);
    }
}

int teensy_uart_init(int teensy_uartn, int baud, int init_bitflags, bool wait) {
    if (teensy_uartn == 0 || teensy_uartn > TEENSY_UARTN_COUNT)
        return -1;

    int imx_bus = teensy_uart_get_imx_bus(teensy_uartn);

    if (init_bitflags & BITN(UART_INIT_BITS_TX_EN)) {
        if (teensy_set_pad_ctl(teensy_uart_get_tx_pad(teensy_uartn), -1, TEENSY_PAD_MODE_UART, wait) < 0)
            return -2;
        if (imx_bus != 1) { // LPUART1 does not have iomuxc input select
            // hopefully noone will ever see this atrocity
            volatile uint32_t* lpuart2p_tx_input_select = (imx_bus == 2) ? &iomuxc.input_select.lpuart2_tx : &iomuxc.input_select.lpuart3_cts_b;
            
            lpuart2p_tx_input_select[(imx_bus - 2) * 2] = teensy_get_uartn_isv(teensy_uartn);
            while (wait && (lpuart2p_tx_input_select[(imx_bus - 2) * 2] != teensy_get_uartn_isv(teensy_uartn))) {};
        }
    }

    if (init_bitflags & BITN(UART_INIT_BITS_RX_EN)) {
        if (teensy_set_pad_ctl(teensy_uart_get_rx_pad(teensy_uartn), -1, TEENSY_PAD_MODE_UART, wait) < 0) // maybe add hysteresis?
            return -3;
        if (imx_bus != 1) { // LPUART1 does not have iomuxc input select
            // cute ^2
            volatile uint32_t* lpuart2p_rx_input_select = (imx_bus == 2) ? &iomuxc.input_select.lpuart2_rx : &iomuxc.input_select.lpuart2_tx;
            
            lpuart2p_rx_input_select[(imx_bus - 2) * 2] = teensy_get_uartn_isv(teensy_uartn);
            while (wait && (lpuart2p_rx_input_select[(imx_bus - 2) * 2] != teensy_get_uartn_isv(teensy_uartn))) {};
        }
    }

    uart_init(imx_bus, baud, init_bitflags, wait);

    return 0;
}