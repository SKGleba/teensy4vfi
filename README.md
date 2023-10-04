# teensy4vfi
Teensy 4.0 firmware for Voltage Fault Injection research

## note
This project is in its very early stages, it also serves as a way for me to learn """white box""" SoC baremetal programming.<br>
Some code might make no sense and/or be overcomplicated, suggestions and non-C++ PRs are welcome.

## Known TODOs
 - Critical bug fixes
 - Docs, readme, measurements, etc
 - More glitch types

### Optional
 - Exceptions & Interrupts
 - Interrupt-based trigger (optional due to high jitter)
 - Interrupt->DMA-based UART RX handling
 - More RPC commands

### Known BUGs
 - logic level trigger is too sensitive
   - triggers if you fart in its general direction
   - works fine with only wire being trigger mosfet, adding a LA to the line triggers the trigger
 - uart artifacts during glitch_prep_uart (2+) after ccm_set_core_clkf causing RPC script error
   - we reset the UART controller here, maybe add some TC delay?
 - PC RPC script skipping, cutting return codes
   - timing issues, very low priority 

## compiling & flashing
This project does not have any external requirements, it can be compiled with the [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain).<br>
The output *.hex* can be used with [teensy-loader](https://www.pjrc.com/teensy/loader_win10.html) to flash the flash (lul)
