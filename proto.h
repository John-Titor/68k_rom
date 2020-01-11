/*
 * Function prototypes
 */

#include "lib.h"

extern void init_led();
extern void led(char c);

extern void init_cons();
extern void cons_putc(char c);
extern int cons_getc();

#define CF_SECTOR_SIZE     512
extern int init_cf();
extern uint32_t cf_size();
extern void *cf_read(uint32_t lba);

extern int init_fs();
extern int fs_read(const char *name, void *buffer, size_t buffer_size);

typedef int (*cmd_handler_fn)(const char *input_buffer);
#define COMMAND(handler_function)                               \
    static int handler_function(const char *input_buffer);      \
    static const                                                \
    __attribute__((section("COMMANDS")))                        \
    __attribute__((used))                                       \
    cmd_handler_fn cmdptr_ ## handler_function = handler_function;
extern cmd_handler_fn   __commands, __commands_end;
