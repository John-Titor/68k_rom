#include "proto.h"

typedef struct {
    uint32_t    d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6;
} registers_t;

__attribute__((noreturn))
static void
fatal_exception(const char *kind, registers_t *regs __attribute((unused)))
{
    board_status(0);
    fmt("fatal: %s\n", kind);
    for (;;);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_bus_error(registers_t regs __attribute((unused)))
{
    fatal_exception("bus error", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_address_error(registers_t regs __attribute((unused)))
{
    fatal_exception("address error", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_illegal_instruction(registers_t regs __attribute((unused)))
{
    fatal_exception("illegal instruction", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_zero_divide(registers_t regs __attribute((unused)))
{
    fatal_exception("zero divide", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_chk(registers_t regs __attribute((unused)))
{
    fatal_exception("chk", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_trapv(registers_t regs __attribute((unused)))
{
    fatal_exception("trapv", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_privilege_violation(registers_t regs __attribute((unused)))
{
    fatal_exception("privilege violation", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_trace(registers_t regs __attribute((unused)))
{
    fatal_exception("trace", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_line_a(registers_t regs __attribute((unused)))
{
    fatal_exception("line a", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_line_f(registers_t regs __attribute((unused)))
{
    fatal_exception("line f", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_uninitialized_interrupt(registers_t regs __attribute((unused)))
{
    fatal_exception("uninitialized interrupt", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_spurious_interrupt(registers_t regs __attribute((unused)))
{
    fatal_exception("spurious interrupt", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_64(registers_t regs __attribute((unused)))
{
    fatal_exception("vector_64", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_65(registers_t regs __attribute((unused)))
{
    fatal_exception("vector_65", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_66(registers_t regs __attribute((unused)))
{
    fatal_exception("vector_66", &regs);
}

INTERRUPT_HANDLER
__attribute__((weak))
void
vector_67(registers_t regs __attribute((unused)))
{
    fatal_exception("vector_67", &regs);
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
