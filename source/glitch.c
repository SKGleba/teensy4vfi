#include "include/debug.h"
#include "include/clib.h"
#include "include/teensy.h"
#include "include/gpio.h"

#include "include/glitch.h"

void (*glitch_arm)(void) = (void*)GLITCH_DEFAULT_FUNC;

// uart mode does NOT configure the uart receiver, only the pad
int glitch_configure(glitch_config_s* config) {
    bool uart_mode = false;
    int driver_pad = (config->driver_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD);
    int trigger_pad = (config->trigger_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD);
    if (config->trigger_ctl & BITN(GLITCH_PAD_CTL_BITS_TRIGGER_UART_MODE)) {
        if (trigger_pad == 0 || trigger_pad > 7)
            return -1;
        trigger_pad = teensy_uart_get_rx_pad(trigger_pad);
        uart_mode = true;
    }

    // check args
    if (driver_pad > (TEENSY_PADS_COUNT - 1) || trigger_pad > (TEENSY_PADS_COUNT - 1))
        return -2;
    if (!config->offset || !config->offset_mult || !config->width)
        return -3;

    // configure mosfet driver
    int driver_pad_ctl = IOMUXC_PORT_CTL_FIELD(
        true,                                                                                                       // fast slew rate
        IOMUXC_PORT_CTL_DSE_R0(((config->driver_ctl >> GLITCH_PAD_CTL_BITS_DSE) & IOMUXC_PORT_CTL_BITMASK_DSE)),    // drive strength
        IOMUXC_PORT_CTL_SPEED(4),                                                                                   // 200Mhz
        false,                                                                                                      // dont set open-drain mode
        true,                                                                                                       // enable pull/keep
        !!(config->driver_ctl & BITN(GLITCH_PAD_CTL_BITS_PUE)),                                                     // pull?
        ((config->driver_ctl >> GLITCH_PAD_CTL_BITS_PUS) & IOMUXC_PORT_CTL_BITMASK_PUS),                            // pull strength/type
        !!(config->driver_ctl & BITN(GLITCH_PAD_CTL_BITS_HYS))                                                      // enable hysteresis
    );
    teensy_set_pad_ctl(driver_pad, driver_pad_ctl, TEENSY_PAD_MODE_GPIO, true);                                     // pad config
    teensy_pad_logic_ctrl_tightness(driver_pad, true, true);                                                        // change to tight-coupled gpio ctl
    teensy_pad_logic_mode(driver_pad, true, true);                                                                  // set as output

    // configure glitch trigger
    int trigger_pad_ctl = IOMUXC_PORT_CTL_FIELD(
        true,                                                                                                       // fast slew rate
        IOMUXC_PORT_CTL_DSE_R0(((config->trigger_ctl >> GLITCH_PAD_CTL_BITS_DSE) & IOMUXC_PORT_CTL_BITMASK_DSE)),   // drive strength
        IOMUXC_PORT_CTL_SPEED(4),                                                                                   // 200Mhz
        false,                                                                                                      // dont set open-drain mode
        true,                                                                                                       // enable pull/keep
        !!(config->trigger_ctl & BITN(GLITCH_PAD_CTL_BITS_PUE)),                                                    // pull?
        ((config->trigger_ctl >> GLITCH_PAD_CTL_BITS_PUS) & IOMUXC_PORT_CTL_BITMASK_PUS),                           // pull strength/type
        !!(config->trigger_ctl & BITN(GLITCH_PAD_CTL_BITS_HYS))                                                     // enable hysteresis
    );
    if (!uart_mode) {
        teensy_set_pad_ctl(trigger_pad, trigger_pad_ctl, TEENSY_PAD_MODE_GPIO, true);                               // pad cfg
        teensy_pad_logic_ctrl_tightness(trigger_pad, true, true);                                                   // change to tight-coupled gpio ctl
        teensy_pad_logic_mode(trigger_pad, false, true);                                                            // set as input
    } else
        teensy_set_pad_ctl(trigger_pad, trigger_pad_ctl, -1, true);

    // configure glitch params
    g_glitch_varray_main.offset = config->offset;                                                                   // wait period between trigger-drive
    g_glitch_varray_main.offset_mult = config->offset_mult;                                                         // multiplicator for the above
    g_glitch_varray_main.width = config->width;                                                                     // wait period between drive start-stop

    if (config->overrides.driver.mask)                                                                              // bitfield written to driver_set_reg and driver_clr_reg
        g_glitch_varray_main.driver_ports = config->overrides.driver.mask;
    else
        g_glitch_varray_main.driver_ports = BITN(teensy_get_pad_port(driver_pad));
    
    if (config->overrides.driver.set_to_drive)                                                                      // write a bitfield preset here to open the mosfet gate
        g_glitch_varray_main.driver_set_reg = config->overrides.driver.set_to_drive;
    else
        g_glitch_varray_main.driver_set_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(driver_pad))->dr_set);

    if (config->overrides.driver.set_to_stop)                                                                       // write a bitfield preset here to close the mosfet gate
        g_glitch_varray_main.driver_clr_reg = config->overrides.driver.set_to_stop;
    else
        g_glitch_varray_main.driver_clr_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(driver_pad))->dr_clear);

    if (config->overrides.trigger.mask) {                                                                           // mask and expected masked value from trigger_data_reg
        g_glitch_varray.trigger_ports = config->overrides.trigger.mask;
        g_glitch_varray.trigger_exp_state = config->overrides.trigger.exp_data;
    } else {
        if (uart_mode) {
            g_glitch_varray.trigger_ports = GLITCH_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK | BITN(UART_DATA_BITS_RXEMPT);
            g_glitch_varray.trigger_exp_state = ((config->trigger_ctl >> GLITCH_PAD_CTL_BITS_TRIGGER_UART_WATERMARK) & GLITCH_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK);
        } else {
            g_glitch_varray.trigger_ports = BITN(teensy_get_pad_port(trigger_pad));
            g_glitch_varray.trigger_exp_state = BITNVALM(teensy_get_pad_port(trigger_pad), !!(config->trigger_ctl & BITN(GLITCH_PAD_CTL_BITS_TRIGGER_STATE)), 1);
        }
    }

    if (config->overrides.trigger.get_to_drive)                                                                     // read, mask, compareok the bitfield to trigger the glitch
        g_glitch_varray.trigger_data_reg = config->overrides.trigger.get_to_drive;
    else {
        if (uart_mode)
            g_glitch_varray.trigger_data_reg = &uart_regs[teensy_uart_get_imx_bus((config->trigger_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD))]->data;
        else
            g_glitch_varray.trigger_data_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(trigger_pad))->dr);
    }

    // set clocks
    if (config->overrides.clockspeed)                                                                               // teensy core clock freq
        ccm_set_core_clkf(0, config->overrides.clockspeed);
    else
        ccm_set_core_clkf(0, GLITCH_DEFAULT_CLKSPEED);

    // set the glitch func
    glitch_arm = s_glitch;                                                                                          // s_glitch (logic level trigger,waitloop)

    return 0;
}

int glitch_configure_default(int type, uint32_t offset, uint32_t offset_mult, uint32_t width) {
    int ret = -1;
    glitch_config_s config;
    memset(&config, 0, sizeof(glitch_config_s));
    config.width = width;
    config.offset = offset;
    config.offset_mult = offset_mult;
    config.driver_ctl = GLITCH_DEFAULT_DRIVER_PAD
        | BITNVALM(GLITCH_PAD_CTL_BITS_DSE, GLITCH_DEFAULT_DRIVER_DSE_DIV, IOMUXC_PORT_CTL_BITMASK_DSE)
    ;
    switch (type) {
    case GLITCH_CONFIG_DEFAULT_TYPE_LOGIC_LEVEL:
        config.trigger_ctl = GLITCH_DEFAULT_LL_TRIGGER_PAD
            | BITNVALM(GLITCH_PAD_CTL_BITS_TRIGGER_STATE, GLITCH_DEFAULT_LL_TRIGGER_EXP_STATE, 1)
            | BITNVALM(GLITCH_PAD_CTL_BITS_DSE, GLITCH_DEFAULT_LL_TRIGGER_DSE_DIV, IOMUXC_PORT_CTL_BITMASK_DSE)
            | BITNVAL(GLITCH_PAD_CTL_BITS_PUE, GLITCH_DEFAULT_LL_TRIGGER_PULL_EN)
            | BITNVAL(GLITCH_PAD_CTL_BITS_PUS, GLITCH_DEFAULT_LL_TRIGGER_PULL_TYPE)
            | BITNVAL(GLITCH_PAD_CTL_BITS_HYS, GLITCH_DEFAULT_LL_TRIGGER_HYS_EN)
            ;
        ret = glitch_configure(&config);
        break;
    case GLITCH_CONFIG_DEFAULT_TYPE_UART:
        config.trigger_ctl = GLITCH_DEFAULT_UART_TRIGGER_UARTN
            | BITNVALM(GLITCH_PAD_CTL_BITS_TRIGGER_UART_WATERMARK, GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE, GLITCH_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK)
            | BITNVALM(GLITCH_PAD_CTL_BITS_DSE, GLITCH_DEFAULT_UART_TRIGGER_DSE_DIV, IOMUXC_PORT_CTL_BITMASK_DSE)
            | BITNVAL(GLITCH_PAD_CTL_BITS_PUE, GLITCH_DEFAULT_UART_TRIGGER_PULL_EN)
            | BITNVAL(GLITCH_PAD_CTL_BITS_PUS, GLITCH_DEFAULT_UART_TRIGGER_PULL_TYPE)
            | BITNVAL(GLITCH_PAD_CTL_BITS_HYS, GLITCH_DEFAULT_UART_TRIGGER_HYS_EN)
            | BITN(GLITCH_PAD_CTL_BITS_TRIGGER_UART_MODE)
            ;
        ret = glitch_configure(&config);
        if (ret >= 0) {
            teensy_uart_wait_tc(GLITCH_DEFAULT_UART_TRIGGER_UARTN);
            ret = teensy_uart_init( // TODO: should we actually have RX FIFO enabled?
                GLITCH_DEFAULT_UART_TRIGGER_UARTN,
                GLITCH_DEFAULT_UART_TRIGGER_BAUD,
                BITN(UART_INIT_BITS_RX_EN) | BITN(UART_INIT_BITS_RX_FIFO_EN) | BITNVAL(UART_INIT_BITS_TX_EN, GLITCH_DEFAULT_UART_TRIGGER_INIT_TX) | BITNVAL(UART_INIT_BITS_TX_FIFO_EN, GLITCH_DEFAULT_UART_TRIGGER_INIT_TX_FIFO),
                true
            );
        }
        break;
    }

    return ret;
}