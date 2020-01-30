/*
 * Function prototypes
 */
#pragma once

#include "lib.h"
#include "board.h"

extern void init_emulator();
extern void emulator_puts(const char *str);

extern uint16_t interrupt_disable();
extern void interrupt_enable();
extern void interrupt_restore(uint16_t state);

extern void timer_tick();
extern uint32_t timer_get_ticks();

extern int init_cf();
extern uint32_t cf_size();
extern void *cf_read(uint32_t lba);

extern int init_fs();
extern size_t fs_filesize(const char *name);
extern int fs_read(const char *name, void *buffer, size_t buffer_size);

extern void init_loader();
extern int loader_load_bytes(uint32_t address, uint8_t *bytes, size_t count);
extern int loader_set_entry(uint32_t address);
extern void loader_go();

typedef int (*cmd_handler_fn)(const char *input_buffer);
#define COMMAND(handler_function)                               \
    static int handler_function(const char *input_buffer);      \
    static const                                                \
    __attribute__((section("COMMANDS")))                        \
    __attribute__((used))                                       \
    cmd_handler_fn cmdptr_ ## handler_function = handler_function;
extern cmd_handler_fn   _commands, _ecommands;

#define INTERRUPT_HANDLER                           \
    __attribute__((interrupt))                      \
    __attribute__((section("INTERRUPT_HANDLERS")))
