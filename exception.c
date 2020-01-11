#include "proto.h"

typedef struct {
    uint32_t    d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
} registers_t;

void
unhandled_exception(registers_t regs __attribute((unused)))
{
    putln("unhandled exception");
}

COMMAND(cmd_exception);

static int
cmd_exception(const char *input_buffer)
{
    uint16_t trap_number;

    if (scan(input_buffer, "trap %w", &trap_number) == 1) {
        switch (trap_number) {
#define TRAP(_x)    case _x: __asm__ volatile ("trap #" #_x ); break
            TRAP(0);
            TRAP(1);
            TRAP(2);
            TRAP(3);
            TRAP(4);
            TRAP(5);
            TRAP(6);
            TRAP(7);
            TRAP(8);
            TRAP(9);
            TRAP(10);
            TRAP(11);
            TRAP(12);
            TRAP(13);
            TRAP(14);
            TRAP(15);

        default:
            return -1;
        }
    }

    if (!strncmp(input_buffer, "reset", 5)) {
        __asm__ volatile("reset");
        return 0;
    }

    return -1;
}