#include <stddef.h>
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "lr1121.h"
#include "pinout.h"

static void set_nss_low(void)
{
    gpio_clear(LR1121_NSS_PORT, LR1121_NSS_PIN);
}

static void set_nss_high(void)
{
    gpio_set(LR1121_NSS_PORT, LR1121_NSS_PIN);
}

static void wait_busy(void)
{
    while (gpio_get(LR1121_BUSY_PORT, LR1121_BUSY_PIN));
}

void lr1121_get_version(struct lr1121_version *version)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x01);
    set_nss_high();

    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x00);  // ignore stat1
    version->hw_version = spi_xfer(LR1121_SPI, 0x00);
    version->use_case = spi_xfer(LR1121_SPI, 0x00);
    version->fw_major = spi_xfer(LR1121_SPI, 0x00);
    version->fw_minor = spi_xfer(LR1121_SPI, 0x00);
    set_nss_high();
}

void lr1121_write_buffer8(uint8_t *data, uint8_t size)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x09);
    for (uint8_t i = 0; i < size; i++) {
        (void)spi_xfer(LR1121_SPI, data[i]);
    }
    set_nss_high();
}

void lr1121_read_buffer8(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x0a);
    (void)spi_xfer(LR1121_SPI, offset);
    (void)spi_xfer(LR1121_SPI, size);
    set_nss_high();

    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x00);  // ignore stat1
    for (uint8_t i = 0; i < size; i++) {
        buffer[i] = spi_xfer(LR1121_SPI, 0x00);
    }
    set_nss_high();
}

void lr1121_get_errors(uint16_t *errors)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x0d);
    set_nss_high();

    wait_busy();

    uint8_t buffer[2];
    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x00);  // ignore stat1
    buffer[0x01] = spi_xfer(LR1121_SPI, 0x00);
    buffer[0x00] = spi_xfer(LR1121_SPI, 0x00);
    set_nss_high();
    *errors = *(uint16_t *)buffer;
}

void lr1121_clear_errors(void)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x0e);
    set_nss_high();
}

void lr1121_calibrate(uint8_t params)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x0f);
    (void)spi_xfer(LR1121_SPI, params);
    set_nss_high();
}

void lr1121_calibrate_image(uint8_t frequency1, uint8_t frequency2)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x11);
    (void)spi_xfer(LR1121_SPI, frequency1);
    (void)spi_xfer(LR1121_SPI, frequency2);
    set_nss_high();
}

void lr1121_set_dio_as_rf_switch(struct lr1121_dio_rf_switch_config *config)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x12);
    (void)spi_xfer(LR1121_SPI, config->enable);
    (void)spi_xfer(LR1121_SPI, config->standby);
    (void)spi_xfer(LR1121_SPI, config->rx);
    (void)spi_xfer(LR1121_SPI, config->tx);
    (void)spi_xfer(LR1121_SPI, config->tx_hp);
    (void)spi_xfer(LR1121_SPI, config->tx_hf);
    (void)spi_xfer(LR1121_SPI, 0x00);
    (void)spi_xfer(LR1121_SPI, 0x00);
    set_nss_high();
}

void lr1121_set_dio_irq_params(uint32_t irqs1, uint32_t irqs2)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x13);
    (void)spi_xfer(LR1121_SPI, (irqs1 & 0xff000000) >> 24);
    (void)spi_xfer(LR1121_SPI, (irqs1 & 0x00ff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (irqs1 & 0x0000ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (irqs1 & 0x000000ff) >> 0);
    (void)spi_xfer(LR1121_SPI, (irqs2 & 0xff000000) >> 24);
    (void)spi_xfer(LR1121_SPI, (irqs2 & 0x00ff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (irqs2 & 0x0000ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (irqs2 & 0x000000ff) >> 0);
    set_nss_high();
}

void lr1121_clear_irq(uint32_t clear, uint32_t *pending)
{
    wait_busy();

    uint8_t buffer[4];

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x14);
    buffer[0x03] = spi_xfer(LR1121_SPI, (clear & 0xff000000) >> 24);
    buffer[0x02] = spi_xfer(LR1121_SPI, (clear & 0x00ff0000) >> 16);
    buffer[0x01] = spi_xfer(LR1121_SPI, (clear & 0x0000ff00) >> 8);
    buffer[0x00] = spi_xfer(LR1121_SPI, (clear & 0x000000ff) >> 0);
    set_nss_high();

    if (pending != NULL) {
        *pending = *(uint32_t *)buffer;
    }
}

void lr1121_set_tcxo_mode(uint8_t tune, uint32_t timeout)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x17);
    (void)spi_xfer(LR1121_SPI, tune);
    (void)spi_xfer(LR1121_SPI, (timeout & 0xff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x00ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x0000ff) >> 0);
    set_nss_high();
}

void lr1121_set_standby(uint8_t config)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x01);
    (void)spi_xfer(LR1121_SPI, 0x1c);
    (void)spi_xfer(LR1121_SPI, config);
    set_nss_high();
}

void lr1121_get_rx_buffer_status(uint8_t *size, uint8_t *offset)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x03);
    set_nss_high();

    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x00);  // ignore stat1
    *size = spi_xfer(LR1121_SPI, 0x00);
    *offset = spi_xfer(LR1121_SPI, 0x00);
    set_nss_high();
}

void lr1121_set_rx(uint32_t timeout)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x09);
    (void)spi_xfer(LR1121_SPI, (timeout & 0xff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x00ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x0000ff) >> 0);
    set_nss_high();
}

void lr1121_set_tx(uint32_t timeout)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x0a);
    (void)spi_xfer(LR1121_SPI, (timeout & 0xff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x00ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (timeout & 0x0000ff) >> 0);
    set_nss_high();
}

void lr1121_set_rf_frequency(uint32_t frequency)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x0b);
    (void)spi_xfer(LR1121_SPI, (frequency & 0xff000000) >> 24);
    (void)spi_xfer(LR1121_SPI, (frequency & 0x00ff0000) >> 16);
    (void)spi_xfer(LR1121_SPI, (frequency & 0x0000ff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (frequency & 0x000000ff) >> 0);
    set_nss_high();
}

void lr1121_set_packet_type(uint8_t type)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x0e);
    (void)spi_xfer(LR1121_SPI, type);
    set_nss_high();
}

void lr1121_lora_set_modulation_params(struct lr1121_lora_modulation_params *params)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x0f);
    (void)spi_xfer(LR1121_SPI, params->spreading_factor);
    (void)spi_xfer(LR1121_SPI, params->bandwidth);
    (void)spi_xfer(LR1121_SPI, params->coding_rate);
    (void)spi_xfer(LR1121_SPI, params->ldro);
    set_nss_high();
}

void lr1121_lora_set_packet_params(struct lr1121_lora_packet_params *params)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x10);
    (void)spi_xfer(LR1121_SPI, (params->preamble_length & 0xff00) >> 8);
    (void)spi_xfer(LR1121_SPI, (params->preamble_length & 0x00ff) >> 0);
    (void)spi_xfer(LR1121_SPI, params->header_type);
    (void)spi_xfer(LR1121_SPI, params->payload_length);
    (void)spi_xfer(LR1121_SPI, params->crc);
    (void)spi_xfer(LR1121_SPI, params->iq);
    set_nss_high();
}

void lr1121_set_tx_params(uint8_t power, uint8_t ramp_time)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x11);
    (void)spi_xfer(LR1121_SPI, power);
    (void)spi_xfer(LR1121_SPI, ramp_time);
    set_nss_high();
}

void lr1121_set_pa_config(struct lr1121_pa_config *config)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x15);
    (void)spi_xfer(LR1121_SPI, config->pa_selection);
    (void)spi_xfer(LR1121_SPI, config->reg_pa_supply);
    (void)spi_xfer(LR1121_SPI, config->pa_duty_cycle);
    (void)spi_xfer(LR1121_SPI, config->pa_hp_selection);
    set_nss_high();
}

void lr1121_set_rx_boosted(uint8_t boosted)
{
    wait_busy();

    set_nss_low();
    (void)spi_xfer(LR1121_SPI, 0x02);
    (void)spi_xfer(LR1121_SPI, 0x27);
    (void)spi_xfer(LR1121_SPI, boosted);
    set_nss_high();
}
