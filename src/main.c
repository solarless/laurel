#include "platform.h"
#include "radio.h"
#include "serial.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <stdint.h>

uint32_t systick;

int main(void)
{
    /* ======== RCC ======== */

    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_SPI1);

    /* ======== SysTick ======== */

    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_set_reload(rcc_ahb_frequency / 1000 - 1);
    systick_interrupt_enable();
    systick_counter_enable();

    /* ======== GPIO ======== */

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  LR1121_MISO_PIN | LR1121_BUSY_PIN | CH340X_TXD_PIN);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  LR1121_NSS_PIN);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  LR1121_MOSI_PIN | LR1121_SCK_PIN | CH340X_RXD_PIN);
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  LR1121_DIO9_PIN);
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN,
                  K1_PIN | K2_PIN);
    /* Pull buttons up */
    gpio_set(GPIOB, K1_PIN | K2_PIN);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  LED1_PIN | LED2_PIN | LR1121_NRESET_PIN);
    exti_enable_request(EXTI1);
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);
    exti_select_source(EXTI1, GPIOB);

    gpio_set(LED1_PORT, LED1_PIN);
    gpio_set(LED2_PORT, LED2_PIN);
    gpio_set(LR1121_NRESET_PORT, LR1121_NRESET_PIN);
    gpio_set(LR1121_NSS_PORT, LR1121_NSS_PIN);

    /* ======== USART ======== */

    usart_set_baudrate(USART1, 115200);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_enable_rx_interrupt(USART1);
    usart_enable(USART1);

    /* ======== SPI ======== */

    spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_4);
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_set_master_mode(SPI1);
    spi_enable(SPI1);

    /* ======== NVIC ======== */

    nvic_set_priority(NVIC_EXTI1_IRQ, 0);
    nvic_enable_irq(NVIC_EXTI1_IRQ);

    nvic_set_priority(NVIC_USART1_IRQ, 0);
    nvic_enable_irq(NVIC_USART1_IRQ);


    radio_setup();

    gpio_clear(GPIOB, LED2_PIN);
    uint32_t blink_timer = 0;
    for (;;) {
        uint32_t now = systick;
        if (now - blink_timer) {
            blink_timer = now;
            gpio_toggle(GPIOB, LED1_PIN | LED2_PIN);
        }
    }

    return 0xdead;
}

void sys_tick_handler(void)
{
    asm volatile ("cpsid i" : : : "memory");
    systick++;
    asm volatile ("cpsie i" : : : "memory");
}

__attribute__((weak)) void exti1_handler(void)
{
}

void exti1_isr(void)
{
    exti_reset_request(EXTI1);
    exti1_handler();
}
