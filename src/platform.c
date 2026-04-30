#include "platform.h"

#include "serial.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <stdint.h>

uint32_t systick;

void sys_tick_handler(void);

void exti1_isr(void);
void handle_lr1121_irq(void);

void usart1_isr(void);
void handle_uart_rx(void);

void dma1_channel4_isr(void);
void handle_uart_dma_complete(void);

void platform_setup(void)
{
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_DMA1);

    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_set_reload(rcc_ahb_frequency / 1000 - 1);
    systick_interrupt_enable();
    systick_counter_enable();

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  LR1121_MISO_PIN | LR1121_BUSY_PIN | CH340X_TXD_PIN);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  LR1121_NSS_PIN);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  LR1121_MOSI_PIN | LR1121_SCK_PIN | CH340X_RXD_PIN);
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  LR1121_DIO9_PIN);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  LED1_PIN | LED2_PIN | LR1121_NRESET_PIN);

    exti_enable_request(EXTI1);
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);
    exti_select_source(EXTI1, GPIOB);

    nvic_set_priority(NVIC_EXTI1_IRQ, 0);
    nvic_enable_irq(NVIC_EXTI1_IRQ);

    /* LEDs are cathode connected */
    gpio_set(LED1_PORT, LED1_PIN);
    gpio_set(LED2_PORT, LED2_PIN);
    /* These signals are active low */
    gpio_set(LR1121_NRESET_PORT, LR1121_NRESET_PIN);
    gpio_set(LR1121_NSS_PORT, LR1121_NSS_PIN);

    usart_set_baudrate(USART1, 115200);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_enable_rx_interrupt(USART1);
    usart_enable_tx_dma(USART1);
    usart_enable(USART1);

    dma_set_peripheral_address(DMA1, DMA_CHANNEL4, USART1_DR);
    dma_set_memory_address(DMA1, DMA_CHANNEL4, txbuffer);
    dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_VERY_HIGH);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);
    dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);

    nvic_set_priority(NVIC_USART1_IRQ, 0);
    nvic_enable_irq(NVIC_USART1_IRQ);

    /* LR1121 SPI maximum speed is 16 MHz */
    spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_4);
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_set_master_mode(SPI1);
    spi_enable(SPI1);
}

void sys_tick_handler(void)
{
    asm volatile ("cpsid i" : : : "memory");
    systick++;
    asm volatile ("cpsie i" : : : "memory");
}

void exti1_isr(void)
{
    exti_reset_request(EXTI1);
    handle_lr1121_irq();
}

__attribute__((weak)) void handle_lr1121_irq(void)
{
}

void usart1_isr(void)
{
    handle_uart_rx();
}

__attribute__((weak)) void handle_uart_rx(void)
{
}

void dma1_channel4_isr(void)
{
    dma_clear_interrupt_flags(DMA1, DMA_CHANNEL4, DMA_IFCR_CTCIF4);
    handle_uart_dma_complete();
}

__attribute__((weak)) void handle_uart_dma_complete(void)
{
}
