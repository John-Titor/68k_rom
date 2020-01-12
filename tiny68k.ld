/*
 * Linker script for ROM on Tiny68k SBC
 */

OUTPUT_ARCH(m68k)
ENTRY(__reset)

MEMORY
{
    vectors(rw) : ORIGIN = 0, LENGTH = 1K
    ram(rwx) : ORIGIN = 1K, LENGTH = 16M - 33K
}

STACKSIZE = 1k;

SECTIONS
{
    .vectors :
    {
        /* lower m68k vectors */
        LONG(__stacktop)
        LONG(__reset)
        LONG(__bus_error)
        LONG(__address_error)
        LONG(__illegal_instruction)
        LONG(__zero_divide)
        LONG(__chk)
        LONG(__trapv)
        LONG(__privilege_violation)
        LONG(__trace)
        LONG(__line_a)
        LONG(__line_f)
        . = 0x3c;
        LONG(__uninitialized_interrupt)
        . = 0x60;
        LONG(__spurious_interrupt)

        /* user vectors */
        . = 0x100;
        LONG(__vector_64)
        LONG(__vector_65)
        LONG(__vector_66)
        LONG(__vector_67)

        /* default handlers */
        PROVIDE(__bus_error             = __unhandled_exception);
        PROVIDE(__address_error         = __unhandled_exception);
        PROVIDE(__illegal_instruction   = __unhandled_exception);
        PROVIDE(__zero_divide           = __unhandled_exception);
        PROVIDE(__chk                   = __unhandled_exception);
        PROVIDE(__trapv                 = __unhandled_exception);
        PROVIDE(__privilege_violation   = __unhandled_exception);
        PROVIDE(__trace                 = __unhandled_exception);
        PROVIDE(__line_a                = __unhandled_exception);
        PROVIDE(__line_f                = __unhandled_exception);
        PROVIDE(__uninitialized_interrupt = __unhandled_exception);
        PROVIDE(__spurious_interrupt    = __unhandled_exception);
        PROVIDE(__vector_64             = __unhandled_exception);
        PROVIDE(__vector_65             = __unhandled_exception);
        PROVIDE(__vector_66             = __unhandled_exception);
        PROVIDE(__vector_67             = __unhandled_exception);

    } > vectors

    .text :
    {
        *(.text .text.*);
        KEEP(*(INTERRUPT_HANDLERS));
        *(.rodata .rodata.*);
        _etext = .;
        *(.lit);
    } > ram
    
    .data :
    {
        . = ALIGN(0x4);
        *(.got.plt) *(.got);
        *(.shdata);
        *(.data .data.*);
        . = ALIGN(0x4);
        PROVIDE(__commands = .);
        KEEP(*(COMMANDS))
        PROVIDE(__commands_end = .);
        _edata = .;
    } > ram

    .bss :
    {
        . = ALIGN(0x4);
        __bss_start = .; 
        *(.shbss);
        *(.bss .bss.*);
        *(COMMON);
        . = ALIGN(0x8);
        PROVIDE(__stackbot = .);
        . += STACKSIZE;
        PROVIDE(__stacktop = .);
        _end = .;
        __end = _end;
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

ASSERT(_edata <= 32K, "image too large for EEPROM")
