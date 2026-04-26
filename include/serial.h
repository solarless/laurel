#pragma once

#include <stdint.h>

void serial_send(uint8_t *data, uint16_t length);
void serial_process(void);
