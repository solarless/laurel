#pragma once

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

/* ======== Buttons ======== */

#define K1_PORT (GPIOB)
#define K1_PIN (GPIO15)

#define K2_PORT (GPIOB)
#define K2_PIN (GPIO14)

/* ======== LEDs  ======== */

#define LED1_PORT (GPIOB)
#define LED1_PIN (GPIO13)

#define LED2_PORT (GPIOB)
#define LED2_PIN (GPIO12)

/* ======== CH340X ======== */

#define CH340X_TXD_PORT (GPIOA) /* RX on my side */
#define CH340X_TXD_PIN (GPIO10)

#define CH340X_RXD_PORT (GPIOA) /* TX on my side */
#define CH340X_RXD_PIN (GPIO9)

#define CH340X_USART (USART1)

/* ======== LR1121 ======== */

#define LR1121_NRESET_PORT (GPIOB)
#define LR1121_NRESET_PIN (GPIO0)

#define LR1121_DIO9_PORT (GPIOB)
#define LR1121_DIO9_PIN (GPIO1)

#define LR1121_DIO8_PORT (GPIOB)
#define LR1121_DIO8_PIN (GPIO2)

#define LR1121_DIO7_PORT (GPIOB)
#define LR1121_DIO7_PIN (GPIO10)

#define LR1121_MISO_PORT (GPIOA)
#define LR1121_MISO_PIN (GPIO6)

#define LR1121_MOSI_PORT (GPIOA)
#define LR1121_MOSI_PIN (GPIO7)

#define LR1121_SCK_PORT (GPIOA)
#define LR1121_SCK_PIN (GPIO5)

#define LR1121_NSS_PORT (GPIOA)
#define LR1121_NSS_PIN (GPIO4)

#define LR1121_BUSY_PORT (GPIOA)
#define LR1121_BUSY_PIN (GPIO3)

#define LR1121_SPI (SPI1)
