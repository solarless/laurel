#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "debug.h"
#include "gpio.h"
#include "pinout.h"
#include "lr1121.h"
#include "phy.h"
#include "rcc.h"
#include "spi.h"
#include "systick.h"
#include "usart.h"

int main(void)
{
    rcc_initialize();
    systick_initialize();

    gpio_initialize();
    gpio_set(LED1_PORT, LED1_PIN);
    gpio_set(LED2_PORT, LED2_PIN);
    gpio_set(LR1121_NRESET_PORT, LR1121_NRESET_PIN);
    gpio_set(LR1121_NSS_PORT, LR1121_NSS_PIN);

    spi_initialize();
    usart_initialize();

    struct lr1121_version version = {0x00};
    lr1121_get_version(&version);
    printd(
        "LR1121 version: hardware = %02x, uc = %02x, major = %02x, minor = %02x\r\n",
        version.hw_version,
        version.use_case,
        version.fw_major,
        version.fw_minor);

    phy_setup();

    printd("initialized\r\n");

    uint32_t now = systick_get_counter();
    uint32_t blink_counter = now;
    uint32_t spit_counter = now;
    gpio_clear(LED1_PORT, LED1_PIN);
    for (;;) {
        now = systick_get_counter();

        if (now - blink_counter >= 500) {
            blink_counter = now;
            gpio_toggle(LED1_PORT, LED1_PIN);
            gpio_toggle(LED2_PORT, LED2_PIN);
        }

        if (now - spit_counter >= 5000) {
            spit_counter = now;
            phy_send((uint8_t *)"that shit works", 15);
        }
    }

    return 0xdead;
}

void hard_fault_handler(void)
{
    printd("hardfault\r\n");
}
