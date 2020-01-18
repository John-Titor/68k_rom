/*
 * Vector entrypoints
 */

    .align 2
    .text

    /****************************************
     * reset entrypoint
     */
    .global _reset
    .section .entrypoint
_reset:
    reset                   /* reset peripherals */
    move.w #0x2700, %sr     /* make sure interrupts are off */
    lea     _stack_top, %sp /* ensure stack pointer */

    /* copy ourselves to the run area */
    lea     _reloc_source, %a0
    lea     _reloc_destination, %a1
    lea     _reloc_end, %a2
    jra     2f
1:
    move.l  (%a0)+, (%a1)+
2:
    cmp.l   %a1, %a2
    jne     1b

    /* force a long jump to the run location */
    lea     3f, %a0
    jra     (%a0)
3:

    /* zero the BSS (always 32-aligned)  */
    lea     _edata, %a0
    lea     _ebss, %a1
    jra     5f
4:
    clrl    (%a0)+
5:
    cmp.l   %a0, %a1
    jne     4b

    /* set up a stack frame */
    link    %fp, #-8

    /* and call the start routine */
    .extern main
    jbsr    main
    jra     .

    /****************************************
     * default handler for unexpected exceptions
     */
    .section .text
    .global vector_unhandled_exception
    .extern unhandled_exception

vector_unhandled_exception:
    movem   %d0-%a6, -(%sp)
    jbsr    unhandled_exception
    jra     .

    /****************************************
     * emulator trace output
     */
    .global trace_check
    .extern trace_available

trace_check:
    move.l  0x28, %a0           /* save old lineA vector */
    move.l  %sp, %a1            /* save stack pointer */
    move.l  #1f, 0x28           /* redirect lineA to failure */
    pea     4f                  /* bogus argument */
    sub.l   #4, %sp              /* bogus return address */
    .dc.w   0xa0ff              /* trap */
    .dc.l   0                   /* compatibility padding */
    move.l  #1, %d0             /* success */
    bra     2f
1:
    clr.l   %d0                 /* failure */
2:
    move.l  %a1, %sp            /* recover stack pointer */
    move.l  %a0, 0x28           /* restore lineA vector */
    rts

4:
    .ascii  "\0"
    .even

    .global _trace_puts

_trace_puts:
    .dc.w   0xa0ff
    .dc.l   0
    rts