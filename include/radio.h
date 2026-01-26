#ifndef _RADIO_H
#define _RADIO_H

#include <stdint.h>

#include "lr1121.h"

extern struct lr1121_lora_modulation_params radio_modulation_params;
extern struct lr1121_lora_packet_params radio_packet_params;

void radio_setup(void);

typedef void (*radio_sent_handler)(void);
typedef void (*radio_received_handler)(uint8_t *data, uint8_t size);

int8_t radio_send(uint8_t *data, uint8_t size, radio_sent_handler handler);

void radio_receive(radio_received_handler handler);

void radio_idle(void);

void radio_process(void);

#endif
