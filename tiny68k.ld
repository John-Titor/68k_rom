/*
 * Linker script for ROM on Tiny68k SBC
 */

OUTPUT_ARCH(m68k)
ENTRY(__reset)

MEMORY
{
	ram(rwx) : ORIGIN = 0, LENGTH = 16M - 32K
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
		LONG(0)
		LONG(0)
		LONG(0)	/* '010 format error */
		LONG(__uninitialized_interrupt)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(0)
		LONG(__spurious_interrupt)
		LONG(__irq_1)
		LONG(__irq_2)
		LONG(__irq_3)
		LONG(__irq_4)
		LONG(__irq_5)
		LONG(__irq_6)
		LONG(__irq_7)
		LONG(__trap_0)
		LONG(__trap_1)
		LONG(__trap_2)
		LONG(__trap_3)
		LONG(__trap_4)
		LONG(__trap_5)
		LONG(__trap_6)
		LONG(__trap_7)
		LONG(__trap_8)
		LONG(__trap_9)
		LONG(__trap_a)
		LONG(__trap_b)
		LONG(__trap_c)
		LONG(__trap_d)
		LONG(__trap_e)
		LONG(__trap_f)

		/* default handlers */
		PROVIDE(__bus_error = __unhandled_exception);
		PROVIDE(__address_error = __unhandled_exception);
		PROVIDE(__illegal_instruction = __unhandled_exception);
		PROVIDE(__zero_divide = __unhandled_exception);
		PROVIDE(__chk = __unhandled_exception);
		PROVIDE(__trapv = __unhandled_exception);
		PROVIDE(__privilege_violation = __unhandled_exception);
		PROVIDE(__trace = __unhandled_exception);
		PROVIDE(__line_a = __unhandled_exception);
		PROVIDE(__line_f = __unhandled_exception);
		PROVIDE(__uninitialized_interrupt = __unhandled_exception);
		PROVIDE(__spurious_interrupt = __unhandled_exception);
		PROVIDE(__irq_1 = __unhandled_exception);
		PROVIDE(__irq_2 = __unhandled_exception);
		PROVIDE(__irq_3 = __unhandled_exception);
		PROVIDE(__irq_4 = __unhandled_exception);
		PROVIDE(__irq_5 = __unhandled_exception);
		PROVIDE(__irq_6 = __unhandled_exception);
		PROVIDE(__irq_7 = __unhandled_exception);
		PROVIDE(__trap_0 = __unhandled_exception);
		PROVIDE(__trap_1 = __unhandled_exception);
		PROVIDE(__trap_2 = __unhandled_exception);
		PROVIDE(__trap_3 = __unhandled_exception);
		PROVIDE(__trap_4 = __unhandled_exception);
		PROVIDE(__trap_5 = __unhandled_exception);
		PROVIDE(__trap_6 = __unhandled_exception);
		PROVIDE(__trap_7 = __unhandled_exception);
		PROVIDE(__trap_8 = __unhandled_exception);
		PROVIDE(__trap_9 = __unhandled_exception);
		PROVIDE(__trap_a = __unhandled_exception);
		PROVIDE(__trap_b = __unhandled_exception);
		PROVIDE(__trap_c = __unhandled_exception);
		PROVIDE(__trap_d = __unhandled_exception);
		PROVIDE(__trap_e = __unhandled_exception);
		PROVIDE(__trap_f = __unhandled_exception);

	} > ram

	.text :
	{
		*(.text .text.*);
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
