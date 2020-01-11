#include "proto.h"
#include "regs.h"

#define SECTOR_SIZE     512
#define SECTOR_WORDS    (SECTOR_SIZE / 2)

static uint16_t  sector_buffer[SECTOR_WORDS];

void
init_cf()
{
    // set SRST
    // observe BSY
    // clear SRST
    // observe !BSY (31s)
    // observe DRDY (30s)
}

static void
cf_setlba(uint32_t lba)
{
    IDE_LBA_3 = ((lba >> 24) & 0x3f) | IDE_LBA_3_LBA;
    IDE_LBA_2 = (lba >> 16) & 0xff;
    IDE_LBA_1 = (lba >> 8) & 0xff;
    IDE_LBA_0 = lba & 0xff;
}

void *
cf_read(uint32_t lba)
{
    // technically we should check DRDY before starting here...

    if (lba > IDE_MAXLBA) {
        return NULL;
    }

    cf_setlba(lba);

    // read a sector
    IDE_SECTOR_COUNT = 1;
    IDE_COMMAND = IDE_CMD_READ_SECTORS;

    // wait for device ready
    for (;;) {
        // could use alt status here (but why?)
        uint8_t status = IDE_STATUS;

        // oops, an error
        if (status & IDE_STATUS_ERR) {
            fmt("CF: read error LBA 0x%l: 0x%b\n", lba, IDE_ERROR);
            return NULL;
        }

        // data be ready
        if (status & IDE_STATUS_DRQ) {
            break;
        }
    }

    // read and unswap bytes
    for (unsigned idx = 0; idx < SECTOR_WORDS; idx++) {
        uint16_t bytes = IDE_DATA16;
        sector_buffer[idx] = (bytes >> 8) | (bytes << 8);   // optimizes to ROR.W #8
    }

    return sector_buffer;
}
