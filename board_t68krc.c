/*
 * Board support for T68KRC
 * 
 * No conditional includes to avoid bad dependencies.
 * 
 * Very similar to Tiny68k; 12MHz vs 8MHz, 2M vs 16M RAM.
 */

#ifdef CONFIG_BOARD_t68krc
const char *board_name = "T68KRC";
# define CONFIG_BOARD_t68k_common
# include "board_tiny68k.c"

#endif /* CONFIG_BOARD_T68KRC */
