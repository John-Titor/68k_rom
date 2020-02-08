#include "proto.h"

typedef struct {
    uint32_t    d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
} registers_t;

void
unhandled_exception(registers_t regs __attribute((unused)))
{
    board_status(0);
    putln("unhandled exception");
}

static inline uint16_t
get_sr()
{
    uint16_t result;
    __asm__ volatile (
        "move.w %%sr, %0"
        : "=d" (result)
        :
        : "memory"
    );
    return result;
}

static inline void
set_sr(uint16_t value)
{
    __asm__ volatile (
        "move.w %0, %%sr"
        :
        : "d" (value)
        : "memory"
    );
}

bool
interrupt_disable()
{
    bool state = ((get_sr() & 0x0700) == 0);
    set_sr(0x2700);
    return state;
}

void
interrupt_enable(bool enable)
{
    if (enable) {
        set_sr(0x2000);
    }
}
