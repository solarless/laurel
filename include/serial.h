#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

void serial_send(uint8_t *data, uint32_t size);

typedef void (*serial_received_handler)(uint8_t *data, uint32_t size);

void serial_receive_timeout(serial_received_handler handler);
void serial_receive_crlf(serial_received_handler handler);

void serial_process(void);

#endif
