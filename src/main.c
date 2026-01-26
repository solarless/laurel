#include <stdint.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "core.h"
#include "gpio.h"
#include "pinout.h"
#include "radio.h"
#include "serial.h"
#include "spi.h"
#include "systick.h"
#include "usart.h"

int main(void)
{
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
    systick_initialize();

    gpio_initialize();
    gpio_set(LED1_PORT, LED1_PIN);
    gpio_set(LED2_PORT, LED2_PIN);
    gpio_set(LR1121_NRESET_PORT, LR1121_NRESET_PIN);
    gpio_set(LR1121_NSS_PORT, LR1121_NSS_PIN);

    spi_initialize();
    usart_initialize();

    core_setup();
    radio_setup();

    for (;;) {
        core_process();
        serial_process();
        radio_process();
    }

    return 0xdead;
}
