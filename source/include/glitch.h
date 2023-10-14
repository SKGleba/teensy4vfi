#ifndef __GLITCH_H__
#define __GLITCH_H__

#include "types.h"
#include "defs.h"
#include "teensy.h"

#define GLITCH_STATIC_CHAIN_N 16 // for static mem alloc

#define GLITCH_DEFAULT_CLKSPEED 600000000 // 600Mhz
#define GLITCH_DEFAULT_FUNC s_glitch
#define GLITCH_DEFAULT_OFF 300000000 // ~1s
#define GLITCH_DEFAULT_OFF_MULT 1
#define GLITCH_DEFAULT_WIDTH 600 // ~1us

#define GLITCH_DEFAULT_DRIVER_PAD 22
#define GLITCH_DEFAULT_DRIVER_DSE_DIV 7

#define GLITCH_DEFAULT_LL_TRIGGER_PAD 23
#define GLITCH_DEFAULT_LL_TRIGGER_EXP_STATE true // trigger on hi
#define GLITCH_DEFAULT_LL_TRIGGER_PK_EN false // disable pull/keep
#define GLITCH_DEFAULT_LL_TRIGGER_PUE false // keeper
#define GLITCH_DEFAULT_LL_TRIGGER_PULL_TYPE 0 // disabled
#define GLITCH_DEFAULT_LL_TRIGGER_HYS_EN false // no hysteresis

/* enhanced precision?
#define GLITCH_DEFAULT_LL_TRIGGER_PAD 23
#define GLITCH_DEFAULT_LL_TRIGGER_EXP_STATE false // trigger on lo
#define GLITCH_DEFAULT_LL_TRIGGER_PK_EN true // disable pull/keep
#define GLITCH_DEFAULT_LL_TRIGGER_PUE true // pull
#define GLITCH_DEFAULT_LL_TRIGGER_PULL_TYPE IOMUXC_PORT_CTL_PUS_22K_PULL_UP
#define GLITCH_DEFAULT_LL_TRIGGER_HYS_EN false // no hysteresis
*/

#define GLITCH_DEFAULT_UART_TRIGGER_UARTN DEBUG_UARTN
#define GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE 0xD9
#define GLITCH_DEFAULT_UART_TRIGGER_PK_EN true
#define GLITCH_DEFAULT_UART_TRIGGER_PUE true // pull
#define GLITCH_DEFAULT_UART_TRIGGER_PULL_TYPE IOMUXC_PORT_CTL_PUS_22K_PULL_UP
#define GLITCH_DEFAULT_UART_TRIGGER_HYS_EN true
#define GLITCH_DEFAULT_UART_TRIGGER_BAUD UART_BAUD_115200
#define GLITCH_DEFAULT_UART_TRIGGER_INIT_TX true
#define GLITCH_DEFAULT_UART_TRIGGER_INIT_TX_FIFO true

enum GLITCH_CONFIG_DEFAULT_TYPES_BITS {
    GLITCH_CONFIG_DEFAULT_TYPE_BITS_LOGIC_LEVEL = 0,
    GLITCH_CONFIG_DEFAULT_TYPE_BITS_UART,
    GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN = 4,
    GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NODRIVER,
    GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NOTRIGGER
};
#define GLITCH_CONFIG_DEFAULT_TYPE_BITMASK_MODES 0b1111

enum GLITCH_PAD_CTL_BITS {
    GLITCH_PAD_CTL_BITS_TEENSY_PAD = 0,
    GLITCH_PAD_CTL_BITS_IGNORE = 6,
    GLITCH_PAD_CTL_BITS_RECONFIGURE // apply config
};
#define GLITCH_PAD_CTL_BITMASK_TEENSY_PAD 0x3f

enum GLITCH_INPUT_PAD_CTL_BITS {
    GLITCH_INPUT_PAD_CTL_BITS_TEENSY_UARTN = 0,
    GLITCH_INPUT_PAD_CTL_BITS_UART_MODE = 8,
    GLITCH_INPUT_PAD_CTL_BITS_PKE, // enable pull/keep
    GLITCH_INPUT_PAD_CTL_BITS_PUE, // 0: Keep | 1: Pull
    GLITCH_INPUT_PAD_CTL_BITS_PUS, // pull/keep strength
    GLITCH_INPUT_PAD_CTL_BITS_HYS = 13, // enable hysteresis
    GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_STATE = 16,
    GLITCH_INPUT_PAD_CTL_BITS_TRIGGER_UART_WATERMARK = 16
};
#define GLITCH_INPUT_PAD_CTL_BITMASK_TEENSY_UARTN GLITCH_PAD_CTL_BITMASK_TEENSY_PAD
#define GLITCH_INPUT_PAD_CTL_BITMASK_TEENSY_UART_WATERMARK 0xff
enum GLITCH_OUTPUT_PAD_CTL_BITS {
    GLITCH_OUTPUT_PAD_CTL_BITS_ODE = 8, // open-drain mode (TODO: support)
    GLITCH_OUTPUT_PAD_CTL_BITS_DSE, // drive strength
};

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

struct _glitch_varray_s {
    uint32_t offset;
    uint32_t offset_mult;
    uint32_t width;
    uint32_t driver_ports;
    volatile uint32_t* driver_set_reg;
    volatile uint32_t* driver_clr_reg;
    uint32_t trigger_exp_state;
    uint32_t trigger_ports;
    volatile uint32_t* trigger_data_reg;
    void* next;
};
typedef struct _glitch_varray_s glitch_varray_s;

extern glitch_varray_s g_glitch_varray[GLITCH_STATIC_CHAIN_N];

// TODO: glitch_intr() with intrs 

extern void (*glitch_arm)(glitch_varray_s *varray);
void s_glitch(glitch_varray_s* varray);
int glitch_configure(glitch_config_s* config, bool add_to_chain);
int glitch_configure_default(int type, uint32_t offset, uint32_t offset_mult, uint32_t width, int trigger_pad, int trigger_state, int driver_pad);

#endif