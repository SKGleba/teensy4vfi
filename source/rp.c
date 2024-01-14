#include "include/defs.h"
#include "include/clib.h"
#include "include/debug.h"
#include "include/utils.h"
#include "include/glitch.h"
#include "include/custom.h"
#include "include/cfg.h"

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
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_LOGIC_LEVEL) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2], data[4], data[5], data[6]);
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
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_UART) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2], data[4], data[5], data[6]);
            break;
        case RPC_CMD_SET_CLK:
            if (data[2])
                cret = g_glitch_clkf = data[1] ?: ccm_calculate_core_clkf(data[0]);
            else
                cret = ccm_set_core_clkf(data[1], data[0]);
            break;
        case RPC_CMD_GLITCH_PREP_CUSTOM_CHAIN:
            cret = glitch_configure((glitch_config_s*)data, true);
            break;
        case RPC_CMD_GLITCH_PREP_NONE:
            cret = glitch_configure_default(BITN(GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_NOTRIGGER) | (data[3] << GLITCH_CONFIG_DEFAULT_TYPE_BITS_FLAG_CHAIN), data[0], data[1], data[2], 0, 0, data[4]);
            break;
        case RPC_CMD_CUSTOM:
            cret = (uint32_t)custom_main(data[0], data[1], data[2], data);
            break;
        case RPC_CMD_UART_INIT:
            cret = (uint32_t)teensy_uart_init(data[0], data[1], data[2], true);
            break;
        case RPC_CMD_PAD_CONFIGURE:
            cret = (uint32_t)teensy_set_pad_ctl(data[0], data[1], data[2], true);
            break;
        case RPC_CMD_PAD_CTRL_LOGIC: // arg0: func, arg1: pad, arg2: wait
            cret = 0;
            switch (data[0]) {
            case RPC_CMD_PAD_CTRL_LOGIC_CLEAR:
                teensy_pad_logic_clear(data[1], data[2]);
                break;
            case RPC_CMD_PAD_CTRL_LOGIC_SET:
                teensy_pad_logic_set(data[1], data[2]);
                break;
            case RPC_CMD_PAD_CTRL_LOGIC_TOGGLE:
                teensy_pad_logic_toggle(data[1], data[2]);
                break;
            case RPC_CMD_PAD_CTRL_LOGIC_MODE: // arg3: output?
                cret = teensy_pad_logic_mode(data[1], data[3], data[2]);
                break;
            case RPC_CMD_PAD_CTRL_LOGIC_READ: // arg3: wait target
                cret = teensy_pad_logic_read(data[1], data[2], data[3]);
                break;
            case RPC_CMD_PAD_CTRL_LOGIC_TIGHTNESS: // arg3: tight?
                teensy_pad_logic_ctrl_tightness(data[1], data[3], data[2]);
                break;
            default:
                break;
            }
            break;
        case RPC_CMD_GLITCH_SET_CHAIN_MAX: // arg0: new max chain element count, arg1: use """"heap""""?, arg2: memset0 the varray?
            if (data[1]) {
                g_glitch_varray = (glitch_varray_s *)&cfg_prog_bss_end;
                g_glitch_max_chain_n = data[0] ?: GLITCH_STATIC_CHAIN_N;
            } else {
                g_glitch_varray = g_static_glitch_varray;
                g_glitch_max_chain_n = (data[0] && (data[0] <= GLITCH_STATIC_CHAIN_N)) ? data[0] : GLITCH_STATIC_CHAIN_N;
            }
            if (data[2])
                memset32(g_glitch_varray, 0, (g_glitch_max_chain_n * sizeof(glitch_varray_s)));
            cret = (uint32_t)g_glitch_varray;
            break;
        case RPC_CMD_GET_SP:
            cret = 0;
            asm volatile ("mov %0, sp\n\t" : "=r" (cret));
            break;
        case RPC_CMD_GLITCH_LOOP:
            if (data[0])
                printf(RPC_WATERMARK "LOOPIN\n", cret);
            cret = glitch_loop_chain(g_glitch_varray);
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