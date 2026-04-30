#pragma once

#include <stdint.h>

extern uint8_t *txbuffer;

void serial_send(uint8_t *data, uint16_t length);
void serial_process(void);
