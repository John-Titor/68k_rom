/*
 * ROM for Tiny68k
 */

#include "regs.h"
#include "proto.h"

void 
main(void)
{
    // 7seg display init
    init_led();

    // DUART init
    init_cons();
    led('1');
    puts("M68K monitor ROM");

    for (;;) {}
}