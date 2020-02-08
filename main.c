/*
 * ROM for Tiny68k
 */

#include "proto.h"

#define STR(_x) #_x
#define XSTR(_x) STR(_x)
static const char *banner = "\n\n68k monitor for %s " XSTR(GITHASH) "\n";

static void cmd_execute(const char *cmd);
static void cmd_script(const char *script);

static const char *auto_script =
//    "emutos"
    "\0";

void
main()
{
    // initialize things
    board_init();
    interrupt_enable(true);

    // signs of life
    trace_fmt(banner, board_name);
    fmt(banner, board_name);

    cmd_script(auto_script);

    // REPL
    putln("\ntry 'help'");

    for (;;) {
        board_status(3);
        puts("] ");
        char *cmd;

        if ((cmd = gets()) != NULL) {
            cmd_execute(cmd);
        }
    }
}

static void
cmd_execute(const char *cmd)
{
    int ret = -1;

    if (strlen(cmd) > 0) {
        for (cmd_handler_fn *cfp = &_commands; cfp < &_ecommands; cfp++) {
            ret = (*cfp)(cmd);
            if (ret >= 0) {
                break;
            }
        }
    }

    if (ret < 0) {
        putln("ERROR");
    }
}

static void
cmd_script(const char *script)
{
    while (*script) {
        fmt("] %s\n", script);
        cmd_execute(script);
        script += strlen(script) + 1;
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

    for (cmd_handler_fn *cfp = &_commands; cfp < &_ecommands; cfp++) {
        (*cfp)(NULL);
    }

    return 0;
}
