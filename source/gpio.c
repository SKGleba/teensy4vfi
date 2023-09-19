#include "include/paddr.h"
#include "include/aips.h"
#include "include/utils.h"

#include "include/gpio.h"

aips_gpio* gpio_regs[10] = { // legacy support
    NULL, // no gpio0
    gpio_get_bus_paddr_direct(1),
    gpio_get_bus_paddr_direct(2),
    gpio_get_bus_paddr_direct(3),
    gpio_get_bus_paddr_direct(4),
    gpio_get_bus_paddr_direct(5),
    gpio_get_bus_paddr_direct(6),
    gpio_get_bus_paddr_direct(7),
    gpio_get_bus_paddr_direct(8),
    gpio_get_bus_paddr_direct(9)
};

int gpio_port_mode(int bus, int port, bool output, bool wait) {
    if (!gpio_regs[bus])
        return -1;
    if (output) {
        gpio_regs[bus]->gdir |= 1 << port;
        while (wait && !(gpio_regs[bus]->gdir & (1 << port)))
            ;
    } else {
        gpio_regs[bus]->gdir &= ~(1 << port);
        while (wait && (gpio_regs[bus]->gdir & (1 << port)))
            ;
    }

    return 0;
}

int gpio_port_set(int bus, int port, bool wait) {
    if (!gpio_regs[bus])
        return -1;
    
    if (gpio_regs[bus]->gdir & (1 << port)) {
        gpio_regs[bus]->dr_set = 1 << port;
        while (wait && !(gpio_regs[bus]->dr & (1 << port)))
            ;
        return 0;
    }
    
    return -2;
}

int gpio_port_clear(int bus, int port, bool wait) {
    if (!gpio_regs[bus])
        return -1;
    
    if (gpio_regs[bus]->gdir & (1 << port)) {
        gpio_regs[bus]->dr_clear = 1 << port;
        while (wait && (gpio_regs[bus]->dr & (1 << port)))
            ;
        return 0;
    }
    
    return -2;
}

int gpio_port_toggle(int bus, int port, bool wait) {
    if (!gpio_regs[bus])
        return -1;

    if (gpio_regs[bus]->gdir & (1 << port)) {
        bool prev = gpio_regs[bus]->dr & (1 << port);
        gpio_regs[bus]->dr_toggle = 1 << port;
        while (wait && (prev == (gpio_regs[bus]->dr & (1 << port)))) {};
        return 0;
    }

    return -2;
}

bool gpio_port_read(int bus, int port, bool wait, bool wait_target) {
    if (!gpio_regs[bus])
        return -1;

    if (!(gpio_regs[bus]->gdir & BITN(port))) {
        bool state = gpio_regs[bus]->dr & BITN(port);
        if (wait) {
            if (wait_target > 1) // dont use this unless you know what you are doing [timings]
                wait_target = !state;
            while (1) {
                state = gpio_regs[bus]->dr & BITN(port);
                if (state == wait_target)
                    break;
            };
        }
        return state;
    }

    return -2;
}