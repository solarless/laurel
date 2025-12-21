#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "spi.h"

void spi_initialize(void)
{
    rcc_periph_clock_enable(RCC_SPI1);

    spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_4);
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_set_master_mode(SPI1);
    spi_enable(SPI1);
}
