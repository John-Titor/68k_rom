/*
 * ROM for Tiny68k
 */

#include "proto.h"

#define STR(_x) #_x
#define XSTR(_x) STR(_x)
static const char *banner = "68k monitor " XSTR(GITHASH);

void
main()
{
    // initialize things
    init_led();
    init_cons();
    led('1');
    fmt("%s\ntry 'help'\n", banner);
    init_cf();

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

    puts(
        "Commands:\n"
        "=========\n"
        "md.[bwl] [address [length]]   dump bytes/words/longs\n"
        "reset                         reset peripherals\n"
#ifdef TEST
        "tests                         run unit tests\n"
#endif
        "\n^K clears input line\n"
        "^C aborts input\n\n"
    );
    return 0;
}
