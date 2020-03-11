/*
 * Linker script for ROM on Tiny68k SBC
 */

OUTPUT_ARCH(m68k)
OUTPUT(elf32-m68k)
ENTRY(_reset_vector)

/*
 * 32K of ROM is copied to zero by hardware at startup. 
 * We leave vectors at zero, but relocate the rest of the
 * ROM to the end of RAM so that it's possible to upload a
 * new program at zero.
 *
 * Peripherals start at 16M - 32K, so we take 64K below that.
 */
MEMORY
{
    vectors(rw) : ORIGIN = 0, LENGTH = 1K
    loadspace(rw) : ORIGIN = 1K, LENGTH = 31K
    runspace(rwx) : ORIGIN = 16M - 96K, LENGTH = 64K
}

PROVIDE(_reset_vector                   = ORIGIN(loadspace));

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

    .text ORIGIN(runspace) :
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
    } > runspace AT> loadspace
    
    .data :
    {
        . = ALIGN(0x4);
        *(.got.plt) *(.got);
        *(.shdata);
        *(.data .data.*);
        . = ALIGN(0x4);
        _edata = .;
    } > runspace AT> loadspace

    .bss :
    {
        . = ALIGN(0x4);
        *(.shbss);
        *(.bss .bss.*);
        *(COMMON);
        _vector_savearea = .;
        . += _vector_top - _vector_base;
        _stack_base = .;
        . = ORIGIN(runspace) + LENGTH(runspace);
        _stack_top = .;
        _ebss = .;
    } > runspace

    .stab 0 (NOLOAD) :
    {
        *(.stab);
    }

    .stabstr 0 (NOLOAD) :
    {
        *(.stabstr);
    }
}

/* parameters for the relocator */
_reloc_source = ORIGIN(loadspace);
_reloc_destination = ORIGIN(runspace);
_reloc_end = _edata;

ASSERT((_stack_top - _stack_base) > 2048, "less than 2K stack available")