#include "include/debug.h"
#include "include/clib.h"
#include "include/teensy.h"
#include "include/gpio.h"

#include "include/glitch.h"

int g_glitch_max_chain_n = GLITCH_STATIC_CHAIN_N;
glitch_varray_s* g_glitch_varray = g_static_glitch_varray;
glitch_varray_s g_static_glitch_varray[GLITCH_STATIC_CHAIN_N];

void (*glitch_arm)(glitch_varray_s* varray) = (void*)GLITCH_DEFAULT_FUNC;

// uart mode does NOT configure the uart receiver, only the pad
int glitch_configure(glitch_config_s* config, bool add_to_chain) {
    bool uart_mode = false;
    
    bool use_driver = !(config->driver_ctl & BITN(GLITCH_PAD_CTL_BITS_IGNORE));
    int driver_pad = (config->driver_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD);
    
    bool use_trigger = !(config->trigger_ctl & BITN(GLITCH_PAD_CTL_BITS_IGNORE));
    int trigger_pad = (config->trigger_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD);
    if (config->trigger_ctl & BITN(GLITCH_INPUT_PAD_CTL_BITS_UART_MODE)) {
        if (trigger_pad > 7)
            return -1;
        trigger_pad = teensy_uart_get_rx_pad(trigger_pad);
        uart_mode = true;
    }

    // check args
    if (driver_pad > (TEENSY_PADS_COUNT - 1) || trigger_pad > (TEENSY_PADS_COUNT - 1))
        return -2;
    if (!config->offset || !config->offset_mult || !config->width)
        return -3;

    // add to chain if requested
    glitch_varray_s* varray = g_glitch_varray;
    if (add_to_chain) {
        while (varray->next) {
            varray = varray->next;
            add_to_chain++;
        }
        if (add_to_chain >= g_glitch_max_chain_n)
            return -4;
        varray = varray->next = &g_glitch_varray[add_to_chain];
    }
    memset(varray, 0, sizeof(glitch_varray_s));

    // configure mosfet driver
    if (use_driver && (config->driver_ctl & GLITCH_PAD_CTL_BITS_RECONFIGURE)) {
        int driver_pad_ctl = IOMUXC_PORT_CTL_FIELD(
            true,                                                                                                   // fast slew rate
            IOMUXC_PORT_CTL_DSE_R0(((config->driver_ctl >> GLITCH_OUTPUT_PAD_CTL_BITS_DSE) & IOMUXC_PORT_CTL_BITMASK_DSE)),// drive strength
            IOMUXC_PORT_CTL_SPEED(4),                                                                               // 200Mhz
            false,                                                                                                  // TODO: support OD as actual glitcher?
            false,                                                                                                  // n/a
            false,                                                                                                  // n/a
            false,                                                                                                  // n/a
            false                                                                                                   // n/a
        );
        teensy_set_pad_ctl(driver_pad, driver_pad_ctl, TEENSY_PAD_MODE_GPIO, true);                                 // pad config
        teensy_pad_logic_ctrl_tightness(driver_pad, true, true);                                                    // change to tight-coupled gpio ctl
        teensy_pad_logic_mode(driver_pad, true, true);                                                              // set as output
    }
    
    // configure glitch trigger
    if (use_trigger && (config->trigger_ctl & GLITCH_PAD_CTL_BITS_RECONFIGURE)) {
        int trigger_pad_ctl = IOMUXC_PORT_CTL_FIELD(
            true,                                                                                                   // fast slew rate
            false,                                                                                                  // n/a
            IOMUXC_PORT_CTL_SPEED(4),                                                                               // 200Mhz, shouldnt matter anyways
            false,                                                                                                  // n/a
            !!(config->trigger_ctl & BITN(GLITCH_INPUT_PAD_CTL_BITS_PKE)),                                          // enable pull/keep
            !!(config->trigger_ctl & BITN(GLITCH_INPUT_PAD_CTL_BITS_PUE)),                                          // pull?
            ((config->trigger_ctl >> GLITCH_INPUT_PAD_CTL_BITS_PUS) & IOMUXC_PORT_CTL_BITMASK_PUS),                 // pull strength/type
            !!(config->trigger_ctl & BITN(GLITCH_INPUT_PAD_CTL_BITS_HYS))                                           // enable hysteresis
        );
        if (!uart_mode) {
            teensy_set_pad_ctl(trigger_pad, trigger_pad_ctl, TEENSY_PAD_MODE_GPIO, true);                           // pad cfg
            teensy_pad_logic_ctrl_tightness(trigger_pad, true, true);                                               // change to tight-coupled gpio ctl
            teensy_pad_logic_mode(trigger_pad, false, true);                                                        // set as input
        } else
            teensy_set_pad_ctl(trigger_pad, trigger_pad_ctl, -1, true);
    }
    
    // configure glitch params
    varray->offset = config->offset;                                                                                // wait period between trigger-drive
    varray->offset_mult = config->offset_mult;                                                                      // multiplicator for the above
    varray->width = config->width;                                                                                  // wait period between drive start-stop

    if (config->overrides.driver.mask)                                                                              // bitfield written to driver_set_reg and driver_clr_reg
        varray->driver_ports = config->overrides.driver.mask;
    else if (use_driver)
        varray->driver_ports = BITN(teensy_get_pad_port(driver_pad));
    
    if (config->overrides.driver.set_to_drive)                                                                      // write a bitfield preset here to open the mosfet gate
        varray->driver_set_reg = config->overrides.driver.set_to_drive;
    else if (use_driver)
        varray->driver_set_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(driver_pad))->dr_set);
    else
        varray->driver_set_reg = &varray->driver_ports;

    if (config->overrides.driver.set_to_stop)                                                                       // write a bitfield preset here to close the mosfet gate
        varray->driver_clr_reg = config->overrides.driver.set_to_stop;
    else if (use_driver)
        varray->driver_clr_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(driver_pad))->dr_clear);
    else
        varray->driver_clr_reg = &varray->driver_ports;

    if (config->overrides.trigger.mask) {                                                                           // mask and expected masked value from trigger_data_reg
        varray->trigger_ports = config->overrides.trigger.mask;
        varray->trigger_exp_state = config->overrides.trigger.exp_data;
    } else {
        if (uart_mode) {
            varray->trigger_ports = GLITCH_INPUT_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK | BITN(UART_DATA_BITS_RXEMPT);
            varray->trigger_exp_state = ((config->trigger_ctl >> GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_UART_WATERMARK) & GLITCH_INPUT_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK);
        } else if (use_trigger) {
            varray->trigger_ports = BITN(teensy_get_pad_port(trigger_pad));
            varray->trigger_exp_state = BITNVALM(teensy_get_pad_port(trigger_pad), !!(config->trigger_ctl & BITN(GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_STATE)), 1);
        }
    }

    if (config->overrides.trigger.get_to_drive)                                                                     // read, mask, compareok the bitfield to trigger the glitch
        varray->trigger_data_reg = config->overrides.trigger.get_to_drive;
    else {
        if (uart_mode)
            varray->trigger_data_reg = &uart_regs[teensy_uart_get_imx_bus((config->trigger_ctl & GLITCH_PAD_CTL_BITMASK_TEENSY_PAD))]->data;
        else if (use_trigger)
            varray->trigger_data_reg = &(gpio_get_bus_paddr(teensy_get_pad_gpio_bus(trigger_pad))->dr);
        else
            varray->trigger_data_reg = &varray->trigger_ports;
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

int glitch_configure_default(int type, uint32_t offset, uint32_t offset_mult, uint32_t width, int trigger_pad, int trigger_state, int driver_pad) {
    int ret = -1;
    glitch_config_s config;
    memset(&config, 0, sizeof(glitch_config_s));
    config.width = width;
    config.offset = offset;
    config.offset_mult = offset_mult;
    
    if (!driver_pad)
        driver_pad = GLITCH_DEFAULT_DRIVER_PAD;
    if (type & BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NODRIVER))
        config.driver_ctl = BITN(GLITCH_PAD_CTL_BITS_IGNORE);
    else {
        config.driver_ctl = BITNVALM(GLITCH_PAD_CTL_BITS_TEENSY_PAD, driver_pad, GLITCH_PAD_CTL_BITMASK_TEENSY_PAD)
            | BITN(GLITCH_PAD_CTL_BITS_RECONFIGURE)
            | BITNVALM(GLITCH_OUTPUT_PAD_CTL_BITS_DSE, GLITCH_DEFAULT_DRIVER_DSE_DIV, IOMUXC_PORT_CTL_BITMASK_DSE)
            ;
    }
    
    if (type & BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NOTRIGGER)) {
        config.trigger_ctl = BITN(GLITCH_PAD_CTL_BITS_IGNORE);
        ret = glitch_configure(&config, !!(type & BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN)));
    } else {
        switch (type & GLITCH_CONFIG_DEFAULT_TYPE_BITMASK_MODES) {
        case BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_LOGIC_LEVEL):
            if (!trigger_pad) {
                trigger_pad = GLITCH_DEFAULT_LL_TRIGGER_PAD;
                trigger_state = GLITCH_DEFAULT_LL_TRIGGER_EXP_STATE;
            }
            config.trigger_ctl = BITNVALM(GLITCH_PAD_CTL_BITS_TEENSY_PAD, trigger_pad, GLITCH_PAD_CTL_BITMASK_TEENSY_PAD)
                | BITN(GLITCH_PAD_CTL_BITS_RECONFIGURE)
                | BITNVALM(GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_STATE, trigger_state, 1)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PKE, GLITCH_DEFAULT_LL_TRIGGER_PK_EN)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PUE, GLITCH_DEFAULT_LL_TRIGGER_PUE)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PUS, GLITCH_DEFAULT_LL_TRIGGER_PULL_TYPE)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_HYS, GLITCH_DEFAULT_LL_TRIGGER_HYS_EN)
                ;
            ret = glitch_configure(&config, !!(type & BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN)));
            break;
        case BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_UART):
            if (!trigger_pad) {
                trigger_pad = GLITCH_DEFAULT_UART_TRIGGER_UARTN;
                trigger_state = GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE;
            }
            config.trigger_ctl = BITNVALM(GLITCH_PAD_CTL_BITS_TEENSY_PAD, trigger_pad, GLITCH_PAD_CTL_BITMASK_TEENSY_PAD)
                | BITN(GLITCH_PAD_CTL_BITS_RECONFIGURE)
                | BITNVALM(GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_UART_WATERMARK, trigger_state, GLITCH_INPUT_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PKE, GLITCH_DEFAULT_UART_TRIGGER_PK_EN)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PUE, GLITCH_DEFAULT_UART_TRIGGER_PUE)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_PUS, GLITCH_DEFAULT_UART_TRIGGER_PULL_TYPE)
                | BITNVAL(GLITCH_INPUT_PAD_CTL_BITS_HYS, GLITCH_DEFAULT_UART_TRIGGER_HYS_EN)
                | BITN(GLITCH_INPUT_PAD_CTL_BITS_UART_MODE)
                ;
            ret = glitch_configure(&config, !!(type & BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN)));
            if (ret >= 0) {
                if (trigger_pad == DEBUG_UARTN)
                    teensy_uart_wait_tc(trigger_pad);
                ret = teensy_uart_init( // TODO: should we actually have RX FIFO enabled?
                    trigger_pad,
                    GLITCH_DEFAULT_UART_TRIGGER_BAUD,
                    BITN(UART_INIT_BITS_RX_EN) | BITN(UART_INIT_BITS_RX_FIFO_EN) | BITNVAL(UART_INIT_BITS_TX_EN, GLITCH_DEFAULT_UART_TRIGGER_INIT_TX) | BITNVAL(UART_INIT_BITS_TX_FIFO_EN, GLITCH_DEFAULT_UART_TRIGGER_INIT_TX_FIFO),
                    true
                );
            }
            break;
        default:
            break;
        }
    }

    return ret;
}