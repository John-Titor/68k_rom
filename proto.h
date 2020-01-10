/*
 * Function prototypes
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lib.h"

/**
 * @brief      Initializes the led.
 */
extern void init_led();

/**
 * @brief      update the 7-segment LED display
 *
 * @param[in]  c     character to display (0-9, a-h, -, ?)
 */
extern void led(char c);

/**
 * @brief      Initializes the console.
 */
extern void init_cons();

/**
 * @brief      print a string to the console
 *
 * @param[in]  str   The string to print
 */
extern void puts(const char *str);

/**
 * @brief      print a string and newline to the console
 *
 * @param[in]  str   The string to print
 */
extern void putln(const char *str);

/**
 * @brief      print a character to the console
 *
 * @param[in]  c     The character to print. Carriage return (\r)
 *                   will be automatically inserted before newline (\n).
 */
extern void putc(char c);

/**
 * @brief      get a character from the console
 *
 * @return     the character read, or -1 if no character is ready
 */
extern int getc(void);

/**
 * @brief      get a line of text from the console
 *
 * @return     pointer to a static buffer containing the 
 *             line that was read, or NULL if ^C was read.
 *             The buffer is guaranteed to be nul-terminated
 *             and input larger than the buffer is discarded.
 */
extern char *gets(void);

/**
 * @brief      print a hexadecimal value
 *
 * @param[in]  value  the value to print
 * @param[in]  len    the number of bytes from the value to print
 */
extern void putx(uint32_t value, unsigned len);

/**
 * @brief      print a decimal value
 *
 * @param[in]  value  the value to print
 */
extern void putd(uint32_t value);

/**
 * @brief      dump a range of memory in canonical hexdump format
 *
 * @param[in]  addr     memory address to start dumping
 * @param[in]  address  starting address to be displayed alongside the dump
 * @param[in]  length   number of bytes to dump
 */
extern void hexdump(const void *addr, uintptr_t address, unsigned length);

/**
 * @brief      printf-style output formatter
 * 
 * Supports:
 *  %c      character (char)
 *  %d      signed decimal integer (int)
 *  %u      unsigned decimal integer (unsigned int)
 *  %p      pointer (32-bit hex) (const void *)
 *  %b      hex byte (uint8_t)
 *  %w      hex word (uint16_t)
 *  %l      hex long (uint32_t)
 *  %s      string (const char *)
 *
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 */
extern void fmt(const char *format, ...);

/**
 * @brief      scanf-style input scanner
 *
 * Supports:
 *  %c      character (char *)
 *  %w      unsigned word, decimal or hex (unsigned short *)
 *  %l      unsigned long, decimal or hex (unsigned long *)
 *  %s      string, needs 2 args, pointer & max length (char *, unsigned short)
 *
 * @param[in]  buf        buffer to scan
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 *
 * @return     the number of arguments converted, or -1 on error
 */
extern int scan(const char *buf, const char *format, ...);

/**
 * @brief      console unit tests
 */
extern void cons_test(void);

/**
 * @brief      Initializes the disk subsystem
 *
 * @return     Zero on success, -1 if no disk found.
 */
extern int init_disk();
