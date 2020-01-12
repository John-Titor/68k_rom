/*
 * Vector entrypoints
 */

    .align 2
    .text

    /****************************************
     * reset entrypoint
     */
    .global __reset
__reset:
    /* set up a stack frame */
    link    %fp, #-8

    /* zero the BSS (always 32-aligned)  */
    movel   #__bss_start, %d1
    movel   #_end, %d0
    subl    %d1, %d0        /* BSS length */
    asrl    #2, %d0         /* as 32-bit words */
    movel   %d1, %a0        /* word to clear */
    bra     2f
1:
    clrl    (%a0)+
2:
    dbra    %d1, 1b

    /* and call the start routine */
    .extern main
    jbsr    main
    bra     .

    /****************************************
     * default handler for unexpected exceptions
     */
    .global __unhandled_exception
    .extern unhandled_exception
__unhandled_exception:
    movem   %d0-%a6, -(%sp)
    jbsr    unhandled_exception
    bra     .

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