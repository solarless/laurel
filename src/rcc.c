#include <libopencm3/stm32/rcc.h>

#include "rcc.h"

static uint32_t sysclk = 0;

void rcc_initialize(void)
{
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
    sysclk = 64000000;
}

uint32_t rcc_get_sysclk(void)
{
    return sysclk;
}
