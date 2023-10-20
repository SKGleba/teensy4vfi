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
    RPC_CMD_CUSTOM
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