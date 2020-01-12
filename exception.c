#include "proto.h"

typedef struct {
    uint32_t    d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
} registers_t;

void
unhandled_exception(registers_t regs __attribute((unused)))
{
    putln("unhandled exception");
}

#define set_sr(a)                         \
    __extension__                             \
    ({short _r, _a = (a);                     \
        __asm__ volatile                        \
        ("move.w %%sr,%0\n\t"                   \
         "move.w %1,%%sr"                       \
         : "=&d"(_r)        /* outputs */        \
         : "nd"(_a)         /* inputs  */        \
         : "cc", "memory"   /* clobbered */      \
        );                                      \
        _r;                                     \
    })

uint16_t
interrupt_disable()
{
    return set_sr(0x2700);
}

void
interrupt_enable()
{
    set_sr(0x2000);
}

void
interrupt_restore(uint16_t state)
{
    set_sr(state);
}