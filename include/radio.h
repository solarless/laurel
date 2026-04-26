#pragma once

#include "lr1121.h"

#include <stdint.h>

extern struct lr1121_pa_config radio_pa_config;
extern struct lr1121_lora_modulation_params radio_modulation_params;
extern struct lr1121_lora_packet_params radio_packet_params;

void radio_setup(void);
void radio_send(uint8_t *data, uint8_t length);
void radio_process(void);
