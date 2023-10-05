#ifndef __GPIO_H__
#define __GPIO_H__

#include "aips.h"

#define GPIO_1_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->gpio1)
#define GPIO_2_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->gpio2) 
#define GPIO_3_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->gpio3) 
#define GPIO_4_OFFSET (&((aips_2_s *)AIPS_2_OFFSET)->gpio4) 
#define GPIO_5_OFFSET (&((aips_1_s *)AIPS_1_OFFSET)->gpio5) 
#define GPIO_6_OFFSET (&((aips_5_s *)AIPS_5_OFFSET)->gpio6) 
#define GPIO_7_OFFSET (&((aips_5_s *)AIPS_5_OFFSET)->gpio7) 
#define GPIO_8_OFFSET (&((aips_5_s *)AIPS_5_OFFSET)->gpio8)  
#define GPIO_9_OFFSET (&((aips_5_s *)AIPS_5_OFFSET)->gpio9)

#define GPIO_TCGPIO_OFFSET 5
#define gpio_get_bus_tightalt(bus) ((bus > 5) ? (bus - GPIO_TCGPIO_OFFSET) : ((bus == 0) ? 0 : ((bus < 5) ? (bus + GPIO_TCGPIO_OFFSET) : 5)))

// use _direct funcs only with abs args
#define gpio_get_bus_paddr_direct(bus) (GPIO_ ## bus ## _OFFSET)
#define gpio_read_direct(bus, port) (!!(((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->dr & BITN(port)))
#define gpio_set_direct(bus, port) (((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->dr_set = BITN(port))
#define gpio_clear_direct(bus, port) (((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->dr_clear = BITN(port))
#define gpio_toggle_direct(bus, port) (((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->dr_toggle = BITN(port))
#define gpio_set_indir_direct(bus, port) (((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->gdir &= ~(BITN(port)))
#define gpio_set_outdir_direct(bus, port) (((aips_gpio*)(gpio_get_bus_paddr_direct(bus)))->gdir |= BITN(port))

#define gpio_get_bus_paddr(bus) gpio_regs[bus]

extern aips_gpio* gpio_regs[10];

int gpio_port_mode(int bus, int port, bool output, bool wait);
int gpio_port_set(int bus, int port, bool wait);
int gpio_port_clear(int bus, int port, bool wait);
int gpio_port_toggle(int bus, int port, bool wait);
bool gpio_port_read(int bus, int port, bool wait, bool wait_target);

#endif