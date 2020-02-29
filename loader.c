/*
 * Backend for upload handlers.
 *
 * Manages avoiding ovewriting things that shouldn't be overwritten.
 */

#include "proto.h"

static uintptr_t loader_entrypoint = ~(uintptr_t)0;

extern uint8_t _btext;
extern uint8_t _vector_top;
extern uint8_t _vector_savearea;

static uint8_t *vector_savearea = &_vector_savearea;
static uint32_t vector_savearea_size = (uintptr_t)&_vector_top;


void
init_loader()
{
    loader_entrypoint = ~(uintptr_t)0;
    memcpy(vector_savearea, 0, vector_savearea_size);
}

int
loader_load_bytes(uint32_t address, uint8_t *bytes, size_t count)
{
    if (address < vector_savearea_size) {
        size_t savecount = ((address + count) > vector_savearea_size) ? (vector_savearea_size - address) : count;

        memcpy(vector_savearea + address, bytes, savecount);
        count -= savecount;
        address += savecount;
        bytes += savecount;
    }

    memcpy((uint8_t *)address, bytes, count);

    /* XXX could check for out-of-bounds */
    return 0;
}

int
loader_set_entry(uint32_t address)
{
    loader_entrypoint = address;

    /* XXX could check for out-of-bounds */
    return 0;
}

extern void _loader_go(uint32_t d0, uint32_t a0, uintptr_t entry) __attribute__((noreturn));
__asm__ (
"_loader_go:                                            \n"
"   addql   #4, %sp                                     \n"
"   moveml  %sp@+, %d0/%a0                              \n"
"   rts                                                 \n"
);

void
loader_go(uint32_t d0, uint32_t a0)
{
    if (loader_entrypoint != ~(uintptr_t)0) {
        board_status(8);
        board_deinit();
        memcpy(0, vector_savearea, vector_savearea_size);

        _loader_go(d0, a0, loader_entrypoint);
    }
}
