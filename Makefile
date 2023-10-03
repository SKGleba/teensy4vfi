SRCDIR 	= source
SRCS	= $(wildcard source/*.c)
OBJS	=  $(SRCS:.c=.o) source/boot_hdr.ao source/boot.ao source/vector.ao source/glitch.ao

PREFIX	= arm-none-eabi
CC		= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
CFLAGS 	= -mcpu=cortex-m7 -mthumb -Os -Wall -fno-builtin
LDFLAGS = -T linker.x -nostartfiles -nostdlib
ASFLAGS	=

all: output/test.hex

output/test.hex: test.hex
	-rm source/*.o
	-rm source/*.ao
	-rm -rf output
	mkdir output
	mv test.elf output/test.elf
	mv test.hex output/test.hex

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.ao: %.s
	$(CC) $(ASFLAGS) -c $< -o $@

clean: 
	-rm source/*.o
	-rm source/*.ao
	-rm -rf output