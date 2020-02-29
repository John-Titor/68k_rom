#ifdef WITH_EMUTOS

#include "proto.h"
#include "pffs/pff.h"

#define TOS_ROM_BASE    0x00e00000

typedef __attribute__((noreturn)) void (*entry_function)(void);

static const char *emutos_image_name = "EMUTOS.SYS";

static void
emutos_load()
{
    uint8_t fs_buffer[4096];
    FATFS   fs;

    board_status(8);

    if (pf_mount(&fs) != FR_OK) {
        putln("disk mount failed");
        return;
    }

    if (pf_open(emutos_image_name) != FR_OK) {
        fmt("%s not found\n", emutos_image_name);
        return;
    }

    uintptr_t bp = TOS_ROM_BASE;
    UINT resid = fs.fsize;
    UINT br;
    fmt("loading %s", emutos_image_name);
    init_loader();

    while (resid > 0) {
        size_t to_read = (resid > sizeof(fs_buffer)) ? sizeof(fs_buffer) : resid;

        if ((pf_read(fs_buffer, to_read, &br) != FR_OK)
                || (br == 0)) {
            putln(" read error");
            return;
        }
        loader_load_bytes(bp, fs_buffer, br);

        resid -= br;
        bp += br;
        putc('.');
    }

    putc('\n');

    const char *emutos_buffer = (const char *)TOS_ROM_BASE;
    if (strncmp((char *)emutos_buffer + 0x2c, "ETOS", 4)) {
        fmt("%s is not an EmuTOS image\n", emutos_image_name);
        return;
    }

    uint32_t version_offset = *(uint32_t *)(emutos_buffer + 0x40);
    fmt("EmuTOS version %s\n", emutos_buffer + version_offset);

    loader_set_entry(TOS_ROM_BASE);
    loader_go(0, 0);
}

COMMAND(emutos);

static int
emutos(const char *input_buffer)
{
    if (input_buffer == NULL) {
        putln("emutos                            load and run EMUTOS.SYS");

    } else if (!strncasecmp(input_buffer, "emutos", 6)) {
        emutos_load();
    }

    return -1;
}

#endif // WITH_EMUTOS