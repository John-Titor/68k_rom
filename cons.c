/*
 * Minimal 68681 console driver.
 */

#include "proto.h"
#include "regs.h"

void
init_cons()
{
    DUART_CRA = DUART_CR_MRRST;
    DUART_MRA = DUART_MR1_8BIT | DUART_MR1_NO_PARITY | DUART_MR1_RTS;
    DUART_MRA = DUART_MR2_1STOP;
    DUART_CSRA = DUART_CSR_9600B;
    DUART_CRA = DUART_CR_TXRST | DUART_CR_TXEN;
    DUART_CRA = DUART_CR_RXRST | DUART_CR_RXEN;
}

void
cons_putc(char c)
{
    if (c == '\n') {
        cons_putc('\r');
    }

    for (;;) {
        if (DUART_SRA & DUART_SR_TRANSMITTER_READY) {
            DUART_TBA = c;
            break;
        }
    }
}

int
cons_getc()
{
    return (DUART_SRA & DUART_SR_RECEIVER_READY) ? DUART_RBA : -1;
}
