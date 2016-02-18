PREFIX=mipsel-none-elf-
AS:="${PREFIX}as"
CC:="${PREFIX}gcc"
LD:="${PREFIX}ld"
OBJCOPY:="${PREFIX}objcopy"

CFLAGS := -D__XC32__ -D__PIC32MX__ -Os -msoft-float -EL -march=m4k -nostdlib -I. -Iusb/include

MAIN_OBJECTS := main.o bluetooth.o keyboard.o util.o
USB_OBJECTS := usb_descriptors.o usb/src/usb.o usb/src/usb_cdc.o usb/src/usb_hid.o

firmware.bin: firmware.srec
	${OBJCOPY} -I srec -O binary firmware.srec firmware.bin

firmware.srec: firmware.elf
	${OBJCOPY} firmware.elf -O srec firmware.srec

firmware.elf: init.o linker.ld p32mx250f128b.o ${MAIN_OBJECTS} ${USB_OBJECTS} Makefile
	${LD} -nostdlib init.o p32mx250f128b.o ${MAIN_OBJECTS} ${USB_OBJECTS} -o firmware.elf -T linker.ld --unresolved-symbols=report-all

p32mx250f128b.o: p32mx250f128b.s
	${AS} -mips32r2 -EL p32mx250f128b.s -o p32mx250f128b.o

init.o: init.s
	${AS} -mips32r2 -EL init.s -o init.o

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean
clean:
	rm -f firmware.srec firmware.elf init.o main.o p32mx250f128b.o ${USB_OBJECTS}

.PHONY: upload
upload: firmware.bin
	./transfer firmware.bin
