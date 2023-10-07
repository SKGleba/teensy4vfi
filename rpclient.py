import sys, os, struct, code, binascii
import serial, time, re, math

DEFAULT_PORT = 'COM18'
DEFAULT_BAUD = 115200

GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE = b'\xD9'

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
    "set_clk" : 0xd
}

uart = serial.Serial(DEFAULT_PORT, baudrate=DEFAULT_BAUD, timeout=0)

def send_rpc_cmd(id, argv):
    data = bytearray()
    for arg in argv:
        data.extend(struct.pack('<I', arg))
    uart.write(bytearray([int.from_bytes(RPC_MAGIC, "little"), RPC_COMMANDS[id], len(data), (RPC_COMMANDS[id] + len(data))]))
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

if __name__ == "__main__":
    if sys.argv[1] == "manual":
        uart.write(GLITCH_DEFAULT_UART_TRIGGER_EXP_BYTE)
    elif sys.argv[1] in RPC_COMMANDS:
        argv = []
        for arg in sys.argv[2:]:
            if arg.startswith('0x'):
                argv.append(int(arg, 16))
            else:
                argv.append(int(arg))
        send_rpc_cmd(sys.argv[1], argv)
    uart.close()