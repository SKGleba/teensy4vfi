# bare-bones rpc client implementation for teensy4vfi

import sys, struct
import serial

DEFAULT_PORT = 'COM18'                  # teensy
DEFAULT_BAUD = 115200                   # default teensy debug uart baud

RPC_MAGIC = b'@'                        # byte indicating we are talking to teensy
RPC_WATERMARK = b'!PC_'                 # prefix indicating teensy is replying to us
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
    "pad_configure" : [0x12, "set pad_ctl and mux_ctl for a teensy pad" "[pad] [pad_ctl] [mux_ctl]"], #TODO: wrap
    "pad_ctrl_logic" : [0x13, "control pad state (logic level mode)", "[func] [pad] <wait?>"], #TODO: wrap
    "glitch_set_chain_max" : [0x14, "set glitch varray/chain location and max elements", "<max glitches> <in heap?>"],
    "get_sp" : [0x15, "get the current stack pointer", ""]
}

DEFAULT_DRIVER_PAD = 22                 # aka mosfet pad for VFI
DEFAULT_LL_TRIGGER_PAD = 23             # logic-level trigger pad
DEFAULT_UART_TRIGGER_N = 1              # uart trigger teensy uartn
DEFAULT_UART_TRIGGER_EXP_BYTE = b'\xD9' # uart trigger byte
DEFAULT_ARG_DICT = {                    # our glitch_add vars with default preset, translated into glitch_prep_* args
    "offset" : [1, "how many ~cycles from trigger to glitch"],
    "offset_mult" : [1, "how many times to repeat the offset loop"],
    "width" : [1, "how many ~cycles the glitch is held"],
    "trigger" : [23, "trigger pad or uartn"],
    "trigger_state" : [0, "trigger logic state or uart byte"],
    "driver" : [22, "(mosfet) glitch driver pad"],
    "queue" : [0, "should this glitch be added to the chain?"],
    "no_trigger" : [0, "skip the glitch trigger"],
    "no_driver" : [0, "skip the glitch driver"],

    "uart_mode" : [False, "this glitch uses an uart trigger"],

    "override" : [False, "set this to enable default config overrides (O)"],
    "clockspeed" : [0, "(O) glitch clockspeed / core frequency"],
    "driver_mask" : [0, "(O) glitch driver start/stop pattern"],
    "driver_set_drive" : [0, "(O) glitch driver start register addr"],
    "driver_set_stop" : [0, "(O) glitch driver stop register addr"],
    "trigger_mask" : [0, "(O) trigger data pattern mask"], # MUST SET trigger_exp TOO
    "trigger_exp" : [0, "(O) trigger expected data pattern"],
    "trigger_get" : [0, "(O) trigger data pattern register addr"],

    "driver_reconfigure" : [False, "(O) set this to reconfigure the driver pad (D)"],
    "driver_ode" : [0, "(O) (D) enable open-drain mode"],
    "driver_dse" : [7, "(O) (D) set drive strength divider"],

    "trigger_reconfigure" : [False, "(O) set this to reconfigure the trigger pad (T)"],
    "trigger_pke" : [0, "(O) (T) enable pull/keep"],
    "trigger_pue" : [0, "(O) (T) set pull mode"],
    "trigger_pus" : [0, "(O) (T) set pull type"],
    "trigger_hys" : [0, "(O) (T) enable hysteresis"],
}

uart = serial.Serial(DEFAULT_PORT, baudrate=DEFAULT_BAUD, timeout=1)

def send_rpc_cmd(id, argv):
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
    while not RPC_WATERMARK in cont:
        cont = uart.readline()
    if cont == RPC_WATERMARK + b'G1\r\n':
        uart.write(data)
        cont = uart.readline()
        while not RPC_WATERMARK in cont:
            cont = uart.readline()
    cont = cont.decode('utf-8').strip("!PC_")
    print(cont)

def glitch_add(argd):
    #print(argd)
    cmd = "glitch_prep_ll"
    if argd["uart_mode"][0] == True:
        cmd = "glitch_prep_uart"
    flags = argd["queue"][0] | (argd["no_driver"][0] << 1) | (argd["no_trigger"][0] << 2)
    argv = [argd["offset"][0], argd["offset_mult"][0], argd["width"][0], flags, argd["trigger"][0], argd["trigger_state"][0], argd["driver"][0]]
    #print(argv)
    send_rpc_cmd(cmd, argv)

def glitch_add_direct(argd):
    #print(argd)
    cmd = "glitch_prep_custom"
    if argd["queue"][0] == 1:
        cmd = "glitch_prep_custom_chain"
    #TODO: maybe handle it in a nice dictionary/array where idx = bit shift?
    trigger_ctl = argd["trigger"][0] | (argd["no_trigger"][0] << 6) | (int(argd["uart_mode"][0] == True) << 8) | (argd["trigger_state"][0] << 16)
    if argd["trigger_reconfigure"][0] == True:
        trigger_ctl = trigger_ctl | (1 << 7) | (argd["trigger_pke"][0] << 9) | (argd["trigger_pue"][0] << 10) | (argd["trigger_pus"][0] << 11) | (argd["trigger_hys"][0] << 13)
    driver_ctl = argd["driver"][0] | (argd["no_driver"][0] << 6)
    if argd["driver_reconfigure"][0] == True:
        driver_ctl = driver_ctl | (1 << 7) | (argd["driver_ode"][0] << 8) | (argd["driver_dse"][0] << 9)
    argv = [argd["width"][0], argd["offset"][0], argd["offset_mult"][0], trigger_ctl, driver_ctl, argd["clockspeed"][0], argd["driver_mask"][0], argd["driver_set_drive"][0], argd["driver_set_stop"][0], argd["trigger_mask"][0], argd["trigger_exp"][0], argd["trigger_get"][0]]
    #print(argv)
    send_rpc_cmd(cmd, argv)

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
            print(f"{'help <CMD>':>24}" + " : " + "show me or CMD's expected args")
        case "glitch_add":
            print("\nUsage: " + focus + " param=value par6=val6 par3=val3 ...\n")
            print("Descr: " + "prepare a glitch with specified params" + "\n")
            print(f"{'PARAM':>20}" + " : " + f"{'DEFAULT':^7}" + " : " + "DESCRIPTION")
            print(f"{'-----':>20}" + " : " + f"{'-------':^7}" + " : " + "-----------")
            for arg in DEFAULT_ARG_DICT:
                print(f"{arg:>20}" + " : " + f"{str(DEFAULT_ARG_DICT[arg][0]):^7}" + " : " + DEFAULT_ARG_DICT[arg][1])
        case _:
            if focus in RPC_COMMANDS:
                print("\nUsage: " + focus + " " + RPC_COMMANDS[focus][2] + "\n")
                print("Descr: " + RPC_COMMANDS[focus][1] + "\n")
            else:
                print("command not found and/or malformed input")
    return ""


def handle_cmd(cmd, argv):
    match cmd:
        case "manual":
            return uart.write(DEFAULT_UART_TRIGGER_EXP_BYTE)
        case "glitch_add":
            arg_dict = DEFAULT_ARG_DICT.copy()
            for arg in argv:
                key, val = arg.split('=')
                if val.startswith('0x'):
                    arg_dict[key][0] = int(val, 16)
                else:
                    arg_dict[key][0] = int(val)
            if arg_dict["override"][0] == True:
                return glitch_add_direct(arg_dict)
            else:
                return glitch_add(arg_dict)
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