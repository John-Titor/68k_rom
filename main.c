/*
 * ROM for Tiny68k
 */

#include "proto.h"

#define STR(_x) #_x
#define XSTR(_x) STR(_x)
static const char *banner = "68k monitor " XSTR(GITHASH);

void 
main(void)
{
    // 7seg display init
    init_led();

    // DUART init
    init_cons();
    led('1');
    putln(banner);

#ifdef TEST
    cons_test();
#endif

    // REPL
    for (;;) {
        puts("] ");
        char *cmd;
        if ((cmd = gets()) != NULL) {
            fmt("got: %u %s\n", (unsigned)strlen(cmd), cmd);
        }
    }
}
