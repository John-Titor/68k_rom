#include "proto.h"
#include "regs.h"
#include "pffs/pff.h"

static FATFS fs;

COMMAND(fs_load);

#define TOS_ROM_BASE    0x00e00000

static int
fs_load(const char *input_buffer)
{
    char filename[13];

    if (scan(input_buffer, "emutos %s", filename) == 1) {
        uint8_t *emutos_buffer = (void *)TOS_ROM_BASE;
        int len = fs_read(filename, emutos_buffer, RAM_END - TOS_ROM_BASE);
        if (len < 0) {
            putln("TOS: no file");
            return -1;
        }
        if ((len < 0x44)
            || strncmp((char *)emutos_buffer + 0x2c, "ETOS", 4)) {
            putln("TOS: bad file");
            return -1;
        }
        uint32_t version_offset = *(uint32_t *)(emutos_buffer + 0x40);
        fmt("EmuTOS version %s\n", emutos_buffer + version_offset);
        return 0;
    }
    return -1;
}

int
fs_read(const char *name, void *buffer, size_t buffer_size)
{
    UINT    br;

    if (pf_open(name) != FR_OK) {
        return -1;
    }
    if (pf_read(buffer, buffer_size, &br) != FR_OK) {
        return -1;
    }
    return br;
}

int
init_fs()
{
    return (pf_mount(&fs) == FR_OK) ? 0 : -1;
}