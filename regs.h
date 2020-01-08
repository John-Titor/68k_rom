/*
 * Register layout for Tiny68K
 */
#include <stdint.h>

#define REG8(_adr)      (*(uint8_t *)(_adr))
#define REG16(_adr)     (*(uint16_t *)(_adr))

#define I2C_BASE                0xffd000UL
#define I2C_REG                 REG8(I2C_BASE)
#define I2C_SDA                 (1U << 0)
#define I2C_SCL                 (1U << 1)
#define I2C_EN_RTC              (1U << 6)
#define I2C_EN                  (1U << 7)

#define IDE_BASE                0xffe000UL
#define IDE_DATA16              REG16(IDE_BASE + 0x00)
#define IDE_DATA8               REG8(IDE_BASE + 0x01)
#define IDE_ERROR               REG8(IDE_BASE + 0x03)
#define IDE_ERROR_ID_NOT_FOUND      0x10
#define IDE_ERROR_UNCORRECTABLE     0x40
#define IDE_FEATURE             REG8(IDE_BASE + 0x03)
#define IDE_SECTOR_COUNT        REG8(IDE_BASE + 0x05)
#define IDE_SECTOR_NUMBER       REG8(IDE_BASE + 0x07)
#define IDE_CYLINDER_LOW        REG8(IDE_BASE + 0x09)
#define IDE_CYLINDER_HIGH       REG8(IDE_BASE + 0x0b)
#define IDE_DRIVE_HEAD          REG8(IDE_BASE + 0x0d)
#define IDE_STATUS              REG8(IDE_BASE + 0x0f)
#define IDE_STATUS_ERR              0x01
#define IDE_STATUS_DRQ              0x08
#define IDE_STATUS_DF               0x20
#define IDE_STATUS_DRDY             0x40
#define IDE_STATUS_BSY              0x80
#define IDE_COMMAND             REG8(IDE_BASE + 0x0f)
#define IDE_CMD_READ_SECTORS        0x20
#define IDE_CMD_WRITE_SECTORS       0x30

#define DUART_BASE              0xfff000UL
#define DUART_MRA               REG8(DUART_BASE + 0x01)
#define DUART_MRB               REG8(DUART_BASE + 0x11)
#define DUART_MR1_8BIT              0x03
#define DUART_MR1_NO_PARITY         0x10
#define DUART_MR1_RTS               0x80
#define DUART_MR2_1STOP             0x07
#define DUART_SRA               REG8(DUART_BASE + 0x03)
#define DUART_SRB               REG8(DUART_BASE + 0x13)
#define DUART_SR_RECEIVED_BREAK     0x80
#define DUART_SR_FRAMING_ERROR      0x40
#define DUART_SR_PARITY_ERROR       0x20
#define DUART_SR_OVERRUN_ERROR      0x10
#define DUART_SR_TRANSMITTER_EMPTY  0x08
#define DUART_SR_TRANSMITTER_READY  0x04
#define DUART_SR_FIFO_FULL          0x02
#define DUART_SR_RECEIVER_READY     0x01
#define DUART_CSRA              REG8(DUART_BASE + 0x03)
#define DUART_CSRB              REG8(DUART_BASE + 0x13)
#define DUART_CSR_9600B             0xaa
#define DUART_CRA               REG8(DUART_BASE + 0x05)
#define DUART_CRB               REG8(DUART_BASE + 0x15)
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
#define DUART_RBA               REG8(DUART_BASE + 0x07)
#define DUART_RBB               REG8(DUART_BASE + 0x17)
#define DUART_TBA               REG8(DUART_BASE + 0x07)
#define DUART_TBB               REG8(DUART_BASE + 0x17)
#define DUART_IPCR              REG8(DUART_BASE + 0x09)
#define DUART_ACR               REG8(DUART_BASE + 0x09)
#define DUART_ACR_MODE_CTR_XTAL16   0x30
#define DUART_ACR_MODE_TMR_XTAL     0x60
#define DUART_ACR_MODE_TMR_XTAL16   0x70
#define DUART_ISR               REG8(DUART_BASE + 0x0b)
#define DUART_IMR               REG8(DUART_BASE + 0x0b)
#define DUART_INT_TXRDY_A           0x01
#define DUART_INT_RXRDY_A           0x02
#define DUART_INT_TXRDY_B           0x10
#define DUART_INT_RXRDY_B           0x20
#define DUART_CUR               REG8(DUART_BASE + 0x0d)
#define DUART_CTUR              REG8(DUART_BASE + 0x0d)
#define DUART_CLR               REG8(DUART_BASE + 0x0f)
#define DUART_CTLR              REG8(DUART_BASE + 0x0f)
#define DUART_IVR               REG8(DUART_BASE + 0x19)
#define DUART_IPR               REG8(DUART_BASE + 0x1b)
#define DUART_OPCR              REG8(DUART_BASE + 0x1b)
#define DUART_OPCR_7SEG             0x00
#define DUART_STARTCC           REG8(DUART_BASE + 0x1d)
#define DUART_OPRSET            REG8(DUART_BASE + 0x1d)
#define DUART_STOPCC            REG8(DUART_BASE + 0x1f)
#define DUART_OPRCLR            REG8(DUART_BASE + 0x1f)

#define LED_DATA_SET            DUART_OPRSET
#define LED_DATA_CLR            DUART_OPRCLR
// segments are connected LSB->A, MSB->G
//
//  Layout:    _A_
//            |   |
//            F   B
//            |_G_|
//            |   |
//            E   C
//            |_D_|
//   
#define LED_SEG_A                   (1U << 1)
#define LED_SEG_B                   (1U << 2)
#define LED_SEG_C                   (1U << 3)
#define LED_SEG_D                   (1U << 4)
#define LED_SEG_E                   (1U << 5)
#define LED_SEG_F                   (1U << 6)
#define LED_SEG_G                   (1U << 7)
#define LED_DATA(_x)                ((_x) << 1)
