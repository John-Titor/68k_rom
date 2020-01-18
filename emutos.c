#include "proto.h"
#include "pffs/pff.h"

#define TOS_ROM_BASE    0x00e00000

typedef __attribute__((noreturn)) void (*entry_function)(void);

static const char *emutos_image_name = "EMUTOS.SYS";

static int
emutos_load()
{
    char    *emutos_buffer = (char *)TOS_ROM_BASE;
    FATFS   fs;

    board_status(8);

    if (pf_mount(&fs) != FR_OK) {
        putln("disk mount failed");
        return -1;
    }

    if (pf_open(emutos_image_name) != FR_OK) {
        fmt("%s not found\n", emutos_image_name);
        return -1;
    }

    char *bp = emutos_buffer;
    UINT resid = fs.fsize;
    UINT br;
    fmt("loading %s", emutos_image_name);

    while (resid > 0) {
        size_t to_read = (resid > 16384) ? 16384 : resid;

        if ((pf_read(bp, to_read, &br) != FR_OK)
                || (br == 0)) {
            putln(" read error");
            return -1;
        }

        resid -= br;
        bp += br;
        putc('.');
    }

    putc('\n');

    if (strncmp((char *)emutos_buffer + 0x2c, "ETOS", 4)) {
        fmt("%s is not an EmuTOS image\n", emutos_image_name);
        return -1;
    }

    uint32_t version_offset = *(uint32_t *)(emutos_buffer + 0x40);
    fmt("EmuTOS version %s\n", emutos_buffer + version_offset);

    board_status(9);
    board_deinit();
    ((entry_function)TOS_ROM_BASE)();
}

COMMAND(emutos);

static int
emutos(const char *input_buffer)
{
    if (input_buffer == NULL) {
        putln("emutos                            load and run EMUTOS.SYS");

    } else if (!strncasecmp(input_buffer, "emutos", 6)) {
        return emutos_load();
    }

    return -1;
}
