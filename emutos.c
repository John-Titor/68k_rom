#include "proto.h"
#include "pffs/pff.h"

#define TOS_ROM_BASE    0x00e00000

typedef __attribute__((noreturn)) void (*entry_function)(void);

static const char *emutos_image_name = "EMUTOS.SYS";

int
emutos_load()
{
    char    *emutos_buffer = (char *)TOS_ROM_BASE;
    FATFS   fs;
    UINT    br;

    board_status(8);

    if (pf_mount(&fs) != FR_OK) {
        putln("EmuTOS: disk mount failed");
    } else if (pf_open(emutos_image_name) != FR_OK) {
        fmt("EmuTOS: %s not found\n", emutos_image_name);
    } else if ((pf_read(emutos_buffer, fs.fsize, &br) != FR_OK)
               || (br != fs.fsize)) {
        fmt("EmuTOS: %s read failed\n", emutos_image_name);
    } else if (strncmp((char *)emutos_buffer + 0x2c, "ETOS", 4)) {
        fmt("EmuTOS: %s is not an EmuTOS image\n", emutos_image_name);
    } else {
        uint32_t version_offset = *(uint32_t *)(emutos_buffer + 0x40);
        fmt("EmuTOS version %s\n", emutos_buffer + version_offset);

        board_status(9);
        board_deinit();
        ((entry_function)TOS_ROM_BASE)();
    }

    return -1;
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
