extern void init_led();
extern void led(char c);

extern void init_cons();
extern void puts(const char *str);
extern void putln(const char *str);
extern void putc(char c);
extern int getc(void);
extern void putx(uint32_t value, unsigned len);
extern void putd(uint32_t value);
extern void hexdump(const void *addr, uint32_t address, unsigned length);
extern void fmt(const char *str, ...);
extern void cons_test(void);

extern int init_disk();
extern int disk_read(void *buffer, uint32_t lba);
