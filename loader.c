/*
 * Backend for upload handlers.
 *
 * Manages avoiding ovewriting things that shouldn't be overwritten.
 */

#include "proto.h"
typedef __attribute__((noreturn)) void (*entry_function)(void);

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

void
loader_go()
{
    if (loader_entrypoint != ~(uintptr_t)0) {
        board_status(8);
        board_deinit();
        memcpy(0, vector_savearea, vector_savearea_size);

        ((entry_function)loader_entrypoint)();
    }
}
