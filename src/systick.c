#include <libopencm3/cm3/systick.h>

#include "rcc.h"
#include "systick.h"

static uint32_t milliseconds_counter = 0;

void sys_tick_handler(void)
{
    __asm volatile ("cpsid i" : : : "memory");
    milliseconds_counter++;
    __asm volatile ("cpsie i" : : : "memory");
}

void systick_initialize(void)
{
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    uint32_t sysclk = rcc_get_sysclk();
    systick_set_reload(sysclk / 1000 - 1);
    systick_interrupt_enable();
    systick_counter_enable();
}

uint32_t systick_get_counter(void)
{
    return milliseconds_counter;
}
