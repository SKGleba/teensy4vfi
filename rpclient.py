# bare-bones rpc client implementation for teensy4vfi

import sys, struct
import serial

DEFAULT_PORT = 'COM18'
DEFAULT_BAUD = 115200

RPC_MAGIC = b'@'
RPC_WATERMARK = b'!PC_'
RPC_COMMANDS = {
    "ping" : 0x0,
    "read32" : 0x1,
    "write32" : 0x2,
    "memset" : 0x3,
    "memcpy" : 0x4,
    "delay" : 0x5, # arg0 : check delay
    "stop" : 0x6,
    "hexdump" : 0x7,
    "memset32" : 0x8,
    "glitch_prep_ll" : 0x9,
    "glitch_arm" : 0xa,
    "glitch_prep_custom" : 0xb,
    "glitch_prep_uart" : 0xc,
    "set_clk" : 0xd,
    "glitch_prep_custom_chain" : 0xe,
    "glitch_prep_none" : 0xf,
    "custom" : 0x10
}

DEFAULT_DRIVER_PAD = 22
DEFAULT_LL_TRIGGER_PAD = 23
DEFAULT_UART_TRIGGER_N = 1
DEFAULT_UART_TRIGGER_EXP_BYTE = b'\xD9'
DEFAULT_ARG_DICT = {
    "offset" : 1,
    "offset_mult" : 1,
    "width" : 1,
    "trigger" : 23,
    "trigger_state" : 0,
    "driver" : 22,
    "queue" : 0,
    "no_trigger" : 0,
    "no_driver" : 0,

    "uart_mode" : False,

    "override" : False,
    "clockspeed" : 0,
    "driver_mask" : 0,
    "driver_set_drive" : 0,
    "driver_set_stop" : 0,
    "trigger_mask" : 0, # MUST SET trigger_exp TOO
    "trigger_exp" : 0,
    "trigger_get" : 0,

    "driver_reconfigure" : False,
    "driver_ode" : 0,
    "driver_dse" : 7,

    "trigger_reconfigure" : False,
    "trigger_pke" : 0,
    "trigger_pue" : 0,
    "trigger_pus" : 0,
    "trigger_hys" : 0,
}

uart = serial.Serial(DEFAULT_PORT, baudrate=DEFAULT_BAUD, timeout=1)

def send_rpc_cmd(id, argv):
    data = bytearray()
    for arg in argv:
        data.extend(struct.pack('<I', arg))
    headr = bytearray([int.from_bytes(RPC_MAGIC, "little"), RPC_COMMANDS[id], len(data), (RPC_COMMANDS[id] + len(data))])
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
    if argd["uart_mode"] == True:
        cmd = "glitch_prep_uart"
    flags = argd["queue"] | (argd["no_driver"] << 1) | (argd["no_trigger"] << 2)
    argv = [argd["offset"], argd["offset_mult"], argd["width"], flags, argd["trigger"], argd["trigger_state"], argd["driver"]]
    #print(argv)
    send_rpc_cmd(cmd, argv)

def glitch_add_direct(argd):
    #print(argd)
    cmd = "glitch_prep_custom"
    if argd["queue"] == 1:
        cmd = "glitch_prep_custom_chain"
    #TODO: maybe handle it in a nice dictionary/array where idx = bit shift?
    trigger_ctl = argd["trigger"] | (argd["no_trigger"] << 6) | (int(argd["uart_mode"] == True) << 8) | (argd["trigger_state"] << 16)
    if argd["trigger_reconfigure"] == True:
        trigger_ctl = trigger_ctl | (1 << 7) | (argd["trigger_pke"] << 9) | (argd["trigger_pue"] << 10) | (argd["trigger_pus"] << 11) | (argd["trigger_hys"] << 13)
    driver_ctl = argd["driver"] | (argd["no_driver"] << 6)
    if argd["driver_reconfigure"] == True:
        driver_ctl = driver_ctl | (1 << 7) | (argd["driver_ode"] << 8) | (argd["driver_dse"] << 9)
    argv = [argd["width"], argd["offset"], argd["offset_mult"], trigger_ctl, driver_ctl, argd["clockspeed"], argd["driver_mask"], argd["driver_set_drive"], argd["driver_set_stop"], argd["trigger_mask"], argd["trigger_exp"], argd["trigger_get"]]
    #print(argv)
    send_rpc_cmd(cmd, argv)

def handle_cmd(cmd, argv):
    match cmd:
        case "manual":
            return uart.write(DEFAULT_UART_TRIGGER_EXP_BYTE)
        case "glitch_add":
            arg_dict = DEFAULT_ARG_DICT.copy()
            for arg in argv:
                key, val = arg.split('=')
                if val.startswith('0x'):
                    arg_dict[key] = int(val, 16)
                else:
                    arg_dict[key] = int(val)
            if arg_dict["override"] == True:
                return glitch_add_direct(arg_dict)
            else:
                return glitch_add(arg_dict)
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
    handle_cmd(sys.argv[1], sys.argv[2:])
    uart.close()