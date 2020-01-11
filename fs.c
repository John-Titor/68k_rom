#include "proto.h"
#include "pffs/pff.h"

int
fs_read(const char *name, void *buffer, size_t buffer_size)
{
    UINT    br;
    if (pf_open(name) != FR_OK) {
        return -1;
    }
    if (pf_read(buffer, buffer_size, &br) != FR_OK) {
        return -1;
    }
    return br;
}


int
init_fs()
{
    static FATFS fs;
    return (pf_mount(&fs) == FR_OK) ? 0 : -1;
}