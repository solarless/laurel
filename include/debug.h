#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdint.h>
#include <stdio.h>

#define _STR(x) #x
#define STR(x) _STR(x)

#define printd(fmt, ...) (void)printf("[" __FILE__ ":" STR(__LINE__) "] " fmt, ##__VA_ARGS__)

void hexdump(uint8_t *buffer, uint8_t size);

#endif
