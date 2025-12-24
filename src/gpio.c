#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "gpio.h"
#include "pinout.h"

#include "lr1121.h"

void gpio_initialize(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);

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
        GPIO_CNF_INPUT_FLOAT,
        LR1121_DIO9_PIN);

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
        LED1_PIN | LED2_PIN | LR1121_NRESET_PIN);

    exti_enable_request(EXTI1);
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);
    exti_select_source(EXTI1, GPIOB);
    nvic_set_priority(NVIC_EXTI1_IRQ, 0);
    nvic_enable_irq(NVIC_EXTI1_IRQ);
}

__attribute__((weak)) void exti1_handler(void)
{
}

void exti1_isr(void)
{
    exti_reset_request(EXTI1);
    exti1_handler();
}
