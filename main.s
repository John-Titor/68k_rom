#NO_APP
	.file	"main.c"
	.text
	.section	.rodata.cmd_execute.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ERROR"
	.section	.text.cmd_execute,"ax",@progbits
	.align	2
	.type	cmd_execute, @function
cmd_execute:
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	move.l 12(%sp),%d3
	move.l %d3,-(%sp)
	jsr strlen
	addq.l #4,%sp
	move.l #_commands,%d2
	tst.l %d0
	jne .L3
.L2:
	move.l #.LC0,12(%sp)
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	jra putln
.L5:
	addq.l #4,%d2
	move.l %d3,-(%sp)
	move.l (%a0),%a0
	jsr (%a0)
	addq.l #4,%sp
	tst.l %d0
	jge .L1
.L3:
	move.l %d2,%a0
	cmp.l #_ecommands,%d2
	jcs .L5
	jra .L2
.L1:
	move.l (%sp)+,%d2
	move.l (%sp)+,%d3
	rts
	.size	cmd_execute, .-cmd_execute
	.section	.rodata.cmd_help.str1.1,"aMS",@progbits,1
.LC1:
	.string	"help"
.LC2:
	.string	"\n^K clears input line\n^C aborts input\n\nCommands:\n=========\n"
	.section	.text.cmd_help,"ax",@progbits
	.align	2
	.type	cmd_help, @function
cmd_help:
	move.l %a2,-(%sp)
	pea .LC1
	move.l 12(%sp),-(%sp)
	jsr scan
	addq.l #8,%sp
	tst.l %d0
	jlt .L13
	pea .LC2
	jsr puts
	addq.l #4,%sp
	lea _commands,%a2
.L11:
	cmp.l #_ecommands,%a2
	jcs .L12
	moveq #0,%d0
.L9:
	move.l (%sp)+,%a2
	rts
.L12:
	clr.l -(%sp)
	move.l (%a2)+,%a0
	jsr (%a0)
	addq.l #4,%sp
	jra .L11
.L13:
	moveq #-1,%d0
	jra .L9
	.size	cmd_help, .-cmd_help
	.section	.rodata.main.str1.1,"aMS",@progbits,1
.LC3:
	.string	"AUTO_SCRIPT"
.LC4:
	.string	"\n\n68k monitor for %s bad8ae4-modified\n"
.LC5:
	.string	"] %s\n"
.LC6:
	.string	"\ntry 'help'"
.LC7:
	.string	"] "
	.section	.text.startup.main,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
	movem.l #62,-(%sp)
	jsr board_init
	pea 1.w
	jsr interrupt_enable
	move.l board_name,-(%sp)
	pea .LC4
	jsr trace_fmt
	move.l board_name,-(%sp)
	pea .LC4
	lea fmt,%a3
	jsr (%a3)
	lea (20,%sp),%sp
	lea .LC3,%a2
	lea cmd_execute,%a4
	lea strlen,%a6
	move.l %a4,%a5
.L15:
	move.l %a2,-(%sp)
	pea .LC5
	jsr (%a3)
	move.l %a2,-(%sp)
	jsr (%a4)
	move.l %a2,-(%sp)
	jsr (%a6)
	lea 1(%a2,%d0.l),%a2
	lea (16,%sp),%sp
	tst.b (%a2)
	jne .L15
	pea .LC6
	jsr putln
	addq.l #4,%sp
	lea board_status,%a2
	lea puts,%a3
	lea gets,%a4
.L16:
	pea 3.w
	jsr (%a2)
	pea .LC7
	jsr (%a3)
	jsr (%a4)
	addq.l #8,%sp
	tst.l %d0
	jeq .L16
	move.l %d0,-(%sp)
	jsr (%a5)
	addq.l #4,%sp
	jra .L16
	.size	main, .-main
	.section	COMMANDS,"a"
	.align	2
	.type	cmdptr_cmd_help, @object
	.size	cmdptr_cmd_help, 4
cmdptr_cmd_help:
	.long	cmd_help
	.ident	"GCC: (crosstool-NG 1.24.0) 8.3.0"
