#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"

#include "include/iomuxc.h"

int iomuxc_set_tcgpio(int bus, int port, int mask, bool wait) {
    bool clear = false;
    
    if (!bus || (bus == 5) || (bus > 9))
        return -1;
    
    if (bus < 5) {
        bus -=- 5;
        clear = true;
    }

    if (!mask)
        mask = 1 << port;

    if (clear) {
        iomuxc_gpr[IOMUXC_GPR_GPIO_MUX1_GPIO_SEL + bus - 5 - 1] &= ~mask;
        dsb();
        while (wait && (iomuxc_gpr[IOMUXC_GPR_GPIO_MUX1_GPIO_SEL + bus - 5 - 1] & mask))
            ;
    } else {
        iomuxc_gpr[IOMUXC_GPR_GPIO_MUX1_GPIO_SEL + bus - 5 - 1] |= mask;
        dsb();
        while (wait && !(iomuxc_gpr[IOMUXC_GPR_GPIO_MUX1_GPIO_SEL + bus - 5 - 1] & mask))
            ;
    }

    return iomuxc_gpr[IOMUXC_GPR_GPIO_MUX1_GPIO_SEL + bus - 5 - 1];
}

/*
We can calculate default periph ctrls for pads based on their gpio ports & bus
    bus1 :
        - ports 0 - 15 -> ad_b0    [n]
        - ports 16 - 31 -> ad_b1   [n - 16]
    bus2 :
        - ports 0 - 15 -> b0       [n]
        - ports 16 - 31 -> b1      [n - 16]
    bus3 :
        - ports 0 - 11 -> sd_b1    [n]
        - ports 12 - 17 -> sd_b0   [n - 12]
        - ports 18 - 27 -> emc     [32 + (n - 18)]     <= only 27 gpio3 ports?
    bus4 : ports 0 - 31 -> emc     [n]
*/
volatile uint32_t* iomuxc_get_ctl_reg_for_port(bool mux_ctl, int port, int gpio_bus) {
    if (!gpio_bus || gpio_bus == 5 || gpio_bus > 9)
        return NULL;

    if (gpio_bus > 5)
        gpio_bus -= 5;

    if (port > 31 || (gpio_bus == 3 && port > 27))
        return NULL;

    switch (gpio_bus) {
    case 1:
        return (mux_ctl
            ? ((port >= 16) ? &iomuxc.sw_mux_ctl_pad_gpio.ad_b1[port - 16] : &iomuxc.sw_mux_ctl_pad_gpio.ad_b0[port])
            : ((port >= 16) ? &iomuxc.sw_pad_ctl_pad_gpio.ad_b1[port - 16] : &iomuxc.sw_pad_ctl_pad_gpio.ad_b0[port])
        );
    case 2:
        return (mux_ctl
            ? ((port >= 16) ? &iomuxc.sw_mux_ctl_pad_gpio.b1[port - 16] : &iomuxc.sw_mux_ctl_pad_gpio.b0[port])
            : ((port >= 16) ? &iomuxc.sw_pad_ctl_pad_gpio.b1[port - 16] : &iomuxc.sw_pad_ctl_pad_gpio.b0[port])
        );
    case 3:
        if (port >= 12) {
            if (port >= 18)
                return (mux_ctl ? &iomuxc.sw_mux_ctl_pad_gpio.emc[32 + (port - 18)] : &iomuxc.sw_pad_ctl_pad_gpio.emc[32 + (port - 18)]);
            else
                return (mux_ctl ? &iomuxc.sw_mux_ctl_pad_gpio.sd_b0[port - 12] : &iomuxc.sw_pad_ctl_pad_gpio.sd_b0[port - 12]);
        } else
            return (mux_ctl ? &iomuxc.sw_mux_ctl_pad_gpio.sd_b1[port] : &iomuxc.sw_pad_ctl_pad_gpio.sd_b1[port]);
    case 4:
        return (mux_ctl ? &iomuxc.sw_mux_ctl_pad_gpio.emc[port] : &iomuxc.sw_pad_ctl_pad_gpio.emc[port]);
    default: // _-_
        break;
    }

    return NULL; // how did we get here? lul
}

int iomuxc_set_port_ctl(int port, int gpio_bus, int ctl, int mux_ctl, bool wait) {
    if (ctl >= 0) { // -1 to skip
        volatile uint32_t* ctl_reg = iomuxc_get_ctl_reg_for_port(false, port, gpio_bus);
        if (!ctl_reg)
            return -1;
        *ctl_reg = ctl;
        while (wait && ((*ctl_reg & BITF(IOMUXC_PORT_CTL_BITS__END)) != (ctl & BITF(IOMUXC_PORT_CTL_BITS__END))))
            ;
    }

    if (mux_ctl >= 0) { // -1 to skip
        volatile uint32_t* mux_ctl_reg = iomuxc_get_ctl_reg_for_port(true, port, gpio_bus);
        if (!mux_ctl_reg)
            return -2;
        *mux_ctl_reg = mux_ctl;
        while (wait && ((*mux_ctl_reg & BITF(IOMUXC_PORT_MUX_CTL_BITS__END)) != (mux_ctl & BITF(IOMUXC_PORT_MUX_CTL_BITS__END))))
            ;
    }

    return 0;
}