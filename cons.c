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
        unsigned char c = DUART_RBA;
//        fmt("got %b\n", c);
        return c;
    }
    return -1;
}

char *
gets(void)
{
    static char buf[100];
    static const unsigned maxlen = sizeof(buf) - 1;
    unsigned len = 0;

    for (;;) {
        int c = getc();
        if (c > 0) {
            switch (c) {
                case '\b':
                    if (len > 0) {
                        puts("\b \b");
                        len--;
                    }
                    break;
                case 0x0b: // ^k
                case 0x15: // ^u
                    while (len > 0) {
                        puts("\b \b");
                        len--;
                    }
                    break;
                case '\r':
                case '\n':
                    buf[len] = '\0';
                    putc('\n');
                    return buf;
                case ' '...126:
                    if (len < maxlen) {
                        buf[len++] = c;
                        putc(c);
                    }
                    break;
                default:
                    break;
            }
        }
    }
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
fmt(const char *format, ...)
{
    char c;
    va_list ap;
    bool dofmt = false;

    va_start(ap, format);

    while ((c = *format++) != 0) {
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

/*
 * low-rent scanf-like thing
 *
 * Supports:
 *  %u      unsigned decimal integer
 *  %l      hex long (up to 8 digits)
 *  %s      string, needs 2 args, pointer & max length (unsigned)
 *
 * Returns the number of arguments converted, or -1 if
 * a match fails.
 */
static int
scan_decval(char c) 
{
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }
    return -1;
}

static int
scan_hexval(char c) 
{
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }
    if ((c >= 'a') && (c <= 'f')) {
        return c - 'a' + 10;
    }
    if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    }
    return -1;
}

static int
scan_digits(const char **bp, unsigned long *result, int (*scanner)(char c))
{
    const char *p = *bp;
    *result = 0;

    if (scanner(*p) < 0) {
        return -1;
    }

    int digit;
    while ((digit = scanner(*p)) >= 0) {
        *result *= 10;
        *result += digit;
        p++;
    }
    *bp = p;
    return 0;
}



int
scan(const char *buf, const char *format, ...)
{
    char c;
    va_list ap;
    int ret = 0;
    bool dofmt = 0;

    va_start(ap, format);

    while ((c = *format++) != 0) {
        if (*buf == 0) {
            return -1;
        }
        if (!dofmt) {
            if (c != '%') {
                if (*buf++ != c) {
                    return -1;
                }
            } else {
                dofmt = true;
            }
            continue;
        }
        dofmt = false;

        void *vvp = va_arg(ap, void *);
        if (vvp == 0) {
            return -1;
        }
        switch (c) {
            case 'u':
            {
                unsigned *vp = (unsigned *)vvp;
                unsigned long tv;
                if (scan_digits(&buf, &tv, scan_decval) < 0) {
                    return -1;
                }
                *vp = tv;
                ret++;
                break;
            }
            case 'l':
            {
                unsigned long *vp = (unsigned long *)vvp;
                if (scan_digits(&buf, vp, scan_hexval) < 0) {
                    return -1;
                }
                ret++;
                break;
            }
            case 's':
            {
                char *vp = (char *)vvp;
                unsigned len = va_arg(ap, unsigned);
                if (len < 1) {
                    return -1;
                }
                unsigned index = 0;
                for (;;) {
                    if ((*buf == 0)
                        || (*buf == ' ')
                        || (*buf == '\t')
                        || (*buf == '\n')
                        || (*buf == '\r')) {
                        break;
                    }
                    char c = *buf++;
                    if ((index + 1) < len) {
                        vp[index++] = c;
                    }
                }
                vp[index] = '\0';
                ret++;
                break;
            }
            default:
                if (*buf++ != c) {
                    return -1;
                }
                break;
        }
    }
    return ret;
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
