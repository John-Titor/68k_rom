#include "regs.h"
#include "proto.h"

static const char *led7_syms = "0123456789abcdefgh-?";
static const unsigned char led7_tab[] = {
    /* 0 */ LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F |         0,
    /* 1 */         0 | LED_SEG_B | LED_SEG_C |         0 |         0 |         0 |         0,
    /* 2 */ LED_SEG_A | LED_SEG_B |         0 | LED_SEG_D | LED_SEG_E |         0 | LED_SEG_G,
    /* 3 */ LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D |         0 |         0 | LED_SEG_G,
    /* 4 */         0 | LED_SEG_B | LED_SEG_C |         0 |         0 | LED_SEG_F | LED_SEG_G,
    /* 5 */ LED_SEG_A |         0 | LED_SEG_C | LED_SEG_D |         0 | LED_SEG_F | LED_SEG_G,
    /* 6 */ LED_SEG_A |         0 | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G, 
    /* 7 */ LED_SEG_A | LED_SEG_B | LED_SEG_C |         0 |         0 |         0 |         0,
    /* 8 */ LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* 9 */ LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D |         0 | LED_SEG_F | LED_SEG_G,
    /* A */ LED_SEG_A | LED_SEG_B | LED_SEG_C |         0 | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* b */         0 |         0 | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* c */         0 |         0 |         0 | LED_SEG_D | LED_SEG_E |         0 | LED_SEG_G,
    /* d */         0 | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E |         0 | LED_SEG_G,
    /* E */ LED_SEG_A |         0 |         0 | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* F */ LED_SEG_A |         0 |         0 |         0 | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* g */ LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D |         0 | LED_SEG_F | LED_SEG_G,
    /* H */         0 | LED_SEG_B | LED_SEG_C |         0 | LED_SEG_E | LED_SEG_F | LED_SEG_G,
    /* - */         0 |         0 |         0 |         0 |         0 |         0 | LED_SEG_G,
    /* ? */ LED_SEG_A | LED_SEG_B |         0 |         0 | LED_SEG_E |         0 | LED_SEG_G,
};

void
led(char c)
{
    int i;

    for (i = 0; led7_syms[i] != '\0'; i++) {
        if (led7_syms[i] == c) {
            LED_DATA_CLR = 0xff;
            LED_DATA_SET = led7_tab[i];
            break;
        }
    }
}

void
init_led(void)
{
	DUART_OPCR = 0;
	LED_DATA_CLR = 0xff;
	led('0');
}