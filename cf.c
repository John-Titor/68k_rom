#include "proto.h"
#include "regs.h"

#define SECTOR_SIZE     512
#define SECTOR_WORDS    (SECTOR_SIZE / 2)

static uint16_t  cf_sector_buffer[SECTOR_WORDS];
static bool cf_detected = false;

static int
cf_command(uint8_t command, uint32_t lba, uint8_t count)
{
    if ((!cf_detected)
            || (lba > IDE_MAXLBA)) {
        return -1;
    }

    // XXX check BSY here

    IDE_LBA_3 = ((lba >> 24) & 0x3f) | IDE_LBA_3_LBA;
    IDE_LBA_2 = (lba >> 16) & 0xff;
    IDE_LBA_1 = (lba >> 8) & 0xff;
    IDE_LBA_0 = lba & 0xff;
    IDE_SECTOR_COUNT = count;
    IDE_COMMAND = command;

    // XXX timeout
    for (;;) {
        uint8_t status = IDE_STATUS;

        if (status & IDE_STATUS_ERR) {
            return IDE_ERROR;
        }

        if (status & IDE_STATUS_DRQ) {
            return 0;
        }
    }
}

static void
cf_sector_in()
{
    // read and unswap bytes
    for (unsigned idx = 0; idx < SECTOR_WORDS; idx++) {
        uint16_t bytes = IDE_DATA16;
        cf_sector_buffer[idx] = (bytes >> 8) | (bytes << 8);   // optimizes to ROR.W #8
    }
}

void *
cf_read(uint32_t lba)
{
    switch (cf_command(IDE_CMD_READ_SECTORS, lba, 1)) {
    case 0:
        cf_sector_in();
        return &cf_sector_buffer;

    default:
        fmt("CF: READ error 0x%b\n", IDE_ERROR);

    // FALLTHROUGH
    case -1:
        return NULL;
    }
}

void
init_cf()
{
    // Tiny68k doesn't decode /CS1 so no device control register -> no soft reset

    cf_detected = true;

    switch (cf_command(IDE_CMD_IDENTIFY_DEVICE, 0, 0)) {
    case -1:
        putln("CF: no device");
        cf_detected = false;
        break;

    case 0:
        cf_sector_in();
        hexdump((uintptr_t)cf_sector_buffer, 0, SECTOR_SIZE, 1);
        puts("CF: ");
        char *p = (char *) & (cf_sector_buffer[27]);
        unsigned len = 40;

        while (len--) {
            putc(*p++);
        }

        putc('\n');
        break;

    default:
        fmt("CF: IDENTIFY error 0x%b\n", IDE_ERROR);
        cf_detected = false;
        break;
    }
}
