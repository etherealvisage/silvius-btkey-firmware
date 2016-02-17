OUTPUT_FORMAT(elf32-little)
OUTPUT_ARCH(mips)

ENTRY(_start)

/* Change these to suit. */
RAM_SIZE = 32 * 1024;
FLASH_SIZE = 128 * 1024;

RESET_ADDR = 0xbd000000;

MEMORY {
    kseg0_program           : ORIGIN = 0x9d000000, LENGTH = FLASH_SIZE
    kseg0_init_mem          : ORIGIN = 0x9fc00000, LENGTH = 0x100
    kseg0_boot_mem          : ORIGIN = 0x9fc00100, LENGTH = 0xaf0
    kseg0_mem               : ORIGIN = 0x80000000, LENGTH = RAM_SIZE

    kseg1_program           : ORIGIN = 0xbd000000, LENGTH = FLASH_SIZE
    kseg1_init_mem          : ORIGIN = 0xbfc00000, LENGTH = 0x100
    kseg1_boot_mem          : ORIGIN = 0xbfc00000, LENGTH = 0xaf0
    kseg1_mem               : ORIGIN = 0xa0000000, LENGTH = RAM_SIZE

    sfrs                    : ORIGIN = 0xbf800000, LENGTH = 0x100000
}

PHDRS {
    code_header PT_LOAD;
    data_header PT_LOAD;
    bss_header PT_LOAD;
}

SECTIONS {
    /* Main code */
    .code RESET_ADDR : {
        *(.reset)
        *(.text)
        *(.rodata)
        *(.rodata.*)
    } > kseg1_program :code_header

    .data_begin : ALIGN(4) {
        data_flash_begin = .;
    } > kseg1_program :data_header

    /* Initialized non-constant globals */
    .data : ALIGN(4) {
        data_ram_begin = .;
        *(.data)
        *(.sdata)
        data_ram_end = .;
    } > kseg1_mem AT >kseg1_program :data_header

    .data_end : ALIGN(4) {
        data_flash_end = .;
    } > kseg1_program :data_header

    . = ALIGN(4);

    .bss : ALIGN(4) {
        bss_ram_begin = .;
        *(.bss)
        *(.sbss)
        . = ALIGN(4);
        bss_ram_end = .;
    } > kseg1_mem :bss_header

    _gp = 0xa0008000;
    _sp = 0xa0008000; /* TODO: change if RAM size is different */

    /DISCARD/ : {
        *(.reginfo)
        *(.gnu.attributes)
        *(.comment)
        *(.pdr)
        *(.MIPS.abiflags)
    }
}
