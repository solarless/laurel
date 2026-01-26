#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

#include "systick.h"

static uint32_t milliseconds_counter = 0;

void sys_tick_handler(void)
{
    asm volatile ("cpsid i" : : : "memory");
    milliseconds_counter++;
    asm volatile ("cpsie i" : : : "memory");
}

void systick_initialize(void)
{
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_set_reload(rcc_ahb_frequency / 1000 - 1);
    systick_interrupt_enable();
    systick_counter_enable();
}

uint32_t systick_get_counter(void)
{
    return milliseconds_counter;
}
