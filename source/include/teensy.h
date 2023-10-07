#ifndef __TEENSY_H__
#define __TEENSY_H__

#include "gpio.h"
#include "uart.h"
#include "iomuxc.h"

#include "teensy_pads.h" // pad defs bloat separately

// customs
#define TEENSY_PAD_ORANGE_LED 13
#define TEENSY_PAD_UART1_RX 0
#define TEENSY_PAD_UART1_TX 1
#define TEENSY_PAD_UART2_RX 7
#define TEENSY_PAD_UART2_TX 8
#define TEENSY_PAD_UART3_RX 15
#define TEENSY_PAD_UART3_TX 14
#define TEENSY_PAD_UART4_RX 16
#define TEENSY_PAD_UART4_TX 17
#define TEENSY_PAD_UART5_RX 21
#define TEENSY_PAD_UART5_TX 20
#define TEENSY_PAD_UART6_RX 25
#define TEENSY_PAD_UART6_TX 24
#define TEENSY_PAD_UART7_RX 28
#define TEENSY_PAD_UART7_TX 29

// generic mux modes
#define TEENSY_PAD_MODE_UART 2
#define TEENSY_PAD_MODE_GPIO 5

// UART stuff
#define TEENSY_UART1_IMX_BUS 6
#define TEENSY_UART2_IMX_BUS 4
#define TEENSY_UART3_IMX_BUS 2
#define TEENSY_UART4_IMX_BUS 3
#define TEENSY_UART5_IMX_BUS 8
#define TEENSY_UART6_IMX_BUS 1
#define TEENSY_UART7_IMX_BUS 7
#define teensy_get_uartn_isv(uartn) ((0x1f10121f >> (uartn * 4)) & 0xf)

// use _direct funcs only with abs args
#define teensy_get_pad_port_direct(pad) TEENSY_PAD_ ## pad ## _PORT
#define teensy_get_pad_gpio_bus_direct(pad) TEENSY_PAD_ ## pad ## _BUS
#define teensy_get_pad_gpio_tcbus_direct(pad) TEENSY_PAD_ ## pad ## _BUS_TCG
#define teensy_pad_logic_set_direct(pad) gpio_set_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_pad_logic_read_direct(pad) gpio_read_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_pad_logic_clear_direct(pad) gpio_clear_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_pad_logic_toggle_direct(pad) gpio_toggle_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_pad_logic_set_indir_direct(pad) gpio_set_indir_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_pad_logic_set_outdir_direct(pad) gpio_set_outdir_direct(teensy_get_pad_gpio_bus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_set_direct(pad) gpio_set_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_read_direct(pad) gpio_read_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_clear_direct(pad) gpio_clear_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_toggle_direct(pad) gpio_toggle_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_set_indir_direct(pad) gpio_set_indir_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))
#define teensy_tcpad_logic_set_outdir_direct(pad) gpio_set_outdir_direct(teensy_get_pad_gpio_tcbus_direct(pad), teensy_get_pad_port_direct(pad))

// wrappers
#define teensy_get_pad_port(pad) teensy_pad_to_port[pad]
#define teensy_get_pad_gpio_bus(pad) teensy_pad_to_gpio_bus[pad]
#define teensy_pad_logic_set(pad, wait) gpio_port_set(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), wait)
#define teensy_pad_logic_clear(pad, wait) gpio_port_clear(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), wait)
#define teensy_pad_logic_toggle(pad, wait) gpio_port_toggle(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), wait)
#define teensy_pad_logic_mode(pad, output, wait) gpio_port_mode(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), output, wait)
#define teensy_pad_logic_read(pad, wait, wait_target) gpio_port_read(teensy_get_pad_gpio_bus(pad), teensy_get_pad_port(pad), wait, wait_target)
#define teensy_pad_ctl(pad) iomuxc_port_ctl(teensy_get_pad_port(pad), teensy_get_pad_gpio_bus(pad))
#define teensy_pad_ctl_mux(pad) iomuxc_port_ctl_mux(teensy_get_pad_port(pad), teensy_get_pad_gpio_bus(pad))
#define teensy_set_pad_ctl(pad, ctl, mux_ctl, wait) iomuxc_set_port_ctl(teensy_get_pad_port(pad), teensy_get_pad_gpio_bus(pad), ctl, mux_ctl, wait)
#define teensy_uart_get_imx_bus(uartn) teensy_uartn_to_imxbus_rx_tx[uartn][0]
#define teensy_uart_get_rx_pad(uartn) teensy_uartn_to_imxbus_rx_tx[uartn][1]
#define teensy_uart_get_tx_pad(uartn) teensy_uartn_to_imxbus_rx_tx[uartn][2]
#define teensy_uart_write(uartn, data) uart_write(teensy_uart_get_imx_bus(uartn), data)
#define teensy_uart_print(uartn, str) uart_print(teensy_uart_get_imx_bus(uartn), str)
#define teensy_uart_printn(uartn, str, n) uart_printn(teensy_uart_get_imx_bus(uartn), str, n)
#define teensy_uart_read(uartn, timeout, wait) uart_read(teensy_uart_get_imx_bus(uartn), timeout, wait)
#define teensy_uart_scann(uartn, out, out_size, timeout) uart_scann(teensy_uart_get_imx_bus(uartn), out, out_size, timeout)
#define teensy_uart_scanns(uartn, out, out_size, timeout) uart_scanns(teensy_uart_get_imx_bus(uartn), out, out_size, timeout)
#define teensy_uart_rxfifo_flush(uartn) uart_rxfifo_flush(teensy_uart_get_imx_bus(uartn))
#define teensy_uart_txfifo_flush(uartn) uart_txfifo_flush(teensy_uart_get_imx_bus(uartn))
#define teensy_uart_wait_tc(uartn) uart_wait_tc(teensy_uart_get_imx_bus(uartn))

extern const uint8_t teensy_pad_to_port[40];
extern uint8_t teensy_pad_to_gpio_bus[40];
extern uint8_t teensy_uartn_to_imxbus_rx_tx[8][3];

void teensy_pad_logic_ctrl_tightness(int pad, bool tight, bool wait);
int teensy_uart_init(int teensy_uartn, int baud, int init_bitflags, bool wait);

#endif