#include "proto.h"

size_t
strlen(const char *s)
{
    size_t len = 0;

    while (*s++ != '\0') {
        len++;
    }
    return len;
}

int
strcmp(const char *s1, const char *s2)
{
    for (;;) {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 < c2) {
            return -1;
        }
        if (c1 > c2) {
            return 1;
        }
        if (c1 == 0) {
            return 0;
        }
    }
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
    while (n--) {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 < c2) {
            return -1;
        }
        if (c1 > c2) {
            return 1;
        }
        if (c1 == 0) {
            return 0;
        }
    }
    return 0;
}

void
putc(char c)
{
    cons_putc(c);
}

int
getc()
{
    return cons_getc();
}

void
puts(const char *str)
{
    if (str) {
        while (*str != '\0')
            putc(*str++);
    } else {
        puts("(null)");
    }
}

void
putln(const char *str)
{
    puts(str);
    putc('\n');
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
                case 0x03: // ^c
                    puts("^C\n");
                    return NULL;
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

#define WSELECT(_s, _l, _w, _b) ((_s) == 'l') ? (_l) : ((_s) == 'w') ? (_w) : (_b)

unsigned
hexdump(uintptr_t addr, uintptr_t address, unsigned length, char width)
{
    unsigned index;
    unsigned incr = WSELECT(width, 4, 2, 1);

    length &= ~(incr - 1);

    for (index = 0; index < length; index += 16) {
        putx(address + index, 8);
        putc(':');

        for (unsigned col = 0; col < 16; col += incr) {
            putc(' ');
            if ((index + col) >= length) {
                unsigned count = WSELECT(width, 8, 4, 2);
                while (count--) {
                    putc(' ');
                }
            } else {
                uintptr_t p = (addr + index + col);
                uint32_t val = WSELECT(width, *(uint32_t *)p, *(uint16_t *)p, *(uint8_t *)p);
                putx(val, incr);
            }
        }
        puts("  ");
        for (unsigned col = 0; col < 16; col++) {
            if ((index + col) < length) {
                uintptr_t p = (addr + index + col);
                unsigned c = *(char *)p;
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
    return length;
}

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
            case 'c':
            {
                char c = va_arg(ap, int);
                putc(c);
                break;
            }
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
scan_digits(const char **bp, uint32_t *result)
{
    int (*scanner)(char c);
    const char *p = *bp;
    *result = 0;
    int scaler = 0;

    // auto-detect hex vs. decimal (could learn more prefixes?)
    if ((strlen(p) >= 3)
        && !strncmp(p, "0x", 2)
        && (scan_hexval(*(p + 2)) >= 0)) {
        scanner = scan_hexval;
        scaler = 16;
        p += 2;
    } else if (scan_decval(*p) >= 0) {
        scanner = scan_decval;
        scaler = 10;
    } else {
        return -1;
    }

    int digit;
    while ((digit = scanner(*p)) >= 0) {
        *result *= scaler;
        *result += digit;
        p++;
    }
    *bp = p;
    return 0;
}

#define ISSPACE(_x) (((_x) == ' ') || ((_x) == '\t'))

int
scan(const char *buf, const char *format, ...)
{
    char c;
    va_list ap;
    int ret = 0;
    bool dofmt = 0;

    va_start(ap, format);

    while ((c = *format++) != 0) {
        // input string exhausted
        if (*buf == 0) {
            return ret;
        }
        if (!dofmt) {
            // any space in the format discards space in the buffer
            if (ISSPACE(c)) {
                while (ISSPACE(*buf)) {
                    buf++;
                }
                continue;
            }
            // any non-space in the format must match in the buffer
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

        // leading whitespace before conversions is always discarded
        while (ISSPACE(*buf)) {
            buf++;
        }
        void *vvp = va_arg(ap, void *);
        if (vvp == 0) {
            return -1;
        }
        switch (c) {
            case 'c':
            {
                *(char *)vvp = *buf++;
                ret++;
                break;
            }
            case 'w':
            {
                uint32_t tv;
                if (scan_digits(&buf, &tv) < 0) {
                    return -1;
                }
                *(uint16_t *)vvp = tv;
                ret++;
                break;
            }
            case 'l':
            {
                uint32_t *vp = (uint32_t *)vvp;
                if (scan_digits(&buf, vp) < 0) {
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
