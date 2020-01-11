#include "proto.h"
#include "regs.h"

#define SECTOR_WORDS    (CF_SECTOR_SIZE / 2)

static uint16_t  cf_sector_buffer[SECTOR_WORDS];
static uint32_t cf_total_sectors = 0;

static int
cf_command(uint8_t command, uint32_t lba, uint8_t count)
{
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
        cf_sector_buffer[idx] = swap16(IDE_DATA16);
    }
}

void *
cf_read(uint32_t lba)
{
    if (lba >= cf_total_sectors) {
        return NULL;
    }

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

int
init_cf()
{
    // Tiny68k doesn't decode /CS1 so no device control register -> no soft reset

    switch (cf_command(IDE_CMD_IDENTIFY_DEVICE, 0, 0)) {
    case 0:
        cf_sector_in();
        puts("CF: ");
        char *p = (char *) & (cf_sector_buffer[27]);
        unsigned len = 40;

        while (len--) {
            putc(*p++);
        }

        cf_total_sectors = cf_sector_buffer[60] + (cf_sector_buffer[61] << 16);
        fmt(" %u sectors\n", cf_total_sectors);
        return 0;

    case -1:
        putln("CF: no device");
        return -1;

    default:
        fmt("CF: IDENTIFY error 0x%b\n", IDE_ERROR);
        return -1;
    }
}
