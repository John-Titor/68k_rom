#include "proto.h"
#include "regs.h"

#define SECTOR_SIZE     512

static uint8_t  sector_buffer[SECTOR_SIZE];

void
init_cf()
{

}

void *
cf_read(uint32_t lba)
{
    (void)lba;
    return sector_buffer;
}
