#pragma once

#include <stdint.h>

enum {
    LED1,
    LED2,
};

void led_blink(uint8_t ledno);

void led_process(void);
