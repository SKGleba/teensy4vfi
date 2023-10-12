#include "include/defs.h"
#include "include/clib.h"
#include "include/debug.h"
#include "include/utils.h"
#include "include/glitch.h"

#include "include/rpc.h"

void rpc_loop(void) {
    uint32_t cret;
    rpc_cmd_s cmd;
    uint32_t data[0x10];
    uint32_t reply_delay = RPC_WRITE_DELAY;
    printf("entering RPC mode\n");
    while (true) {
        memset(&cmd, 0, sizeof(rpc_cmd_s));
        memset(data, 0, sizeof(data));
        rxflush();
        print(RPC_WATERMARK "G0\n");
        scanb(&cmd, sizeof(rpc_cmd_s));
        if (cmd.magic != RPC_MAGIC || cmd.hash != (cmd.id + cmd.data_size)) {
            print(RPC_WATERMARK "E1\n");
            continue;
        }
        if (cmd.data_size) {
            print(RPC_WATERMARK "G1\n");
            scanb(data, cmd.data_size);
        }

        cret = -1;
        switch (cmd.id) {
        case RPC_CMD_NOP:
            cret = get_build_timestamp();
            break;
        case RPC_CMD_READ32:
            cret = vp data[0];
            break;
        case RPC_CMD_WRITE32:
            vp(data[0]) = data[1];
            cret = 0;
            break;
        case RPC_CMD_MEMSET:
            cret = (uint32_t)memset((void*)data[0], data[1] & 0xFF, data[2]);
            break;
        case RPC_CMD_MEMCPY:
            cret = (uint32_t)memcpy((void*)data[0], (void*)data[1], data[2]);
            break;
        case RPC_CMD_SET_DELAY:
            cret = reply_delay;
            reply_delay = data[0];
            break;
        case RPC_CMD_STOP_RPC:
            cret = 0;
            break;
        case RPC_CMD_HEXDUMP:
            hexdump(data[0], data[1], data[2]);
            cret = 0;
            break;
        case RPC_CMD_MEMSET32:
            cret = (uint32_t)memset32((void*)data[0], data[1], data[2]);
            break;
        case RPC_CMD_GLITCH_PREP_LL:
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_LOGIC_LEVEL) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2]);
            break;
        case RPC_CMD_GLITCH_ARM:
            if (data[0])
                printf(RPC_WATERMARK "ARMED!\n", cret);
            glitch_arm(g_glitch_varray);
            cret = 0;
            break;
        case RPC_CMD_GLITCH_PREP_CUSTOM:
            cret = glitch_configure((glitch_config_s*)data, false);
            break;
        case RPC_CMD_GLITCH_PREP_UART:
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_UART) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2]);
            break;
        case RPC_CMD_SET_CLK:
            cret = ccm_set_core_clkf(data[1], data[0]);
            break;
        case RPC_CMD_GLITCH_PREP_CUSTOM_CHAIN:
            cret = glitch_configure((glitch_config_s*)data, true);
            break;
        case RPC_CMD_GLITCH_PREP_NONE:
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NOTRIGGER) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2]);
            break;
        default:
            break;
        }

        delay(reply_delay);
        printf(RPC_WATERMARK "%X\n", cret);

        if (cmd.id == RPC_CMD_STOP_RPC)
            break;
    }

    printf("exiting RPC mode\n");
}