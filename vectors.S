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
