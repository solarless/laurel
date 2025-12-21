#ifndef _SYSTICK_H
#define _SYSTICK_H

#include <stdint.h>

void systick_initialize(void);

uint32_t systick_get_counter(void);

#endif
