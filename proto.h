#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);

extern void init_led();
extern void led(char c);

extern void init_cons();
extern void puts(const char *str);
extern void putln(const char *str);
extern void putc(char c);
extern int getc(void);
extern char *gets(void);
extern void putx(uint32_t value, unsigned len);
extern void putd(uint32_t value);
extern void hexdump(const void *addr, uint32_t address, unsigned length);
extern void fmt(const char *format, ...);
extern int scan(const char *buf, const char *format, ...);
extern void cons_test(void);

extern int init_disk();
extern int disk_read(void *buffer, uint32_t lba);
