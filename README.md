# teensy4vfi
Teensy 4.0 firmware for Voltage Fault Injection research

## note
This project is in its very early stages, it also serves as a way for me to learn """white box""" SoC baremetal programming.<br>
Some code might make no sense and/or be overcomplicated, suggestions and non-C++ PRs are welcome.

## TODOs
 - RPC
 - Exceptions & GPIO Interrupts
 - Basic PoC for intr-trigger -> wait -> glitch
 - Custom configuration support for the above
 - Docs, readme, measurements, etc

## compiling & flashing
This project does not have any external requirements, it can be compiled with the [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain).<br>
The output *.hex* can be used with [teensy-loader](https://www.pjrc.com/teensy/loader_win10.html) to flash the flash (lul)
