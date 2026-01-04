#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

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

    phy_setup();

    uint32_t now = systick_get_counter();
    uint32_t sample_counter = now;
    uint8_t button1_last = 1;
    uint8_t button2_last = 1;

    phy_receive();

    uint8_t buffer[256] = {0x00};

    for (;;) {
        phy_process();

        now = systick_get_counter();
        if (now - sample_counter >= 50) {
            sample_counter = now;
            uint8_t button1_new = !!gpio_get(BUTTON1_PORT, BUTTON1_PIN);
            uint8_t button2_new = !!gpio_get(BUTTON2_PORT, BUTTON2_PIN);

            if (button1_last && !button1_new) {
                phy_send((uint8_t *)"\x01", 1);
            }

            if (button2_last && !button2_new) {
                phy_send((uint8_t *)"\x02", 1);
            }

            button1_last = button1_new;
            button2_last = button2_new;

            continue;
        }

        if (phy_status & PHY_STATUS_TX_DONE) {
            phy_status = 0x00;
            phy_receive();
        }

        if (!(phy_status & PHY_STATUS_RX_DONE)) {
            continue;
        }

        phy_status &= ~PHY_STATUS_RX_DONE;
        phy_read_buffer(buffer, 5);

        if (buffer[0x04] == 0x01) {
            gpio_toggle(LED1_PORT, LED1_PIN);
        } else if (buffer[0x04] == 0x02) {
            gpio_toggle(LED2_PORT, LED2_PIN);
        }

        phy_receive();
    }

    return 0xdead;
}
