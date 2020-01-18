/*
 * Mini C library
 */
#pragma once

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
extern void puts(const char *str);
extern void putln(const char *str);
extern char *gets();
extern size_t hexdump(uintptr_t addr, uintptr_t address, size_t length, char width);
extern void fmt(const char *format, ...);
extern int scan(const char *buf, const char *format, ...);

static inline char toupper(char c)
{
    return ((c >= 'a') && (c <= 'z')) ? c + ('A' - 'a') : c;
}

static inline bool isspace(char c)
{
    return (c == ' ') || (c == '\t');
}

static inline bool isdigit(char c)
{
	return (c >= '0') && (c <= '9');
}

static inline bool isxdigit(char c)
{
	return isdigit(c) || (((c | 0x20) >= 'a') && ((c | 0x20) <= 'z'));
}

static inline int xdigit(char c)
{
	return isdigit(c) ? c - '0' : isxdigit(c) ? (c | 0x20) - 'a' + 10: -1;
}

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
