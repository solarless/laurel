#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lr1121.h"
#include "radio.h"

struct lr1121_lora_modulation_params radio_modulation_params = {
    .spreading_factor = LR1121_LORA_MODULATION_SPREADING_FACTOR_8,
    .bandwidth = LR1121_LORA_MODULATION_BANDWIDTH_62,
    .coding_rate = LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_6,
    .ldro = LR1121_LORA_MODULATION_LDRO_DISABLED,
};

struct lr1121_lora_packet_params radio_packet_params = {
    .preamble_length = 8,
    .header_type = LR1121_LORA_PACKET_HEADER_TYPE_EXPLICIT,
    .payload_length = 0,
    .crc = LR1121_LORA_PACKET_CRC_ENABLED,
    .iq = LR1121_LORA_PACKET_IQ_STANDARD,
};

void radio_setup(void)
{
    lr1121_set_tcxo_mode(LR1121_TCXO_TUNE_1_8_V, 320);
    lr1121_clear_errors();
    lr1121_clear_irq(LR1121_IRQ_ALL, NULL);

    lr1121_calibrate(LR1121_CALIBRATE_ALL);
    lr1121_calibrate_image(LR1121_IMAGE_CALIBRATION_863, LR1121_IMAGE_CALIBRATION_870);

    lr1121_set_packet_type(LR1121_PACKET_TYPE_LORA);
    lr1121_lora_set_modulation_params(&radio_modulation_params);

    struct lr1121_pa_config pa_config = {
        .pa_selection = LR1121_PA_LOW_POWER_PA,
        .reg_pa_supply = LR1121_PA_SUPPLY_INTERNAL_REGULATOR,
        .pa_duty_cycle = 0x05,
        .pa_hp_selection = 0x00,
    };

    lr1121_set_pa_config(&pa_config);
    lr1121_set_tx_params(14, LR1121_TX_RAMP_TIME_48_US);
    lr1121_set_rx_boosted(1);

    lr1121_set_rf_frequency(868750000);

    struct lr1121_dio_rf_switch_config rf_switch_config = {
        .enable = LR1121_RFSW0 | LR1121_RFSW1,
        .standby = 0x00,
        .rx = 0x00,
        .tx = LR1121_RFSW1,
        .tx_hp = LR1121_RFSW0,
        .tx_hf = LR1121_RFSW0 | LR1121_RFSW1,
    };

    lr1121_set_dio_as_rf_switch(&rf_switch_config);
    lr1121_set_dio_irq_params(LR1121_IRQ_TX_DONE | LR1121_IRQ_RX_DONE, 0x00);
}

static radio_sent_handler sent_handler;
static radio_received_handler received_handler;

#define MAX_FRAME_SIZE (253)

int8_t radio_send(uint8_t *data, uint8_t size, radio_sent_handler handler)
{
    if (size >= MAX_FRAME_SIZE) {
        return -1;
    }

    struct lr1121_lora_packet_params params;
    (void)memcpy(&params, &radio_packet_params, sizeof(params));
    params.payload_length = size;

    lr1121_write_buffer8(data, size);
    lr1121_lora_set_packet_params(&params);
    lr1121_set_tx(0x00);
    sent_handler = handler;

    return 0;
}

void radio_receive(radio_received_handler handler)
{
    lr1121_lora_set_packet_params(&radio_packet_params);
    lr1121_set_rx(0x00);
    received_handler = handler;
}

void radio_idle(void)
{
    lr1121_set_standby(LR1121_STANDBY_RC);
    sent_handler = NULL;
    received_handler = NULL;
}

static void tx_done_handler(void)
{
    if (sent_handler == NULL) {
        return;
    }

    radio_sent_handler handler = sent_handler;
    sent_handler = NULL;
    handler();
}

static void rx_done_handler(void)
{
    if (received_handler == NULL) {
        return;
    }

    uint8_t buffer[256];
    uint8_t size;
    uint8_t offset;
    lr1121_get_rx_buffer_status(&size, &offset);
    lr1121_read_buffer8(offset, buffer, size);

    radio_received_handler handler = received_handler;
    received_handler = NULL;
    handler(buffer, size);
}

static uint8_t irq_happened;

void radio_process(void)
{
    uint32_t pending = 0x00;

    if (irq_happened) {
        irq_happened = 0;
        lr1121_clear_irq(LR1121_IRQ_ALL, &pending);
    }

    if (pending & LR1121_IRQ_TX_DONE) {
        tx_done_handler();
    }

    if (pending & LR1121_IRQ_RX_DONE) {
        rx_done_handler();
    }
}

void exti1_handler(void)
{
    irq_happened = 1;
}
