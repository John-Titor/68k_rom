/*
 * Minix loader
 */

#include "proto.h"
#include "pffs/pff.h"

#define MINIX_BASE          0x020000
#define MINIX_ENTRY         0x020200    /* possibly +4 or +8 */

typedef __attribute__((noreturn)) void (*entry_function)(void);

static const char *minix_image_name = "MINIX.SYS";

static int
minix_load()
{
    char    *minix_buffer = (char *)MINIX_BASE;
    FATFS   fs;

    board_status(8);

    if (pf_mount(&fs) != FR_OK) {
        putln("disk mount failed");
        return -1;
    }

    if (pf_open(minix_image_name) != FR_OK) {
        fmt("%s not found\n", minix_image_name);
        return -1;
    }

    char *bp = minix_buffer;
    UINT br;
    fmt("loading %s", minix_image_name);

    if ((pf_read(bp, 512, &br) != FR_OK)
        || (br != 512)) {
        putln(" read error");
        return -1;
    }
    if (!strncmp(bp + 2, "MINIX  ", 6)
        || ((*(uint16_t *)(bp + 504) * 512) != fs.fsize)) {
        putln(" invalid Minix-ST image");
        return -1;
    }

    UINT resid = fs.fsize - br;
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

    board_status(9);
    board_deinit();
    ((entry_function)MINIX_ENTRY)();
}


COMMAND(minix);

static int
minix(const char *input_buffer)
{
    if (input_buffer == NULL) {
        putln("minix                             load and run MINIX.SYS");

    } else if (!strncasecmp(input_buffer, "minix", 6)) {
        return minix_load();
    }

    return -1;
}