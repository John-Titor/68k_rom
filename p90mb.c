/*
 * Board support for P90MB
 * 
 * No conditional includes to avoid bad dependencies.
 */
#include <stdint.h>
#include "proto.h"

#ifdef CONFIG_BOARD_p90mb

#define IDE_BASE                0xffe000UL
#define IDE_DATA16              REG16(IDE_BASE + 0x00)
#define IDE_ERROR               REG8(IDE_BASE + 0x03)
#define IDE_ERROR_ID_NOT_FOUND      0x10
#define IDE_ERROR_UNCORRECTABLE     0x40
#define IDE_FEATURE             REG8(IDE_BASE + 0x03)
#define IDE_SECTOR_COUNT        REG8(IDE_BASE + 0x05)
#define IDE_LBA_0               REG8(IDE_BASE + 0x07)
#define IDE_LBA_1               REG8(IDE_BASE + 0x09)
#define IDE_LBA_2               REG8(IDE_BASE + 0x0b)
#define IDE_LBA_3               REG8(IDE_BASE + 0x0d)
#define IDE_LBA_3_DEV1              0x10
#define IDE_LBA_3_LBA               0xe0    // incl. bits 7/5 for compat
#define IDE_STATUS              REG8(IDE_BASE + 0x0f)
#define IDE_STATUS_ERR              0x01
#define IDE_STATUS_DRQ              0x08
#define IDE_STATUS_DF               0x20
#define IDE_STATUS_DRDY             0x40
#define IDE_STATUS_BSY              0x80
#define IDE_COMMAND             REG8(IDE_BASE + 0x0f)
#define IDE_CMD_READ_SECTORS        0x20
#define IDE_CMD_WRITE_SECTORS       0x30
#define IDE_CMD_IDENTIFY_DEVICE     0xec

/**********************************************************************/

#define RX_BUF_SIZE     128
static char board_rx_buf[RX_BUF_SIZE];
static uint16_t board_rx_head;
static uint16_t board_rx_tail;

const char *board_name = "P90MB";

/**********************************************************************
 * board_init
 */
void
board_init()
{
    /* Configure UART */

    board_status(2);
}

/**********************************************************************
 * board_deinit
 */
void
board_deinit()
{
    // XXX could wait to drain TXA?

    interrupt_disable();

    // mask interrupts / reset peripherals
}

/**********************************************************************
 * board_status
 */

uint8_t
board_status(uint8_t code)
{
    static uint8_t prev_code;
    uint8_t ret_code = prev_code;

    /* no board status indicator */

    prev_code = code;
    return ret_code;
}

/**********************************************************************
 * board_putc
 */
void
board_putc(char c)
{
    if (c == '\n') {
        board_putc('\r');
    }

    for (;;) {
        if (1/* ready to transmit */) {
            /* transmit c */
            break;
        }
    }
    board_status(3);
}

/**********************************************************************
 * board_getc
 */
int
board_getc()
{
    if (board_rx_head != board_rx_tail) {
        char c = board_rx_buf[board_rx_tail % RX_BUF_SIZE];
        board_rx_tail++;
        return c;
    }
    return -1;
}

/**********************************************************************
 * board_diskread
 */

int
board_diskread(void *buffer, uint32_t lba)
{
    // XXX should check for busy here?

    IDE_LBA_3 = ((lba >> 24) & 0x3f) | IDE_LBA_3_LBA;
    IDE_LBA_2 = (lba >> 16) & 0xff;
    IDE_LBA_1 = (lba >> 8) & 0xff;
    IDE_LBA_0 = lba & 0xff;
    IDE_SECTOR_COUNT = 1;
    IDE_COMMAND = IDE_CMD_READ_SECTORS;

    // XXX timeout
    for (;;) {
        uint8_t status = IDE_STATUS;

        if (status & IDE_STATUS_ERR) {
            fmt("CF: READ error 0x%b\n", IDE_ERROR);
            return -1;
        }

        if (status & IDE_STATUS_DRQ) {
            uint16_t *bp = (uint16_t *)buffer;

            for (unsigned idx = 0; idx < DISK_SECTOR_SIZE; idx += 2) {
                *bp++ = swap16(IDE_DATA16);
            }

            return 0;
        }
    }

}

/**********************************************************************
 * interrupt handlers
 */

#endif /* CONFIG_BOARD_p90mb */
