/*
 * Linker script for ROM on P90MB SBC
 */

OUTPUT_ARCH(m68k)
OUTPUT(elf32-m68k)
ENTRY(_reset_vector)

/*
 * 512K of ROM at 0, 512K of RAM at 16M. We run from ROM,
 * and use the last 32K of RAM for BSS + stack.
 */
MEMORY
{
    vectors(rw) : ORIGIN = 0, LENGTH = 1K
    rom(rx) : ORIGIN = 1K, LENGTH = 511K
    ram(rwx) : ORIGIN = 16M + 480K, LENGTH = 32K
}

PROVIDE(_reset_vector = ORIGIN(rom));

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
        . = ALIGN(0x4);
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
        . = ALIGN(0x4);
        _edata = .;
        _reloc_end = .;
    } > ram AT> rom
    _reloc_source = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(0x4);
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
