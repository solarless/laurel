#include "led.h"
#include "platform.h"

#include <libopencm3/stm32/gpio.h>

#include <stdbool.h>
#include <stdint.h>

#define BLINK_TIME (50)

static struct {
    uint32_t port;
    uint16_t pin;
    uint32_t start_time;
    bool active;
} leds[] = {
    [LED1] = {
        .port = LED1_PORT,
        .pin = LED1_PIN,
    },
    [LED2] = {
        .port = LED2_PORT,
        .pin = LED2_PIN,
    },
};

void led_blink(uint8_t ledno)
{
    gpio_clear(leds[ledno].port, leds[ledno].pin);
    leds[ledno].start_time = systick;
    leds[ledno].active = true;
}

void led_process(void)
{
    for (int i = 0; i < sizeof(leds); i++) {
        if (!leds[i].active)
            continue;

        if (systick - leds[i].start_time >= BLINK_TIME) {
            gpio_set(leds[i].port, leds[i].pin);
            leds[i].active = false;
        }
    }
}
