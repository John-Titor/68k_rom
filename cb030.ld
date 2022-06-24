/*
 * Linker script for ROM on CB030 SBC
 */

OUTPUT_ARCH(m68k)
OUTPUT(elf32-m68k)
ENTRY(_reset_vector)

/*
 * 16/64/128M of RAM at 0, ROM at 0xfe000000. 
 * We run from ROM, and use the last 32K of the low 16M of RAM
 * for BSS + stack.
 *
 * ROM is mirrored at 0 at startup, we'll turn off the mirror
 * before touching RAM.
 */
MEMORY
{
    vectors(rw)   : ORIGIN = 0xfe000000, LENGTH = 1K
    rom(rx)       : ORIGIN = 0xfe000400, LENGTH = 511K
    ram(rwx)      : ORIGIN = 16M - 32K, LENGTH = 32K
}

PROVIDE(_reset_vector                   = ORIGIN(rom));

SECTIONS
{
    .vectors :
    {
        /* lower m68k vectors */
        _vector_base = .;
        LONG(_stack_top)
        LONG(_reset_vector)
        LONG(vector_bus_error)
        LONG(vector_address_error)
        LONG(vector_illegal_instruction)
        LONG(vector_zero_divide)
        LONG(vector_chk)
        LONG(vector_trapv)
        LONG(vector_privilege_violation)
        LONG(vector_trace)
        LONG(vector_line_a)
        LONG(vector_line_f)
        . = 0x3c;
        LONG(vector_uninitialized_interrupt)
        . = 0x60;
        LONG(vector_spurious_interrupt)

        /* user vectors */
        . = 0x100;
        LONG(vector_64)
        LONG(vector_65)
        LONG(vector_66)
        LONG(vector_67)
        _vector_top = .;

    } > vectors

    .text ORIGIN(rom) :
    {
        _btext = .;
        KEEP(*(.entrypoint))
        *(.text .text.*);
        KEEP(*(INTERRUPT_HANDLERS));
        *(.rodata .rodata.*);
        . = ALIGN(4);
        _commands = .;
        KEEP(*(COMMANDS))
        _ecommands = .;
        _etext = .;
        *(.lit);
    } > rom
    
    . = ORIGIN(ram);
    .data ALIGN(4) :
    {
        _reloc_destination = .;
        *(.got.plt) *(.got);
        *(.shdata);
        *(.data .data.*);
        . = ALIGN(4);
        _edata = .;
        _reloc_end = .;
    } > ram AT> rom
    _reloc_source = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        *(.shbss);
        *(.bss .bss.*);
        *(COMMON);
        _stack_base = .;
        . = ORIGIN(ram) + LENGTH(ram);
        _stack_top = .;
        _ebss = .;
    } > ram

    .stab 0 (NOLOAD) :
    {
        *(.stab);
    }

    .stabstr 0 (NOLOAD) :
    {
        *(.stabstr);
    }
}

ASSERT((_stack_top - _stack_base) > 2048, "less than 2K stack available")
