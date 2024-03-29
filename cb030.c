/*
 * Board support for CB030
 * 
 * No conditional includes to avoid bad dependencies.
 */
#include <stdint.h>
#include "proto.h"

#ifdef CONFIG_BOARD_cb030
const char *board_name = "CB030";

#define IDE_BASE                0xffffe000UL
#if defined(WITH_8BIT_COMPACT_FLASH)
# define IDE_DATA8               REG8(IDE_BASE + 0x00)
# define IDE_ERROR               REG8(IDE_BASE + 0x01)
# define IDE_FEATURE             REG8(IDE_BASE + 0x01)
# define IDE_SECTOR_COUNT        REG8(IDE_BASE + 0x02)
# define IDE_LBA_0               REG8(IDE_BASE + 0x03)
# define IDE_LBA_1               REG8(IDE_BASE + 0x04)
# define IDE_LBA_2               REG8(IDE_BASE + 0x05)
# define IDE_LBA_3               REG8(IDE_BASE + 0x06)
# define IDE_STATUS              REG8(IDE_BASE + 0x07)
# define IDE_COMMAND             REG8(IDE_BASE + 0x07)
#else /* !WITH_8BIT_COMPACT_FLASH */
# define IDE_DATA16              REG16(IDE_BASE + 0x00)
# define IDE_ERROR               REG8(IDE_BASE + 0x03)
# define IDE_FEATURE             REG8(IDE_BASE + 0x03)
# define IDE_SECTOR_COUNT        REG8(IDE_BASE + 0x05)
# define IDE_LBA_0               REG8(IDE_BASE + 0x07)
# define IDE_LBA_1               REG8(IDE_BASE + 0x09)
# define IDE_LBA_2               REG8(IDE_BASE + 0x0b)
# define IDE_LBA_3               REG8(IDE_BASE + 0x0d)
# define IDE_STATUS              REG8(IDE_BASE + 0x0f)
# define IDE_COMMAND             REG8(IDE_BASE + 0x0f)
#endif /* WITH_8BIT_COMPACT_FLASH */
#define IDE_ERROR_ID_NOT_FOUND      0x10
#define IDE_ERROR_UNCORRECTABLE     0x40
#define IDE_LBA_3_DEV1              0x10
#define IDE_LBA_3_LBA               0xe0    // incl. bits 7/5 for compat
#define IDE_STATUS_ERR              0x01
#define IDE_STATUS_DRQ              0x08
#define IDE_STATUS_DF               0x20
#define IDE_STATUS_DRDY             0x40
#define IDE_STATUS_BSY              0x80
#define IDE_CMD_READ_SECTORS        0x20
#define IDE_CMD_WRITE_SECTORS       0x30
#define IDE_CMD_IDENTIFY_DEVICE     0xec

#define DUART_BASE              0xfffff000UL
#define DUART_MRA               REG8(DUART_BASE + 0x00)
#define DUART_MRB               REG8(DUART_BASE + 0x10)
#define DUART_MR1_8BIT              0x03
#define DUART_MR1_NO_PARITY         0x10
#define DUART_MR1_RTS               0x80
#define DUART_MR2_1STOP             0x07
#define DUART_MR2_CTS_ENABLE_TX     0x10
#define DUART_SRA               REG8(DUART_BASE + 0x02)
#define DUART_SRB               REG8(DUART_BASE + 0x12)
#define DUART_SR_RECEIVED_BREAK     0x80
#define DUART_SR_FRAMING_ERROR      0x40
#define DUART_SR_PARITY_ERROR       0x20
#define DUART_SR_OVERRUN_ERROR      0x10
#define DUART_SR_TRANSMITTER_EMPTY  0x08
#define DUART_SR_TRANSMITTER_READY  0x04
#define DUART_SR_FIFO_FULL          0x02
#define DUART_SR_RECEIVER_READY     0x01
#define DUART_CSRA              REG8(DUART_BASE + 0x02)
#define DUART_CSRB              REG8(DUART_BASE + 0x12)
#define DUART_CSR_38400B            0xcc
#define DUART_CRA               REG8(DUART_BASE + 0x04)
#define DUART_CRB               REG8(DUART_BASE + 0x14)
#define DUART_CR_BRKSTOP            0x70
#define DUART_CR_BRKSTART           0x60
#define DUART_CR_BRKRST             0x50
#define DUART_CR_ERRST              0x40
#define DUART_CR_TXRST              0x30
#define DUART_CR_RXRST              0x20
#define DUART_CR_MRRST              0x10
#define DUART_CR_TXDIS              0x08
#define DUART_CR_TXEN               0x04
#define DUART_CR_RXDIS              0x02
#define DUART_CR_RXEN               0x01
#define DUART_RBA               REG8(DUART_BASE + 0x06)
#define DUART_RBB               REG8(DUART_BASE + 0x16)
#define DUART_TBA               REG8(DUART_BASE + 0x06)
#define DUART_TBB               REG8(DUART_BASE + 0x16)
#define DUART_IPCR              REG8(DUART_BASE + 0x08)
#define DUART_ACR               REG8(DUART_BASE + 0x08)
#define DUART_ACR_MODE_CTR_XTAL16   0x30
#define DUART_ACR_MODE_TMR_XTAL     0x60
#define DUART_ACR_MODE_TMR_XTAL16   0x70
#define DUART_ISR               REG8(DUART_BASE + 0x0a)
#define DUART_IMR               REG8(DUART_BASE + 0x0a)
#define DUART_INT_TXRDY_A           0x01
#define DUART_INT_RXRDY_A           0x02
#define DUART_INT_CTR               0x08
#define DUART_INT_TXRDY_B           0x10
#define DUART_INT_RXRDY_B           0x20
#define DUART_CUR               REG8(DUART_BASE + 0x0c)
#define DUART_CTUR              REG8(DUART_BASE + 0x0c)
#define DUART_CLR               REG8(DUART_BASE + 0x0e)
#define DUART_CTLR              REG8(DUART_BASE + 0x0e)
#define DUART_IVR               REG8(DUART_BASE + 0x18)
#define DUART_IPR               REG8(DUART_BASE + 0x1a)
#define DUART_OPCR              REG8(DUART_BASE + 0x1a)
#define DUART_STARTCC           REG8(DUART_BASE + 0x1c)
#define DUART_OPRSET            REG8(DUART_BASE + 0x1c)
#define DUART_STOPCC            REG8(DUART_BASE + 0x1e)
#define DUART_OPRCLR            REG8(DUART_BASE + 0x1e)

/**********************************************************************/

#define RX_BUF_SIZE     128
static char board_rx_buf[RX_BUF_SIZE];
static uint16_t board_rx_head;
static uint16_t board_rx_tail;

/**********************************************************************
 * board_init
 */
void
board_init()
{
    /* configure for driving LEDs */
    DUART_OPCR = 0;
    board_status(1);

    /* initialise console UART */
    DUART_MRA = DUART_MR1_8BIT | DUART_MR1_NO_PARITY | DUART_MR1_RTS;
    DUART_MRA = DUART_MR2_CTS_ENABLE_TX | DUART_MR2_1STOP;
    DUART_IVR = 64;
    DUART_ACR = DUART_ACR_MODE_TMR_XTAL16;
    DUART_CTLR = 0x80;
    DUART_CTUR = 0x4;
    DUART_CSRA = DUART_CSR_38400B;
    DUART_CRA = DUART_CR_TXEN | DUART_CR_RXEN;

    // clear any pending interrupt
    (void)DUART_STOPCC;

    // interrupts enabled
    DUART_IMR = DUART_INT_RXRDY_A | DUART_INT_CTR;

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

    // mask interrupts
    DUART_IMR = 0;
}

/**********************************************************************
 * board_status
 */

uint8_t
board_status(uint8_t code)
{
    static uint8_t prev_code;
    uint8_t ret_code = prev_code;
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
        board_status(5);
        if (DUART_SRA & DUART_SR_TRANSMITTER_READY) {
            DUART_TBA = c;
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
#if defined(IDE_DATA16)
            uint16_t *bp = (uint16_t *)buffer;

            for (unsigned idx = 0; idx < DISK_SECTOR_SIZE; idx += 2) {
                *bp++ = swap16(IDE_DATA16);
            }
#else /* !IDE_DATA16 */
            uint8_t *bp = (uint8_t *)buffer;

            for (unsigned idx = 0; idx < DISK_SECTOR_SIZE; idx += 1) {
                *bp++ = IDE_DATA8;
            }
#endif

            return 0;
        }
    }

}

/**********************************************************************
 * interrupt handlers
 */
INTERRUPT_HANDLER
void
vector_64(void)
{
    uint8_t prev_code = board_status(9);
    uint8_t stat = DUART_ISR;

    if (stat & DUART_INT_RXRDY_A) {
        char c = DUART_RBA;
        if ((board_rx_head - board_rx_tail) < RX_BUF_SIZE) {
            board_rx_buf[board_rx_head % RX_BUF_SIZE] = c;
            board_rx_head++;
        }
    }

    if (stat & DUART_INT_CTR) {
        (void)DUART_STOPCC;
        timer_tick();
    }
    board_status(prev_code);
}

#endif /* CONFIG_BOARD_cb030 */
