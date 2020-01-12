/*
 * ROM for Tiny68k
 */

#include "proto.h"

#define STR(_x) #_x
#define XSTR(_x) STR(_x)
static const char *banner = "68k monitor " XSTR(GITHASH);

void
init_all()
{
    // initialize things
    init_trace();
    init_led();
//    init_timer();
    init_cons();
    led('1');
    init_fs();
}

void
deinit_all()
{
    // quiesce the system for boot
//    deinit_timer();
    deinit_cons();
}

void
main()
{
    init_all();

    trace_puts(banner); trace_puts("\n");
    fmt("%s\ntry 'help'\n", banner);
 
    // REPL
    for (;;) {
        puts("] ");
        char *cmd;
        int ret = -1;

        if ((cmd = gets()) != NULL) {
            if (strlen(cmd) == 0) {
                continue;
            }
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
