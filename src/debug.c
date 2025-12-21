#include <stdint.h>
#include <stdio.h>

#include "debug.h"

static uint8_t min(uint8_t a, uint8_t b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

void hexdump(uint8_t *buffer, uint8_t size)
{
    uint8_t offset = 0;
    int8_t left = size;
    while (left > 0) {
        uint8_t line_length = min(left, 8);

        printf("    ");
        for (uint8_t i = 0; i < line_length; i++) {
            printf("0x%02x", buffer[offset + i]);
            if (i != 7) {
                printf(" ");
            }
        }
        printf("\r\n");

        left -= line_length;
        offset += 8;
    }
}
