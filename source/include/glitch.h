#ifndef __GLITCH_H__
#define __GLITCH_H__

#include "types.h"
#include "defs.h"
#include "teensy.h"

#define GLITCH_DEFAULT_CLKSPEED CCM_ARM_CLKF_600MHZ
#define GLITCH_DEFAULT_FUNC s_glitch
#define GLITCH_DEFAULT_OFF 300000000 // ~1s
#define GLITCH_DEFAULT_OFF_MULT 1
#define GLITCH_DEFAULT_WIDTH 600 // ~1us

#define GLITCH_DEFAULT_DRIVER_PAD 22
#define GLITCH_DEFAULT_DRIVER_DSE_DIV 6

#define GLITCH_DEFAULT_LL_TRIGGER_PAD 23
#define GLITCH_DEFAULT_LL_TRIGGER_EXP_STATE true
#define GLITCH_DEFAULT_LL_TRIGGER_PULL_EN true
#define GLITCH_DEFAULT_LL_TRIGGER_PULL_TYPE IOMUXC_PORT_CTL_PUS_100K_PULL_DOWN
#define GLITCH_DEFAULT_LL_TRIGGER_HYS_EN false
#define GLITCH_DEFAULT_LL_TRIGGER_DSE_DIV 6

#define GLITCH_DEFAULT_UART_TRIGGER_UARTN DEBUG_UARTN
#define GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE 0xD9
#define GLITCH_DEFAULT_UART_TRIGGER_PULL_EN true
#define GLITCH_DEFAULT_UART_TRIGGER_PULL_TYPE IOMUXC_PORT_CTL_PUS_22K_PULL_UP
#define GLITCH_DEFAULT_UART_TRIGGER_HYS_EN true
#define GLITCH_DEFAULT_UART_TRIGGER_DSE_DIV 7
#define GLITCH_DEFAULT_UART_TRIGGER_BAUD UART_BAUD_115200
#define GLITCH_DEFAULT_UART_TRIGGER_INIT_TX true
#define GLITCH_DEFAULT_UART_TRIGGER_INIT_TX_FIFO true

enum GLITCH_CONFIG_DEFAULT_TYPES {
    GLITCH_CONFIG_DEFAULT_TYPE_LOGIC_LEVEL = 0,
    GLITCH_CONFIG_DEFAULT_TYPE_UART,
};

enum GLITCH_PAD_CTL_BITS {
    GLITCH_PAD_CTL_BITS_TEENSY_PAD = 0,
    GLITCH_PAD_CTL_BITS_TEENSY_UARTN = 0,
    GLITCH_PAD_CTL_BITS_TRIGGER_UART_MODE = 8,
    GLITCH_PAD_CTL_BITS_DSE,
    GLITCH_PAD_CTL_BITS_PUE = 12, // 0: Keep | 1: Pull
    GLITCH_PAD_CTL_BITS_PUS,
    GLITCH_PAD_CTL_BITS_HYS = 15,
    GLITCH_PAD_CTL_BITS_TRIGGER_STATE = 16,
    GLITCH_PAD_CTL_BITS_TRIGGER_UART_WATERMARK = 16
};
#define GLITCH_PAD_CTL_BITMASK_TEENSY_PAD 0xff
#define GLITCH_PAD_CTL_BITMASK_TEENSY_UARTN 0xff
#define GLITCH_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK 0xff

struct _glitch_config_s {
    uint32_t width;
    uint32_t offset;
    uint32_t offset_mult;
    uint32_t trigger_ctl;
    uint32_t driver_ctl;
    struct { // custom overrides, enables stuff like multi-pad trigger
        uint32_t clockspeed;
        struct { // driver aka transistor gate ctl
            uint32_t mask; // mask, by default 1<<driver_port
            uint32_t* set_to_drive; // write [mask] here to open the gate
            uint32_t* set_to_stop; // write [mask] here to close the gate
        } driver;
        struct { // trigger details
            uint32_t mask; // mask, by default 1<<trigger_port
            uint32_t exp_data; // *[get_to_drive] & [mask] = trigger
            uint32_t* get_to_drive;
        } trigger;
    } overrides;
} __attribute__((packed));
typedef struct _glitch_config_s glitch_config_s;

struct _glitch_varray_main_s {
    uint32_t offset;
    uint32_t offset_mult;
    uint32_t width;
    uint32_t driver_ports;
    volatile uint32_t *driver_set_reg;
    volatile uint32_t *driver_clr_reg;
} __attribute__((packed));
typedef struct _glitch_varray_main_s glitch_varray_main_s;

struct _glitch_varray_s {
    uint32_t trigger_exp_state;
    uint32_t trigger_ports;
    volatile uint32_t* trigger_data_reg;
} __attribute__((packed));
typedef struct _glitch_varray_s glitch_varray_s;

extern glitch_varray_main_s g_glitch_varray_main;
extern glitch_varray_s g_glitch_varray;

// TODO: glitch_intr() with intrs 

extern void (*glitch_arm)(void);
void s_glitch(void);
int glitch_configure(glitch_config_s* config);
int glitch_configure_default(int type, uint32_t offset, uint32_t offset_mult, uint32_t width);

#endif