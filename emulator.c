/*
 * Emulator 'native features' interface.
 */

#include <stdbool.h>
#include <stdint.h>

#include "proto.h"

static bool hasNF;

static uintptr_t nfid_name;
static uintptr_t nfid_stderr;
static uintptr_t nfid_xhdi;
static uintptr_t nfid_shutdown;
static uintptr_t bootstrap_id;
static uintptr_t nfid_config;

#define NF_CONFIG_MMU   0x5f4d4d55L /* '_MMU' */

extern bool _detect_native_features(void);
extern uintptr_t _nfID(const char *);
extern uint32_t _nfCall(long ID, ...);

__asm__
(
"        .equ vec_illegal, 0x10      /* illegal exception vector */     \n"
"_detect_native_features:                                               \n"
"        moveq   #0,%d0              /* assume no NatFeats available */ \n"
"        move.l  %sp,%a1                                                \n"
"        move.l  vec_illegal,%a0                                        \n"
"        move.l  #_fail_natfeat, vec_illegal                            \n"
"        pea     _nf_version_name                                       \n"
"        sub.l   #4,%sp                                                 \n"
"        .dc.w   0x7300              /* Jump to NATFEAT_ID */           \n"
"        tst.l   %d0                                                    \n"
"        jeq     _fail_natfeat                                          \n"
"        moveq   #1,%d0              /* NatFeats detected */            \n"
"                                                                       \n"
"_fail_natfeat:                                                         \n"
"        move.l  %a1,%sp                                                \n"
"        move.l  %a0,vec_illegal                                        \n"
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

void init_emulator(void)
{
    hasNF = _detect_native_features();

    if (hasNF) {
        nfid_name       = _nfID("NF_NAME");
        nfid_stderr     = _nfID("NF_STDERR");
        nfid_xhdi       = _nfID("XHDI");
        nfid_shutdown   = _nfID("NF_SHUTDOWN");
        bootstrap_id    = _nfID("BOOTSTRAP");
        nfid_config     = _nfID("NF_CONFIG");
    }
}

void
emulator_puts(const char *str)
{
    if (nfid_stderr) {
        _nfCall(nfid_stderr, str);
    }
}

COMMAND(emulator);

static int
emulator(const char *input_buffer)
{
    if (input_buffer == NULL) {
        if (nfid_shutdown) {
            puts("shutdown                          shutdown / quit the emulator");
        }
    }
    if (!strcmp(input_buffer, "shutdown") && nfid_shutdown) {
        _nfCall(nfid_shutdown);
    }

    return -1;
}