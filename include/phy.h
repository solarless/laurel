#ifndef _PHY_H
#define _PHY_H

#include <stdint.h>

void phy_setup(void);

void phy_set_sent_handler(void (*handler)(void));

void phy_send(uint8_t *data, uint8_t size);

void phy_set_received_handler(void (*handler)(uint8_t *data, uint8_t size));

void phy_receive(void);

void phy_process(void);

#endif
