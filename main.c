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
    board_init();
    interrupt_enable();

    // signs of life
    trace_puts(banner);
    trace_puts("\n");
    fmt("%s\ntry 'help'\n", banner);

    // REPL
    for (;;) {
        board_status(1);
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

    puts("\n^K clears input line\n"
         "^C aborts input\n\n"
         "Commands:\n"
         "=========\n");
    for (cmd_handler_fn *cfp = &__commands; cfp < &__commands_end; cfp++) {
        (*cfp)(NULL);
    }
    return 0;
}
