/*
 * Mini C library
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern void *memset(void *b, int c, size_t len);
extern void *memcpy(void *restrict dst, const void *restrict src, size_t len);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern void putc(char c);
extern int getc();

static inline uint16_t swap16(uint16_t val)
{
    __asm__ volatile("rolw #8,%0" : "=d"(val) : "0"(val));
    return val;
}

static inline uint32_t swap32(uint32_t val)
{
    __asm__ volatile("rolw #8,%0; swap %0; rolw #8,%0" : "=d"(val) : "0"(val));
    return val;
}

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
 * @brief      get a line of text from the console
 *
 * @return     pointer to a static buffer containing the
 *             line that was read, or NULL if ^C was read.
 *             The buffer is guaranteed to be nul-terminated
 *             and input larger than the buffer is discarded.
 */
extern char *gets();

/**
 * @brief      print a hexadecimal value
 *
 * @param[in]  value  the value to print
 * @param[in]  len    the number of bytes from the value to print
 */
extern void putx(uint32_t value, size_t len);

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
 * @param[in]  width    'b', 'w' or 'l' to select byte, word or long hex display
 *
 * @return     number of bytes printed (may be rounded down)
 */
extern size_t hexdump(uintptr_t addr, uintptr_t address, size_t length, char width);

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
 *  %l      unsigned number, decimal or hex with preceding 0x (uint32_t *)
 *  %s      string, needs 2 args, pointer & max length (char *, size_t)
 *
 * @param[in]  buf        buffer to scan
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 *
 * @return     the number of arguments converted, or -1 on error
 */
extern int scan(const char *buf, const char *format, ...);
