/*
 * Emulator 'native features' interface.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "proto.h"

extern bool         _detect_native_features(void);
extern uint32_t     _nfID(const char *);
extern uint32_t     _nfCall(uint32_t ID, ...);

/*
 * d0 - return code
 * d1 - old illegal vector
 * a0 - address of illegal vector
 * a1 - old sp
 */
__asm__
(
#if defined(__mc68010) || defined(__mc68020) || defined(__mc68030) || defined(__mc68040)
"        movec   %vbr, %a0                                              \n"
"        add.l   #0x10, %a0                                             \n"
#else
"        move.l  #0x10, %a0                                             \n"
#endif
"_detect_native_features:                                               \n"
"        moveq   #0, %d0             /* assume no NatFeats available */ \n"
"        move.l  %sp, %a1                                               \n"
"        move.l  (%a0), %d1                                             \n"
"        move.l  #_fail_nf, (%a0)                                       \n"
"        pea     _nf_version_name                                       \n"
"        sub.l   #4, %sp                                                \n"
"        .dc.w   0x7300              /* NATFEAT_ID */                   \n"
"        tst.l   %d0                                                    \n"
"        jeq     _fail_nf            /* expect non-zero ID */           \n"
"        moveq   #1, %d0             /* NatFeats detected */            \n"
"                                                                       \n"
"_fail_nf:                                                              \n"
"        move.l  %a1, %sp                                               \n"
"        move.l  %d1, (%a0)                                             \n"
"                                                                       \n"
"        rts                                                            \n"
"                                                                       \n"
"_nf_version_name:                                                      \n"
"        .ascii  \"NF_VERSION\\0\"                                      \n"
"        .even                                                          \n"
"                                                                       \n"
"_nfID:                                                                 \n"
"        .dc.w   0x7300                                                 \n"
"        rts                                                            \n"
"_nfCall:                                                               \n"
"        .dc.w   0x7301                                                 \n"
"        rts                                                            \n"
);

static int
nf_id(const char *method)
{
    static bool probed, supported;

    if (!probed && _detect_native_features()) {
        supported = true;
    }
    probed = true;
    if (supported) {
        return _nfID(method);
    }
    return -1;
}

static void
nf_puts(const char *str)
{
    static int nfid_stderr = 0;

    if (nfid_stderr == 0) {
        nfid_stderr = nf_id("NF_STDERR");
    }

    if (nfid_stderr > 0) {
        _nfCall(nfid_stderr, str);
    }
}

static void
nf_exit()
{
    static int nfid_shutdown = 0;

    if (nfid_shutdown == 0) {
        nfid_shutdown = nf_id("NF_SHUTDOWN");
    }

    if (nfid_shutdown > 0) {
        _nfCall(nfid_shutdown);
    }
    for (;;) ;
}

void
trace_puts(const char *str)
{
    nf_puts(str);
}

void
trace_putc(const char c)
{
    char buf[2] = {c, 0};
    nf_puts(buf);
}

void
trace_fmt(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _fmt(trace_putc, format, ap);
    va_end(ap);
}

COMMAND(emulator);

static int
emulator(const char *input_buffer)
{
    if (input_buffer == NULL) {
        puts("shutdown                          shutdown / quit if running in an emulator\n");
    }
    if (!strcmp(input_buffer, "shutdown")) {
        nf_exit();
    }

    return -1;
}