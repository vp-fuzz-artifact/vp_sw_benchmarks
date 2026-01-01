MEMORY {
    /* Map everything to 0x0 to match 'system.h' RAM_LO */
    ram (rwx) : ORIGIN = 0x00000000, LENGTH = 256M
}

SECTIONS {
    .text : {
        KEEP(*(.vector_table))
        KEEP(*(.init))
        *(.text .text.*)
        *(.rodata .rodata.*)
    } > ram

    .data : {
        . = ALIGN(4);
        *(.data .data.*)
    } > ram

    .bss : {
        . = ALIGN(4);
        *(.bss .bss.*)
        *(COMMON)
    } > ram

    /* Stack goes to a safe place in RAM (e.g. 1MB mark) */
    .stack (NOLOAD) : {
        . = ALIGN(8);
        . = . + 0x4000;
        PROVIDE(__stack_end = .);
    } > ram
}