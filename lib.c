#include "proto.h"

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

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n--) {
        char c1 = toupper(*s1++);
        char c2 = toupper(*s2++);

        c1 = toupper(c1);
        c2 = toupper(c2);

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

int
memcmp(const void *s1, const void *s2, size_t n)
{
    while (n--) {
        char c1 = *(const char *)s1++;
        char c2 = *(const char *)s2++;

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }
    }

    return 0;
}

void
putc(char c)
{
    board_putc(c);
}

int
getc()
{
    return board_getc();
}

void
puts(const char *str)
{
    if (str) {
        while (*str != '\0') {
            putc(*str++);
        }
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

static char *
gets_internal(uint32_t timeout, bool echo)
{
    static char buf[100];
    static const uint32_t maxlen = sizeof(buf) - 1;
    uint32_t len = 0;
    uint32_t start_time = timer_get_ticks();

    for (;;) {
        if (timeout
            && ((timer_get_ticks() - start_time) > timeout)) {
            return NULL;
        }
        int c = getc();

        if (c > 0) {
            switch (c) {
            case '\b':
                if (len > 0) {
                    len--;
                    if (echo) {
                        puts("\b \b");
                    }
                }

                break;

            case 0x03: // ^c
                if (echo) {
                    puts("^C\n");
                }
                return NULL;

            case 0x0b: // ^k
            case 0x15: // ^u
                while (len > 0) {
                    len--;
                    if (echo) {
                        puts("\b \b");
                    }
                }

                break;

            case '\r':
            case '\n':
                buf[len] = '\0';
                if (echo) {
                    putc('\n');
                }
                return buf;

            case ' '...126:
                if (len < maxlen) {
                    buf[len++] = c;
                    if (echo) {
                        putc(c);
                    }
                }

                break;

            default:
                break;
            }
        }
    }
}

char *
gets()
{
    return gets_internal(0, true);
}

char *
getln(uint32_t timeout)
{
    return gets_internal(timeout, false);
}

static const char *hextab = "0123456789abcdef";

static void
putx(uint32_t value, size_t len)
{
    uint32_t shifts = len * 2;
    char buf[shifts + 1];
    char *p = buf + shifts;

    *p = '\0';

    do {
        uint32_t nibble = value & 0xf;
        value >>= 4;
        *--p = hextab[nibble];
    } while (p > buf);

    puts(p);
}

static void
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
        uint32_t digit = value % 10;
        value /= 10;
        *--p = hextab[digit];
    }

    puts(p);
}

#define WSELECT(_s, _l, _w, _b) ((_s) == 'l') ? (_l) : ((_s) == 'w') ? (_w) : (_b)

size_t
hexdump(uintptr_t addr, uintptr_t address, size_t length, char width)
{
    uint32_t index;
    uint32_t incr = WSELECT(width, 4, 2, 1);

    length &= ~(incr - 1);

    for (index = 0; index < length; index += 16) {
        putx(address + index, 8);
        putc(':');

        for (uint32_t col = 0; col < 16; col += incr) {
            putc(' ');

            if ((index + col) >= length) {
                uint32_t count = WSELECT(width, 8, 4, 2);

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

        for (uint32_t col = 0; col < 16; col++) {
            if ((index + col) < length) {
                uintptr_t p = (addr + index + col);
                uint32_t c = *(char *)p;

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

/**
 * @brief      printf-style output formatter
 *
 * Supports:
 *  %c      character (char)
 *  %d      signed decimal integer (int)
 *  %u      uint32_t decimal integer (uint32_t int)
 *  %p      pointer (32-bit hex) (const void *)
 *  %b      hex byte (uint8_t)
 *  %w      hex word (uint16_t)
 *  %l      hex long (uint32_t)
 *  %s      string (const char *)
 *
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
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
        case 'c': {
                char c = va_arg(ap, int);
                putc(c);
                break;
            }

        case 'd': {
                int v = va_arg(ap, int);

                if (v < 0) {
                    putc('-');
                    v = -v;
                }

                putd(v);
                break;
            }

        case 'u': {
                uint32_t v = va_arg(ap, uint32_t);
                putd(v);
                break;
            }

        case 'p': {
                void *v = va_arg(ap, void *);
                puts("0x");
                putx((uintptr_t)v, sizeof(v));
                break;
            }

        case 'b': {
                uint8_t v = va_arg(ap, uint32_t);
                putx(v, sizeof(v));
                break;
            }

        case 'w': {
                uint16_t v = va_arg(ap, uint32_t);
                putx(v, sizeof(v));
                break;
            }

        case 'l': {
                uint32_t v = va_arg(ap, uint32_t);
                putx(v, sizeof(v));
                break;
            }

        case 's': {
                const char *v = va_arg(ap, const char *);
                puts(v);
                break;
            }

        case '%': {
            putc('%');
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

/**
 * @brief      scanf-style input scanner
 *
 * Supports:
 *  %c      character (char *)
 *  %l      uint32_t number, decimal or hex with preceding 0x (uint32_t *)
 *  %s      string, needs 2 args, pointer & max length (char *, size_t)
 *
 * @param[in]  buf        buffer to scan
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 *
 * @return     the number of arguments converted, or -1 on error
 */
int
scan(const char *buf, const char *format, ...)
{
    char c;
    va_list ap;
    int ret = 0;
    bool dofmt = 0;

    va_start(ap, format);

    while ((c = *format++) != 0) {
        // input string exhausted; success if we made any conversions
        if (*buf == 0) {
            return ret > 0 ? ret : -1;
        }

        if (!dofmt) {
            // any space in the format discards space in the buffer
            if (isspace(c)) {
                while (isspace(*buf)) {
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
        while (isspace(*buf)) {
            buf++;
        }

        void *vvp = va_arg(ap, void *);

        if (vvp == 0) {
            return -1;
        }

        switch (c) {
        case 'c': {
                *(char *)vvp = *buf++;
                ret++;
                break;
            }

        case 'l': {
                uint32_t *vp = (uint32_t *)vvp;

                if (scan_digits(&buf, vp) < 0) {
                    return -1;
                }

                ret++;
                break;
            }

        case 's': {
                char *vp = (char *)vvp;
                size_t len = va_arg(ap, size_t);

                if (len < 1) {
                    return -1;
                }

                uint32_t index = 0;

                for (;;) {
                    if ((*buf == 0) || isspace(*buf)) {
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
