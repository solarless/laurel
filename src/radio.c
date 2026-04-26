#include "radio.h"

#include "led.h"
#include "lr1121.h"
#include "platform.h"
#include "serial.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct lr1121_lora_modulation_params radio_modulation_params = {
    .spreading_factor = LR1121_LORA_SPREADING_FACTOR_8,
    .bandwidth = LR1121_LORA_BANDWIDTH_62,
    .coding_rate = LR1121_LORA_CODING_RATE_SHORT_4_6,
    .ldro_enabled = false,
};

struct lr1121_lora_packet_params radio_packet_params = {
    .preamble_length = 8,
    .header_type = LR1121_LORA_HEADER_TYPE_EXPLICIT,
    .payload_length = 0,
    .crc_enabled = true,
    .iq_inverted = false,
};

/* Low power PA 14 dBm, see docs/lr1121-user-manual.pdf Table 9-1 */
struct lr1121_pa_config radio_pa_config = {
    .pa = 0x00,
    .pa_power_source = 0x00,
    .pa_duty_cycle = 0x05,
    .hp_pa_size = 0x00,
};

/* see docs/e80-900m2213s-user-manual.pdf
    Chapter 3 Mechanical Dimensions and Pin Definition */
static struct lr1121_dio_rf_switch_config rf_switch_config = {
    .enable = LR1121_RFSW0 | LR1121_RFSW1,
    .standby = 0x00,
    .rx = 0x00,
    .tx = LR1121_RFSW1,
    .tx_hp = LR1121_RFSW0,
    .tx_hf = LR1121_RFSW0 | LR1121_RFSW1,
};

static bool interrupt;

static void handle_tx_done(void);
static void handle_rx_done(void);
void handle_lr1121_irq(void);

void radio_setup(void)
{
    lr1121_set_tcxo_mode(LR1121_TCXO_TUNE_1V8, 320);
    lr1121_clear_errors();
    lr1121_clear_irq(LR1121_IRQ_ALL, NULL);

    lr1121_calibrate(LR1121_CALIBRATE_ALL);
    lr1121_calibrate_image(LR1121_IMAGE_CALIBRATION_863,
                           LR1121_IMAGE_CALIBRATION_870);

    lr1121_set_packet_type(LR1121_PACKET_TYPE_LORA);
    lr1121_lora_set_modulation_params(&radio_modulation_params);

    lr1121_set_pa_config(&radio_pa_config);
    lr1121_set_tx_params(14, LR1121_RAMP_TIME_48_US);
    lr1121_set_rx_boosted(true);

    /* Center of 868.7-869.2 MHz band */
    lr1121_set_rf_frequency(868950000);

    lr1121_set_dio_as_rf_switch(&rf_switch_config);
    lr1121_set_dio_irq_params(LR1121_IRQ_TX_DONE | LR1121_IRQ_RX_DONE, 0x00);

    lr1121_lora_set_packet_params(&radio_packet_params);
    lr1121_set_rx(0x00);
}

void radio_send(uint8_t *data, uint8_t length)
{
    radio_packet_params.payload_length = length;
    lr1121_write_buffer8(data, length);
    lr1121_lora_set_packet_params(&radio_packet_params);
    lr1121_set_tx(0x00);
    led_blink(LED2);
}

void radio_process(void)
{
    uint32_t pending = 0x00;

    if (interrupt) {
        interrupt = false;
        lr1121_clear_irq(LR1121_IRQ_ALL, &pending);
    }

    if (pending & LR1121_IRQ_TX_DONE)
        handle_tx_done();
    if (pending & LR1121_IRQ_RX_DONE)
        handle_rx_done();
}

static void handle_tx_done(void)
{
    radio_packet_params.payload_length = 0;
    lr1121_lora_set_packet_params(&radio_packet_params);
    lr1121_set_rx(0x00);
}

static void handle_rx_done(void)
{
    led_blink(LED1);
    uint8_t buffer[256];
    uint8_t length;
    uint8_t offset;
    lr1121_get_rx_buffer_status(&length, &offset);
    lr1121_read_buffer8(offset, buffer, length);
    lr1121_set_rx(0x00);
    serial_send(buffer, length);
}

void handle_lr1121_irq(void)
{
    interrupt = true;
}
