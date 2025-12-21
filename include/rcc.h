#ifndef _RCC_H
#define _RCC_H

#include <stdint.h>

void rcc_initialize(void);

uint32_t rcc_get_sysclk(void);

#endif
