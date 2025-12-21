#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

void usart_initialize(void)
{
    rcc_periph_clock_enable(RCC_USART1);

    usart_set_baudrate(USART1, 115200);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_enable(USART1);
}
