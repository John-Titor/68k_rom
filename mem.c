#include "proto.h"

COMMAND(cmd_memdump);

static int
cmd_memdump(const char *input_buffer)
{
    static uint32_t address = 0;
    static uint32_t count = 512;
    char width;

    if (input_buffer == NULL) {
        putln("md.[bwl] [<address> [<count>]]    display memory");

    } else if (scan(input_buffer, "md.%c %l %w", &width, &address, &count) >= 1) {
        address += hexdump(address, address, count, width);
        return 0;
    }

    return -1;
}
