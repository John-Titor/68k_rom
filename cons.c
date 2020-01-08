#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "regs.h"
#include "proto.h"

void
init_cons(void)
{
    DUART_CRA = DUART_CR_MRRST;
    DUART_MRA = DUART_MR1_8BIT | DUART_MR1_NO_PARITY | DUART_MR1_RTS;
    DUART_MRA = DUART_MR2_1STOP;
    DUART_CSRA = DUART_CSR_9600B;
    DUART_CRA = DUART_CR_TXRST | DUART_CR_TXEN;
    DUART_CRA = DUART_CR_RXRST | DUART_CR_RXEN;
}

void
puts(const char *str)
{
    if (str)
        while (*str != '\0')
            putc(*str++);
}

void
putln(const char *str)
{
    puts(str);
    putc('\n');
}

void
putc(char c)
{
    if (c == '\n')
        putc('\r');
    for (;;) {
        if (DUART_SRA & DUART_SR_TRANSMITTER_READY) {
            DUART_TBA = c;
            break;
        }
    }
}

int
getc(void)
{
    if (DUART_SRA & DUART_SR_RECEIVER_READY) {
        return DUART_RBA;
    }
    return -1;
}

static const char *hextab = "0123456789abcdef";

void
putx(uint32_t value, unsigned len)
{
    unsigned shifts = len * 2;
    char buf[shifts + 1];
    char *p = buf + shifts;

    *p = '\0';
    do {
        unsigned nibble = value & 0xf;
        value >>= 4;
        *--p = hextab[nibble];
    } while (p > buf);
    puts(p);
}

void
putd(uint32_t value)
{
    if (value == 0) {
        putc('0');
        return;
    }

    char buf[11];
    char *p = buf + sizeof(buf) - 1;
    *p = '\0';

    while (value > 0) {
        unsigned digit = value % 10;
        value /= 10;
        *--p = hextab[digit];
    }
    puts(p);
}

void
hexdump(const void *addr, uint32_t address, unsigned length)
{
    unsigned char *p = (unsigned char *)addr;

    unsigned index;
    for (index = 0; index < length; index += 16) {
        putx(address + index, 8);
        putc(':');

        unsigned col;
        for (col = 0; col < 16; col++) {
            if ((index + col) < length) {
                unsigned c = *(p + index + col);
                putc(' ');
                putx(c, 1);
            } else {
                puts("   ");
            }
        }
        puts("  ");
        for (col = 0; col < 16; col++) {
            if ((index + col) < length) {
                unsigned c = *(p + index + col);
                if ((c >= ' ') && (c < 127)) {
                    putc(c);
                } else {
                    putc('.');
                }
            } else {
                putc(' ');
            }
        }
        putc('\n');
    }
}

/*
 * low-rent printf-like thing
 *
 * Supports:
 *  %d      signed decimal integer
 *  %u      unsigned decimal integer
 *  %p      pointer (32-bit hex)
 *  %b      hex byte
 *  %w      hex word
 *  %l      hex long
 *  %s      string
 */
void
fmt(const char *str, ...)
{
    char c;
    va_list ap;
    bool dofmt = false;

    va_start(ap, str);

    while ((c = *str++) != 0) {
        if (!dofmt) {
            if (c == '%') {
                dofmt = true;
            } else {
                putc(c);
            }
            continue;
        }
        dofmt = false;
        switch (c) {
            case 'd':
            {
                int v = va_arg(ap, int);
                if (v < 0) {
                    putc('-');
                    v = -v;
                }
                putd(v);
                break;
            }
            case 'u':
            {
                unsigned v = va_arg(ap, unsigned);
                putd(v);
                break;
            }
            case 'p':
            {
                void *v = va_arg(ap, void *);
                puts("0x");
                putx((uintptr_t)v, sizeof(v));
                break;
            }
            case 'b':
            {
                uint8_t v = va_arg(ap, int);
                putx(v, sizeof(v));
                break;
            }
            case 'w':
            {
                uint16_t v = va_arg(ap, unsigned);
                putx(v, sizeof(v));
                break;
            }
            case 'l':
            {
                uint32_t v = va_arg(ap, uint32_t);
                putx(v, sizeof(v));
                break;
            }
            case 's':
            {
                const char *v = va_arg(ap, const char *);
                puts(v);
                break;
            }
            default:
                putc('%');
                putc(c);
                break;
        }
    }
}

void
cons_test(void)
{
    putc('A'); putc('\n');
    puts("puts test:"); puts(" one line\n");
    putln("putln test: one line");
    fmt("fmt test:\n"
        "%%d -5678: %d\n"
        "%%u 1234: %u\n"
        "%%p &fmt: %p\n"
        "%%b 0x12: %b\n"
        "%%w 0x3456: %w\n"
        "%%l 0x7890abcd: %l\n"
        "%%s test message: %s\n",
        -5678,
        1234,
        fmt,
        0x12,
        0x3456,
        0x7890abcd,
        "test message");
}
