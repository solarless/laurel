#include "led.h"
#include "platform.h"
#include "radio.h"
#include "serial.h"

int main(void)
{
    platform_setup();
    radio_setup();

    for (;;) {
        serial_process();
        radio_process();
        led_process();
    }

    return 0xdead;
}
