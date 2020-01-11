#include "proto.h"

static void
cons_test()
{
    putc('A');
    putc('\n');
    puts("puts test:");
    puts(" one line\n");
    putln("putln test: one line");
    fmt("fmt test:\n"
        "%%d -5678: %d\n"
        "%%u 1234: %u\n"
        "%%p &fmt: %p\n"
        "%%b 0x12: %b\n"
        "%%w 0x3456: %w\n"
        "%%l 0x7890abcd: %l\n"
        "%%s test message: %s\n"
        "%%s NULL: %s\n",
        -5678,
        1234,
        fmt,
        0x12,
        0x3456,
        0x7890abcd,
        "test message",
        NULL);

    const char *scantest = "test  a 1234 0x789 0xabcDE";
    const char *scanfmt = "test %c %w %w %l";
    char v1 = 0;
    uint16_t v2 = 0;
    uint16_t v3 = 0;
    uint32_t v4 = 0;
    int result = scan(scantest, scanfmt, &v1, &v2, &v3, &v4);

    fmt("scan '%s' -> %d, %c %u 0x%w 0x%l\n", scantest, result, v1, v2, v3, v4);
}

static void
cf_test()
{
    putln("CF read LBA 0...");
    void *buf = cf_read(0);

    if (buf) {
        hexdump((uintptr_t)buf, 0, 512, 'b');
    } else {
        putln("CF read failed (no device?)");
    }
}

#ifdef TEST
COMMAND(cmd_test);
#endif

static int
cmd_test(const char *input_buffer)
{
    if (scan(input_buffer, "test") < 0) {
        return -1;
    }

    cons_test();
    cf_test();
    // XXX add more tests

    return 0;
}