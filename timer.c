/*
 * Generic timebase
 */

#include "proto.h"

static volatile uint32_t timer_ticks = 0;

void
timer_tick()
{
    timer_ticks++;
}

uint32_t
timer_get_ticks()
{
    unsigned s = interrupt_disable();
    uint32_t ret = timer_ticks;
    interrupt_restore(s);
    return ret;
}

COMMAND(timer_command);

static int
timer_command(const char *input_buffer)
{
    if (input_buffer == NULL) {
        putln("timer                             print current timer tick count");
    } else if (scan(input_buffer, "timer") >= 0) {
        fmt("TIMER: %u\n", timer_ticks);
        return 0;
    }

    return -1;
}
