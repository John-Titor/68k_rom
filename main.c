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
    // initialize things
    init_led();
    init_cons();
    led('1');
    putln(banner);

    // REPL
    for (;;) {
        puts("] ");
        char *cmd;
        int ret = -1;
        if ((cmd = gets()) != NULL) {
            for (cmd_handler_fn *cfp = &__commands; cfp < &__commands_end; cfp++) {
                ret = (*cfp)(cmd);
                if (ret >= 0) {
                    break;
                }
            }
            if (ret < 0) {
                putln("ERROR");
            }
        }
    }
}

COMMAND(cmd_help);

static int
cmd_help(const char *input_buffer)
{
    if (scan(input_buffer, "help") < 0) {
        return -1;
    }
    putln("helpful");
    return 0;
}
