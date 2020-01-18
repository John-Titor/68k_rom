/*
 * Board interface
 */
#pragma once

#include <stdint.h>

#define DISK_SECTOR_SIZE     512

extern void board_init();
extern void board_deinit();
extern void board_status(uint8_t code);
extern void board_putc(char c);
extern int board_getc();
extern int board_diskread(void *buffer, uint32_t lba);
