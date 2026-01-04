#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libopencm3/stm32/gpio.h>

#include "lr1121.h"
#include "phy.h"
#include "pinout.h"

uint8_t phy_status;
uint8_t phy_payload_size;

void phy_setup(void)
{
    lr1121_set_tcxo_mode(LR1121_TCXO_TUNE_1_8_V, 320);
    lr1121_clear_errors();
    lr1121_clear_irq(LR1121_IRQ_ALL, NULL);

    lr1121_calibrate(LR1121_CALIBRATE_ALL);
    lr1121_calibrate_image(LR1121_IMAGE_CALIBRATION_863, LR1121_IMAGE_CALIBRATION_870);

    struct lr1121_lora_modulation_params modulation_params = {
        .spreading_factor = LR1121_LORA_MODULATION_SPREADING_FACTOR_7,
        .bandwidth = LR1121_LORA_MODULATION_BANDWIDTH_62,
        .coding_rate = LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_6,
        .ldro = LR1121_LORA_MODULATION_LDRO_DISABLED,
    };

    lr1121_set_packet_type(LR1121_PACKET_TYPE_LORA);
    lr1121_lora_set_modulation_params(&modulation_params);

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

struct __attribute__((packed)) packet {
    uint32_t address;
    uint8_t data[];
};

#define ADDRESS_SIZE (sizeof(((struct packet *)0x00)->address))

#define MAX_FRAME_SIZE (253)
#define MAX_PAYLOAD_SIZE (MAX_FRAME_SIZE - ADDRESS_SIZE)

static struct lr1121_lora_packet_params packet_params = {
    .preamble_length = 8,
    .header_type = LR1121_LORA_PACKET_HEADER_TYPE_EXPLICIT,
    .payload_length = 0,
    .crc = LR1121_LORA_PACKET_CRC_ENABLED,
    .iq = LR1121_LORA_PACKET_IQ_STANDARD,
};

int8_t phy_send(uint8_t *data, uint8_t size)
{
    if (size >= MAX_PAYLOAD_SIZE) {
        return -1;
    }

    uint8_t buffer[sizeof(struct packet) + size];
    (void)memset(buffer, 0x00, sizeof(buffer));
    struct packet *const packet = (struct packet *)buffer;
    packet->address = 0x11223344;
    (void)memcpy(packet->data, data, size);

    struct lr1121_lora_packet_params params;
    (void)memcpy(&params, &packet_params, sizeof(params));
    params.payload_length = sizeof(buffer);

    lr1121_write_buffer8(buffer, sizeof(buffer));
    lr1121_lora_set_packet_params(&params);
    lr1121_set_tx(0x00);

    return 0;
}

void phy_receive(void)
{
    lr1121_lora_set_packet_params(&packet_params);
    lr1121_set_rx(0x00);
}

static void tx_done_handler(void)
{
    phy_status |= PHY_STATUS_TX_DONE;
}

static uint8_t rx_buffer[256];
static struct packet *const rx_packet = (struct packet *)rx_buffer;

static void rx_done_handler(void)
{
    uint8_t frame_size;
    uint8_t offset;
    lr1121_get_rx_buffer_status(&frame_size, &offset);
    lr1121_read_buffer8(offset, rx_buffer, frame_size);
    uint8_t payload_size = frame_size - ADDRESS_SIZE;

    if (rx_packet->address != 0x11223344) {
        phy_receive();
        return;
    }

    phy_status |= PHY_STATUS_RX_DONE;
    phy_payload_size = payload_size;
}

void phy_read_buffer(uint8_t *buffer, uint8_t size)
{
    (void)memcpy(buffer, rx_buffer, size);
}

static uint8_t irq_happened;

void phy_process(void)
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
