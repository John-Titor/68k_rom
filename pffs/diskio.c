/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "../board.h"
#include "../lib.h"


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	static uint8_t sector_buffer[DISK_SECTOR_SIZE];
	static DWORD cached_sector = ~0;

	if (sector != cached_sector) {
		if (board_diskread(sector_buffer, sector) < 0) {
			return RES_ERROR;
		}
		cached_sector = sector;
	}

	memcpy(buff, sector_buffer + offset, count);
	return RES_OK;
}
