#include "regs.h"
#include "proto.h"


void
puts(const char *str)
{
    while (*str != '\0')
        putc(*str++);
    putc('\n');
}

void
putc(char c)
{
    if (c == '\n')
        putc('\r');
    for (;;) {
        if (DUART_SRA & DUART_SR_TRANSMITTER_READY) {
            DUART_TBA = c;
            break;
        }
    }
}

int
getc(void)
{
    if (DUART_SRA & DUART_SR_RECEIVER_READY) {
        return DUART_RBA;
    }
    return -1;
}

void
init_cons(void)
{
	DUART_CRA = DUART_CR_MRRST;
	DUART_MRA = DUART_MR1_8BIT | DUART_MR1_NO_PARITY | DUART_MR1_RTS;
	DUART_MRA = DUART_MR2_1STOP;
	DUART_CSRA = DUART_CSR_9600B;
	DUART_CRA = DUART_CR_TXRST | DUART_CR_TXEN;
	DUART_CRA = DUART_CR_RXRST | DUART_CR_RXEN;
}