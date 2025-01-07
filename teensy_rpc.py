# bare-bones rpc client implementation for teensy4vfi

import sys, struct
import serial
from enum import Enum

DEFAULT_PORT = 'COM7'                   # teensy
DEFAULT_BAUD = 115200                   # default teensy debug uart baud
RPC_TIMEOOUT = 1                        # rpc command timeout
MAX_WAIT_RPC = 0                        # max wait timeout count for rpc commands, set to 0 for infinite

DEFAULT_DRIVER_PAD = 22                 # aka mosfet pad for VFI
DEFAULT_LL_TRIGGER_PAD = 23             # logic-level trigger pad
DEFAULT_UART_TRIGGER_N = 1              # uart trigger teensy uartn
DEFAULT_UART_TRIGGER_EXP_BYTE = b'\xD9' # uart trigger byte
DEFAULT_ARG_DICT = {                    # our glitch_add vars with default preset, translated into glitch_prep_* args
    "offset" : [1, "how many ~cycles from trigger to glitch"],
    "offset_mult" : [1, "how many times to repeat the offset loop"],
    "width" : [1, "how many ~cycles the glitch is held"],
    "trigger" : [23, "trigger pad or uartn"],
    "trigger_state" : [1, "trigger logic state or uart byte"],
    "driver" : [22, "(mosfet) glitch driver pad"],
    "queue" : [0, "should this glitch be added to the chain?"],
    "no_trigger" : [0, "skip the glitch trigger"],
    "no_driver" : [0, "skip the glitch driver"],

    "uart_mode" : [0, "set this glitch to use an uart trigger"],

    "override" : [0, "set this to enable default config overrides (O)"],
    "clockspeed" : [0, "(O) glitch clockspeed / core frequency"],
    "driver_mask" : [0, "(O) glitch driver start/stop pattern"],
    "driver_set_drive" : [0, "(O) glitch driver start register addr"],
    "driver_set_stop" : [0, "(O) glitch driver stop register addr"],
    "trigger_mask" : [0, "(O) trigger data pattern mask"], # MUST SET trigger_exp TOO
    "trigger_exp" : [0, "(O) trigger expected data pattern"],
    "trigger_get" : [0, "(O) trigger data pattern register addr"],

    "driver_reconfigure" : [0, "(O) set this to reconfigure the driver pad (D)"],
    "driver_ode" : [0, "(O) (D) enable open-drain mode"],
    "driver_dse" : [7, "(O) (D) driver impedance divider, from 400ohms, max div 7"],

    "trigger_reconfigure" : [0, "(O) set this to reconfigure the trigger pad (T)"],
    "trigger_pke" : [0, "(O) (T) enable pull/keep"],
    "trigger_pue" : [0, "(O) (T) set p/k mode (0|1 - keep|pull)"], # requires pke
    "trigger_pus" : [0, "(O) (T) set pull type (0|1|2|3 - 100k down|47k up|100k up|22k up)"], # requires pue
    "trigger_hys" : [0, "(O) (T) enable hysteresis"],
}

# bitfield enums for glitch_prep args
class PULL_TYPE(Enum):
    PULL_DOWN_100K = 0
    PULL_UP_47K = 1
    PULL_UP_100K = 2
    PULL_UP_22K = 3

class CFG_TYPES(Enum):
    LOGIC_LEVEL = 0
    UART = 1
    FLAG_CHAIN = 4
    FLAG_NODRIVER = 5
    FLAG_NOTRIGGER = 6

class CFG_XPAD_CTL(Enum):
    TEENSY_PAD = 0
    IGNORE = 6
    RECONFIGURE = 7

class CFG_IPAD_CTL(Enum):
    TEENSY_UARTN = 0
    UART_MODE = 8
    PKE = 9
    PUE = 10
    PUS = 11
    HYS = 13
    TRIG_STATE = 16
    TRIG_UART_WM = 16

class CFG_OPAD_CTL(Enum):
    ODE = 8
    DSE = 9

class CFG_GLITCH_DFL(Enum):
    QUEUE = 0
    NODRIVER = 1
    NOTRIGGER = 2

RPC_MAGIC = b'@'                        # byte indicating we are talking to teensy
RPC_WATERMARK = b'!PC_'                 # prefix indicating teensy is replying to us
RPC_SENDMORE = b'G1\r\n'                # teensy wants more data
RPC_READYNEXT = b'G0\r\n'               # teensy is ready for the next command
RPC_COMMANDS = {                        # supported RPC commands & short descriptions
    "ping" : [0x0, "get rpc server firmware build timestamp", ""],
    "read32" : [0x1, "read 32bits from a memory address", "[address]"],
    "write32" : [0x2, "write 32bits to a memory address", "[address] [data]"],
    "memset" : [0x3, "fill a memory range (8bit)", "[start] [fill] [size]"],
    "memcpy" : [0x4, "copy a memory range to another address", "[dst] [src] [size]"],
    "delay" : [0x5, "set rpc server reply delay", "[delay]"],
    "stop" : [0x6, "stop the rpc server", ""],
    "hexdump" : [0x7, "print a memory range (hex)", "[address] [size] <print line addr?>"],
    "memset32" : [0x8, "fill a memory range (32bit)", "[start] [fill] [size]"],
    "glitch_prep_ll" : [0x9, "prepare a default logic level -triggered glitch", "<offset> <offset_mult> <width> <add to chain?> <trigger pad> <trigger state> <driver pad>"],
    "glitch_arm" : [0xa, "execute the glitch chain", "<fake rpc reply to free the client?>"],
    "glitch_prep_custom" : [0xb, "prepare a custom glitch", "[glitch_config]"],
    "glitch_prep_uart" : [0xc, "prepare a default uart -triggered glitch", "<offset> <offset_mult> <width> <add to chain?> <trigger pad> <trigger char> <driver pad>"],
    "set_clk" : [0xd, "set core freq", "[freq] <clkf>"],
    "glitch_prep_custom_chain" : [0xe, "prepare a custom glitch (add to chain)", "[glitch_config]"],
    "glitch_prep_none" : [0xf, "prepare a default glitch (no trigger)", "<offset> <offset_mult> <width> <add to chain?> <driver pad>"],
    "custom" : [0x10, "run the custom_main func", "[arg0] [arg1] [arg2] <data>"],
    "uart_init" : [0x11, "init a teensy uartn", "[uartn] [baud] [flags] <wait?>"], #TODO: wrap
    "pad_configure" : [0x12, "set pad_ctl and mux_ctl for a teensy pad", "[pad] [pad_ctl] [mux_ctl]"], #TODO: wrap
    "pad_ctrl_logic" : [0x13, "control pad state (logic level mode)", "[func] [pad] <wait?>"],
    "glitch_set_chain_max" : [0x14, "set glitch varray/chain location and max elements", "<max glitches> <in heap?>"],
    "get_sp" : [0x15, "get the current stack pointer", ""],
    "glitch_arm_loop" : [0x16, "execute the glitch chain in an infinite loop", ""]
}

LOGIC_COMMANDS = { # cmd : [func, desc, opt_args]
    "clear" : [0, "set pad low", "None"],
    "set" : [1, "set pad high", "None"],
    "toggle" : [2, "toggle pad state", "None"],
    "mode" : [3, "set pad mode", "[in|out]"],
    "read" : [4, "read pad state", "None"],
    "tight" : [5, "use tight-coupled gpio controller for pad", "[0|1]"],
}

uart = serial.Serial(DEFAULT_PORT, baudrate=DEFAULT_BAUD, timeout=RPC_TIMEOOUT)

def send_rpc_cmd(id, argv, max_wait=MAX_WAIT_RPC):
    data = bytearray()
    for arg in argv:
        data.extend(struct.pack('<I', arg))
    headr = bytearray([int.from_bytes(RPC_MAGIC, "little"), RPC_COMMANDS[id][0], len(data), (RPC_COMMANDS[id][0] + len(data))])
    #print(headr.hex().upper())
    #print(data.hex().upper())
    uart.reset_output_buffer()
    uart.reset_input_buffer()
    uart.write(headr)
    cont = uart.readline()
    loopc = 0
    while (not RPC_WATERMARK in cont) and (max_wait == 0 or loopc < max_wait):
        cont = uart.readline()
        loopc += 1
    if not RPC_WATERMARK in cont:
        print("E: timeout1")
        return -1
    if cont == RPC_WATERMARK + RPC_SENDMORE:
        uart.write(data)
        cont = uart.readline()
        loopc = 0
        while (not RPC_WATERMARK in cont) and (max_wait == 0 or loopc < max_wait):
            cont = uart.readline()
            loopc += 1
        if not RPC_WATERMARK in cont:
            print("E: timeout2")
            return -2
    rdy = cont
    loopc = 0
    while (not rdy == RPC_WATERMARK + RPC_READYNEXT) and (max_wait == 0 or loopc < max_wait):
        rdy = uart.readline()
        loopc += 1
    if not rdy == RPC_WATERMARK + RPC_READYNEXT:
        print("W: did not get a rdy reply from teensy, might hang")
    cont = cont.decode('utf-8').strip("!PC_")
    print(cont.strip() + " <done")
    try:
        return int(cont[2:], 16)
    except:
        return 0xDEFA

def glitch_add_dfl(argd, max_wait=MAX_WAIT_RPC):
    #print(argd)
    cmd = "glitch_prep_ll"
    if argd["uart_mode"] == 1:
        cmd = "glitch_prep_uart"
    flags = argd["queue"] | (argd["no_driver"] << CFG_GLITCH_DFL.NODRIVER.value) | (argd["no_trigger"] << CFG_GLITCH_DFL.NOTRIGGER.value)
    argv = [argd["offset"], argd["offset_mult"], argd["width"], flags, argd["trigger"], argd["trigger_state"], argd["driver"]]
    #print(argv)
    return send_rpc_cmd(cmd, argv, max_wait)

def glitch_add_custom(argd, max_wait=MAX_WAIT_RPC):
    #print(argd)
    cmd = "glitch_prep_custom"
    if argd["queue"] == 1:
        cmd = "glitch_prep_custom_chain"
    #TODO: maybe handle it in a nice dictionary/array where idx = bit shift?
    trigger_ctl = argd["trigger"] | (argd["no_trigger"] << CFG_XPAD_CTL.IGNORE.value) | (argd["uart_mode"] << CFG_IPAD_CTL.UART_MODE.value) | (argd["trigger_state"] << CFG_IPAD_CTL.TRIG_STATE.value)
    if argd["trigger_reconfigure"] == 1:
        trigger_ctl = trigger_ctl | (1 << CFG_XPAD_CTL.RECONFIGURE.value) | (argd["trigger_pke"] << CFG_IPAD_CTL.PKE.value) | (argd["trigger_pue"] << CFG_IPAD_CTL.PUE.value) | (argd["trigger_pus"] << CFG_IPAD_CTL.PUS.value) | (argd["trigger_hys"] << CFG_IPAD_CTL.HYS.value)
    driver_ctl = argd["driver"] | (argd["no_driver"] << CFG_XPAD_CTL.IGNORE.value)
    if argd["driver_reconfigure"] == 1:
        driver_ctl = driver_ctl | (1 << CFG_XPAD_CTL.RECONFIGURE.value) | (argd["driver_ode"] << CFG_OPAD_CTL.ODE) | (argd["driver_dse"] << CFG_OPAD_CTL.DSE)
    argv = [argd["width"], argd["offset"], argd["offset_mult"], trigger_ctl, driver_ctl, argd["clockspeed"], argd["driver_mask"], argd["driver_set_drive"], argd["driver_set_stop"], argd["trigger_mask"], argd["trigger_exp"], argd["trigger_get"]]
    #print(argv)
    return send_rpc_cmd(cmd, argv, max_wait)

# "ui design is my passion" xD
def helper(focus):
    match focus:
        case "none":
            print(f"{'CMD':>24}" + " : " + "DESCRIPTION")
            print(f"{'---':>24}" + " : " + "-----------")
            for cmd in RPC_COMMANDS:
                print(f"{cmd:>24}" + " : " + RPC_COMMANDS[cmd][1])
            print(f"{' ':>24}" + " ! " + " ")
            print(f"{'glitch_add':>24}" + " : " + "wrapper for glitch_prep_* funcs")
            print(f"{'manual':>24}" + " : " + "manual trigger for default glitch_prep_uart")
            print(f"{'logic':>24}" + " : " + "control pad logic state")
            print(f"{'help <CMD>':>24}" + " : " + "show me or CMD's expected args")
        case "glitch_add":
            print("\nUsage: " + focus + " param=value par6=val6 par3=val3 ...")
            print("Descr: " + "prepare a glitch with specified params" + "\n")
            print(f"{'PARAM':>20}" + " : " + f"{'DEFAULT':^7}" + " : " + "DESCRIPTION")
            print(f"{'-----':>20}" + " : " + f"{'-------':^7}" + " : " + "-----------")
            for arg in DEFAULT_ARG_DICT:
                print(f"{arg:>20}" + " : " + f"{str(DEFAULT_ARG_DICT[arg][0]):^7}" + " : " + DEFAULT_ARG_DICT[arg][1])
        case "logic":
            print("\nUsage: " + focus + " [SUBCMD] [ARG] [PAD] <pad2> ...")
            print("Descr: " + "control pad logic state" + "\n")
            print(f"{'SUBCMD':>8}" + " : " + f"{'ARG':^8}" + " : " + "DESCRIPTION")
            print(f"{'------':>8}" + " : " + f"{'---':^8}" + " : " + "-----------")
            for cmd in LOGIC_COMMANDS:
                print(f"{cmd:>8}" + " : " + f"{LOGIC_COMMANDS[cmd][2]:^8}" + " : " + LOGIC_COMMANDS[cmd][1])
        case _:
            if focus in RPC_COMMANDS:
                print("\nUsage: " + focus + " " + RPC_COMMANDS[focus][2])
                print("Descr: " + RPC_COMMANDS[focus][1] + "\n")
            else:
                print("command not found and/or malformed input")
    return ""


def handle_cmd(cmd, argv):
    match cmd:
        case "manual":
            return uart.write(DEFAULT_UART_TRIGGER_EXP_BYTE)
        case "glitch_add":
            arg_dict = {param: value[0] for param, value in DEFAULT_ARG_DICT.copy().items()}
            for arg in argv:
                key, val = arg.split('=')
                if val.startswith('0x'):
                    arg_dict[key] = int(val, 16)
                else:
                    arg_dict[key] = int(val)
            if arg_dict["override"] == 1:
                return glitch_add_custom(arg_dict)
            else:
                return glitch_add_dfl(arg_dict)
        case "logic":
            if not argv[0] in LOGIC_COMMANDS or len(argv) < 2:
                return helper("logic")
            op_argv = [LOGIC_COMMANDS[argv[0]][0], 0, 0]
            if op_argv[0] == 3:
                op_argv.append(1 if argv[1] == "1" or argv[1].startswith("o") else 0)
            elif op_argv[0] == 5:
                op_argv.append(1 if argv[1] == "1" or argv[1].startswith("t") else 0)
            pads_start = 2 if (op_argv[0] == 3 or op_argv[0] == 5) else 1
            if len(argv) <= pads_start:
                return helper("logic")
            op_ret = 0xFFFFDEFA
            for pad in argv[pads_start:]:
                op_argv[1] = int(pad)
                op_ret = send_rpc_cmd("pad_ctrl_logic", op_argv)
            return op_ret
        case "help":
            if len(argv) > 0:
                return helper(argv[0])
            return helper("none")
        case _:
            if cmd in RPC_COMMANDS:
                rargv = []
                for arg in argv:
                    if arg.startswith('0x'):
                        rargv.append(int(arg, 16))
                    else:
                        rargv.append(int(arg))
                return send_rpc_cmd(cmd, rargv)
            else:
                print("command not found and/or malformed input")
    return ""

if __name__ == "__main__":
    if len(sys.argv) > 1:
        handle_cmd(sys.argv[1], sys.argv[2:])
    else:
        helper("none")
    uart.close()