#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "gpio.h"
#include "pinout.h"

void gpio_initialize(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(
        GPIOA,
        GPIO_MODE_INPUT,
        GPIO_CNF_INPUT_FLOAT,
        LR1121_MISO_PIN | LR1121_BUSY_PIN | CH340X_TXD_PIN);

    gpio_set_mode(
        GPIOA,
        GPIO_MODE_OUTPUT_2_MHZ,
        GPIO_CNF_OUTPUT_PUSHPULL,
        LR1121_NSS_PIN);

    gpio_set_mode(
        GPIOA,
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        LR1121_MOSI_PIN | LR1121_SCK_PIN | CH340X_RXD_PIN);

    gpio_set_mode(
        GPIOB,
        GPIO_MODE_INPUT,
        GPIO_CNF_INPUT_PULL_UPDOWN,
        BUTTON1_PIN | BUTTON2_PIN);
    gpio_set(GPIOB, BUTTON1_PIN | BUTTON2_PIN);

    gpio_set_mode(
        GPIOB,
        GPIO_MODE_OUTPUT_2_MHZ,
        GPIO_CNF_OUTPUT_PUSHPULL,
        LED1_PIN | LED2_PIN | LR1121_NRESET_PIN | LR1121_DIO9_PIN | LR1121_DIO8_PIN | LR1121_DIO7_PIN);
}
