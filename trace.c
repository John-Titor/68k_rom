/*
 * Emulator-friendly trace messages
 */
#include "proto.h"

extern int trace_check();
extern void _trace_puts(const char *str);

static int trace_available = -1;

void
trace_puts(const char *str)
{
    if (trace_available < 0) {
        trace_available = trace_check();
    }

    if (trace_available) {
        _trace_puts(str);
    }
}
