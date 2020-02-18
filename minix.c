/*
 * Minix loader
 *
 * The minix image consists of six programs: boot_dd, kernel, mm, fs, init and menu.
 * boot_dd is 512B long and is not loaded, but the layout table (last 32 bytes) is
 * used to arrange the remaining programs.
 *
 * The table consists of 16 words counted backwards from the end:
 *
 * 16  kernel on-disk size (512B sectors)
 * 15  kernel BSS padding (512B blocks)
 * 14  mm on-disk size (512B sectors)
 * 13  mm BSS padding (512B blocks)
 * 12  fs on-disk size (512B sectors)
 * 11  fs BSS padding (512B blocks)
 * 10  init on-disk size (512B sectors)
 *  9  init BSS padding (512B blocks)
 *  8  menu on-disk size (512B sectors)
 *  7  menu BSS padding (512B blocks)
 *  6  spare (zero)
 *  5  boot_dd checksum fixup (sector words must sum to 0x1234)
 *  4  sum of size of kernel, mm, fs, init, menu (including padding)
 *  3  spare (base of menu)
 *  2  spare (zero)
 *  1  spare (base of menu)
 * 
 */

#ifdef WITH_MINIX

#include "proto.h"
#include "pffs/pff.h"

#define MINIX_BASE          0x000000

static const char *minix_image_name = "MINIX.IMG";

static int
minix_load()
{

    board_status(8);

    FATFS       fs;
    if (pf_mount(&fs) != FR_OK) {
        putln("disk mount failed");
        return -1;
    }
    if (pf_open(minix_image_name) != FR_OK) {
        fmt("%s not found\n", minix_image_name);
        return -1;
    }

    uint16_t bootblk[256];
    UINT br;
    fmt("validating %s...", minix_image_name);

    if ((pf_read(bootblk, 512, &br) != FR_OK)
        || (br != 512)) {
        putln(" read error");
        return -1;
    }
    uint16_t sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += bootblk[i];
    }
    uint16_t nsec = 0;
    for (int i = 16; i > 6; i--) {
        nsec += bootblk[256 - i];
    }
    if (strncmp((const char *)&bootblk[1], "MINIX ", 6)
        || (sum != 0x1234)
        || (nsec != bootblk[256 - 4])) {
        putln(" bootblock not valid");
        return -1;
    }
    putln("ok.");
    init_loader();

    uint8_t buf[512];
    uint32_t dst = MINIX_BASE;
    for (int i = 16; i > 6; i -= 2) {
        UINT read_count = bootblk[256 - i]; 
        fmt(" %u", read_count);
        while (read_count > 0) {
            if ((pf_read(buf, 512, &br) != FR_OK)
                || (br != 512)) {
                putln(" read error");
                return -1;
            }
            loader_load_bytes(dst, buf, 512);
            if (dst == MINIX_BASE) {
                uint32_t *entryp = (uint32_t *)(&buf[4]);
                loader_set_entry(*entryp);
            }

            dst += 512;
            read_count--;
        }

        int zero_count = bootblk[256 - i + 1];
        fmt("+%u", zero_count);
        memset(buf, 0, sizeof(buf));
        while (zero_count > 0) {
            loader_load_bytes(dst, buf, 512);
            dst += 512;
            zero_count--;
        }
    }
    putln(" booting...");
    loader_go();
    return -1;
}


COMMAND(minix);

static int
minix(const char *input_buffer)
{
    if (input_buffer == NULL) {
        fmt("minix                             load and run %s\n", minix_image_name);

    } else if (!strncasecmp(input_buffer, "minix", 6)) {
        return minix_load();
    }

    return -1;
}

#endif // WITH_MINIX