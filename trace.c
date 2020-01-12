/*
 * Emulator-friendly trace messages
 */
#include "proto.h"

extern int trace_check();
extern void _trace_puts(const char *str);

static bool trace_available = false;

void
trace_puts(const char *str)
{
    if (trace_available) {
        _trace_puts(str);
    }
}

void
init_trace()
{
    trace_available = trace_check();
}
