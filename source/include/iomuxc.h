#ifndef __IOMUXC_H__
#define __IOMUXC_H__

#include "aips.h"

#define iomuxc_gpr ((aips_1_s*)AIPS_1_OFFSET)->iomuxc_gpr.gpr
#define iomuxc ((aips_2_s*)AIPS_2_OFFSET)->iomuxc

#define IOMUXC_GPR_GPIO_MUX1_GPIO_SEL 26
#define IOMUXC_GPR_GPIO_MUX2_GPIO_SEL 27
#define IOMUXC_GPR_GPIO_MUX3_GPIO_SEL 28
#define IOMUXC_GPR_GPIO_MUX4_GPIO_SEL 29

enum IOMUXC_PORT_MUX_CTL_BITS {
    IOMUXC_PORT_MUX_CTL_BITS_MUX_MODE = 0,
    IOMUXC_PORT_MUX_CTL_BITS_SION = 4,
    IOMUXC_PORT_MUX_CTL_BITS__END
};

#define IOMUXC_PORT_MUX_CTL_BITMASK_MUX_MODE 0b1111 // sometimes 0b111

enum IOMUXC_PORT_CTL_BITS {
    IOMUXC_PORT_CTL_BITS_SRE = 0, // Fast Slew Rate
    IOMUXC_PORT_CTL_BITS_DSE = 3, // Drive Strength
    IOMUXC_PORT_CTL_BITS_SPEED = 6,
    IOMUXC_PORT_CTL_BITS_ODE = 11, // Open Drain Enabled
    IOMUXC_PORT_CTL_BITS_PKE, // Pull/Keep Enable
    IOMUXC_PORT_CTL_BITS_PUE, // Pull ^
    IOMUXC_PORT_CTL_BITS_PUS,
    IOMUXC_PORT_CTL_BITS_HYS = 16, // Hysteresis Enabled
    IOMUXC_PORT_CTL_BITS__END
};

enum IOMUXC_PORT_CTL_PUS_MODES { // Pull Up/Down config
    IOMUXC_PORT_CTL_PUS_100K_PULL_DOWN = 0,
    IOMUXC_PORT_CTL_PUS_47K_PULL_UP,
    IOMUXC_PORT_CTL_PUS_100K_PULL_UP,
    IOMUXC_PORT_CTL_PUS_22K_PULL_UP
};

#define IOMUXC_PORT_CTL_DSE_DISABLED 0
#define IOMUXC_PORT_CTL_DSE_R0(div) (div) // R0(150 Ohm @ 3.3V, 260 Ohm@1.8V), max div 7
#define IOMUXC_PORT_CTL_SPEED(mult) (mult - 1) // mult * 50Mhz

#define IOMUXC_PORT_CTL_BITMASK_DSE 0b111
#define IOMUXC_PORT_CTL_BITMASK_SPEED 0b11
#define IOMUXC_PORT_CTL_BITMASK_PUS 0b11

// ehh
#define IOMUXC_PORT_CTL_FIELD(sre, dse, speed, ode, pke, pue, pus, hys) ( \
    BITNVALM(IOMUXC_PORT_CTL_BITS_SRE, sre, 1) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_DSE, dse, IOMUXC_PORT_CTL_BITMASK_DSE) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_SPEED, speed, IOMUXC_PORT_CTL_BITMASK_SPEED) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_ODE, ode, 1) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_PKE, pke, 1) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_PUE, pue, 1) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_PUS, pus, IOMUXC_PORT_CTL_BITMASK_PUS) \
        | BITNVALM(IOMUXC_PORT_CTL_BITS_HYS, hys, 1) \
)

/*
    enable tcgpio [bus] [port]
        to set tcgpio->gpio give the func a gpio [bus]
    if you want to set multiple ports, set the [mask] bitfield
*/
int iomuxc_set_tcgpio(int bus, int port, int mask, bool wait);

volatile uint32_t* iomuxc_get_ctl_reg_for_port(bool mux_ctl, int port, int gpio_bus);
// TODO: iomuxc_get_ctl_reg_for_port_direct ?
#define iomuxc_port_ctl_mux(port, gpio_bus) vp(iomuxc_get_ctl_reg_for_port(true, port, gpio_bus))
#define iomuxc_port_ctl(port, gpio_bus) vp(iomuxc_get_ctl_reg_for_port(false, port, gpio_bus))
int iomuxc_set_port_ctl(int port, int gpio_bus, int ctl, int mux_ctl, bool wait);

#endif