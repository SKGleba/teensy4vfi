#ifndef __RPC_H__
#define __RPC_H__

#include "types.h"

#define RPC_MAGIC '@'
#define RPC_WATERMARK "!PC_"

enum RPC_COMMANDS {
    RPC_CMD_NOP,
    RPC_CMD_READ32,
    RPC_CMD_WRITE32,
    RPC_CMD_MEMSET,
    RPC_CMD_MEMCPY,
    RPC_CMD_SET_DELAY,
    RPC_CMD_STOP_RPC,
    RPC_CMD_HEXDUMP,
    RPC_CMD_MEMSET32,
    RPC_CMD_GLITCH_PREP_LL,
    RPC_CMD_GLITCH_ARM,
    RPC_CMD_GLITCH_PREP_CUSTOM,
    RPC_CMD_GLITCH_PREP_UART,
    RPC_CMD_SET_CLK,
    RPC_CMD_GLITCH_PREP_CUSTOM_CHAIN,
    RPC_CMD_GLITCH_PREP_NONE,
    RPC_CMD_CUSTOM,
    RPC_CMD_UART_INIT,
    RPC_CMD_PAD_CONFIGURE,
    RPC_CMD_PAD_CTRL_LOGIC,
    RPC_CMD_GLITCH_SET_CHAIN_MAX,
    RPC_CMD_GET_SP
};

enum RPC_CMD_PAD_CTRL_LOGIC_FUNCS {
    RPC_CMD_PAD_CTRL_LOGIC_CLEAR = 0,
    RPC_CMD_PAD_CTRL_LOGIC_SET,
    RPC_CMD_PAD_CTRL_LOGIC_TOGGLE,
    RPC_CMD_PAD_CTRL_LOGIC_MODE,
    RPC_CMD_PAD_CTRL_LOGIC_READ,
    RPC_CMD_PAD_CTRL_LOGIC_TIGHTNESS
};

struct _rpc_cmd_s {
    uint8_t magic;
    uint8_t id;
    uint8_t data_size;
    uint8_t hash;
} __attribute__((packed));
typedef struct _rpc_cmd_s rpc_cmd_s;

void rpc_loop(void);

#endif