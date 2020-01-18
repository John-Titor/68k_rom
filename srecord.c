/*
 * S-record handler
 */

#include "proto.h"

typedef __attribute__((noreturn)) void (*entry_function)(void);

#define MAX_SRECORD_SIZE    80

static uint32_t srecord_entrypoint = ~(uint32_t)0;

extern uint8_t _btext;
extern uint8_t _vector_top;
extern uint8_t _vector_savearea;
static uint8_t *vector_savearea = &_vector_savearea;

static int
srecord_onehex(const char *bp)
{
    int nh = xdigit(*bp);
    int nl = xdigit(*(bp + 1));
    if ((nh < 0) || (nl < 0)) {
        fmt("invalid hex %c%c", *bp, *(bp + 1));
        return -1;
    }
    return (nh << 4) + nl;
}

static int
srecord_process(const char *input_buffer)
{
    uint8_t buf[MAX_SRECORD_SIZE] __attribute__((unused));

    board_status(8);

    srecord_entrypoint = ~(uint32_t)0;
    memcpy(vector_savearea, 0, sizeof(vector_savearea));

    for (;;) {
        // basic sanity on the input string
        int linelen = strlen(input_buffer);
        if ((linelen < 10)
            || (linelen % 2)
            || (input_buffer[0] != 'S')
            || !isdigit(input_buffer[1])
            || (input_buffer[1] == '4')) {

            putln("bad record length / type");
            return -1;
        }

        // get the length byte and validate
        int count = srecord_onehex(input_buffer + 2);
        if (count < 0) {
            return -1;
        }
        if (((count * 2) + 4) > linelen) {
            putln("record truncated");
            return -1;
        }
        if (count > MAX_SRECORD_SIZE) {
            putln("record too long");
            return -1;
        }

        // get the expected checksum
        int expected_sum = srecord_onehex(input_buffer + 4 + 2 * (count - 1));
        if (expected_sum < 0) {
            return -1;
        }

        // convert the rest of the input string into hex in the buffer
        int measured_sum = count;
        for (int index = 0; index < (count - 1); index++) {
            int val = srecord_onehex(input_buffer + 4 + 2 * index);
            if (val < 0) {
                return -1;
            }
            measured_sum += val;
            buf[index] = val;
        }

        // validate the checksum
        measured_sum = ~measured_sum & 0xff;
        if (measured_sum != expected_sum) {
            fmt("sum 0x%b not 0x%b\n", measured_sum, expected_sum);
            return -1;
        }

//        uint8_t type = input_buffer[1] - '0';
/*
        // fetch an address
        uint32_t address = 0;
        unsigned parse_index = 1;
        switch (type) {
        case 3: // 32
        case 7:
        case 2: // 24
        case 8:
        case 1: // 16
        case 9:
*/


        input_buffer = gets();
        if (input_buffer == NULL) {
            putln("cancelled");
            return 0;
        }
    }
}


COMMAND(srecord);

static int
srecord(const char *input_buffer)
{
    if (input_buffer == NULL) {
        putln("<S-records>                       Upload S-records to load a new program");
        putln("go                                Start a previously uploaded program");
    } else if ((strlen(input_buffer) > 12)
               && (input_buffer[0] == 'S') 
               && isdigit(input_buffer[1])) {
        // might be s-records
        return srecord_process(input_buffer);
        //return srecord_loop(input_buffer);

    } else if (!strcmp(input_buffer, "go")) {
        if (srecord_entrypoint != ~(uint32_t)0) {
            board_status(9);
            board_deinit();
            memcpy(0, vector_savearea, sizeof(vector_savearea));

            ((entry_function)srecord_entrypoint)();
        }
    }
    return -1;
}
