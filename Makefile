PREFIX=mipsel-none-elf-
AS="${PREFIX}as"
CC="${PREFIX}gcc"
LD="${PREFIX}ld"
OBJCOPY="${PREFIX}objcopy"

firmware.bin: firmware.srec
	${OBJCOPY} -I srec -O binary firmware.srec firmware.bin

firmware.srec: firmware.elf Makefile
	${OBJCOPY} firmware.elf -O srec firmware.srec

firmware.elf: init.o main.o linker.ld p32mx250f128b.o
	${LD} -nostdlib init.o main.o p32mx250f128b.o -o firmware.elf -T linker.ld

p32mx250f128b.o: p32mx250f128b.s
	${AS} -mips32r2 -EL p32mx250f128b.s -o p32mx250f128b.o

init.o: init.s
	${AS} -mips32r2 -EL init.s -o init.o

main.o: main.c
	${CC} -Os -msoft-float -EL -march=m4k -nostdlib -c main.c

.PHONY: clean
clean:
	rm -f firmware.srec firmware.elf init.o main.o p32mx250f128b.o


.PHONY: upload
upload: firmware.bin
	./transfer firmware.bin
