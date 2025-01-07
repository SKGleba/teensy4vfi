SRCDIR 	= source
SRCS	= $(wildcard source/*.c)
OBJS	=  $(SRCS:.c=.o) source/boot_hdr.ao source/boot.ao source/vector.ao source/glitch.ao
OBJS41	=  $(SRCS:.c=.o41) source/boot_hdr.ao41 source/boot.ao41 source/vector.ao41 source/glitch.ao41

PREFIX	= arm-none-eabi
CC		= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
CFLAGS 	= -mcpu=cortex-m7 -mthumb -Os -Wall -fno-builtin
LDFLAGS = -T linker.x -nostartfiles -nostdlib
ASFLAGS	=

.PHONY: teensy4 teensy41 all clean

all: output/firmware40.hex output/firmware41.hex
teensy4: output/firmware40.hex
teensy41: output/firmware41.hex

output/firmware40.hex: fw40.hex
	-rm source/*.o source/*.ao
	-rm output/firmware40.hex output/firmware40.elf
	-mkdir output
	mv fw40.elf output/firmware40.elf
	mv fw40.hex output/firmware40.hex

fw40.hex: fw40.elf
	$(OBJCOPY) -O ihex $^ $@

fw40.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -Xlinker --defsym=TARGET_TEENSY41=0

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.ao: %.s
	$(CC) $(ASFLAGS) -c $< -o $@

output/firmware41.hex: fw41.hex output/firmware40.hex
	-rm source/*.o41 source/*.ao41
	-rm output/firmware41.hex output/firmware41.elf
	-mkdir output
	mv fw41.elf output/firmware41.elf
	mv fw41.hex output/firmware41.hex

fw41.hex: fw41.elf
	$(OBJCOPY) -O ihex $^ $@

fw41.elf: $(OBJS41)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -Xlinker --defsym=TARGET_TEENSY41=1

%.o41: %.c
	$(CC) $(CFLAGS) -DTARGET_TEENSY41 -c $< -o $@

%.ao41: %.s
	$(CC) $(ASFLAGS) -c $< -o $@

clean: 
	-rm source/*.o
	-rm source/*.ao
	-rm source/*.o41
	-rm source/*.ao41
	-rm -rf output