#ifndef _PHY_H
#define _PHY_H

#include <stdint.h>

void phy_setup(void);

void phy_send(uint8_t *data, uint8_t size);

#endif
