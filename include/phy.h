#ifndef _PHY_H
#define _PHY_H

#include <stdint.h>

#define PHY_STATUS_TX_DONE (0x01)
#define PHY_STATUS_RX_DONE (0x02)

extern uint8_t phy_status;
extern uint8_t phy_payload_size;

void phy_setup(void);

int8_t phy_send(uint8_t *data, uint8_t size);

void phy_receive(void);

void phy_read_buffer(uint8_t *buffer, uint8_t size);

void phy_process(void);

#endif
