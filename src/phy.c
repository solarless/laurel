#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "lr1121.h"
#include "phy.h"

static char *stoa(uint8_t stat1)
{
    switch (stat1 & 0x0e) {
    case 0x00: return "FAIL";
    case 0x02: return "PERR";
    case 0x04: return "OK";
    case 0x06: return "DAT";
    default: return "-";
    }
}

static void log_status(char *command)
{
    uint8_t stat1;
    uint8_t stat2;
    uint32_t irqs;

    lr1121_get_status(&stat1, &stat2, &irqs);
    printd("%s... %s\r\n", command, stoa(stat1));
}

static void log_errors(void)
{
    uint16_t errors;
    lr1121_get_errors(&errors);

    if (errors & LR1121_ERROR_LF_RC_CALIBRATION) {
        printd("    LF RC calibration error\r\n");
    }

    if (errors & LR1121_ERROR_HF_RC_CALIBRATION) {
        printd("    HF RC calibration error\r\n");
    }

    if (errors & LR1121_ERROR_ADC_CALIBRATION) {
        printd("    ADC calibration error\r\n");
    }

    if (errors & LR1121_ERROR_PLL_CALIBRATION) {
        printd("    PLL calibration error\r\n");
    }

    if (errors & LR1121_ERROR_IMAGE_CALIBRATION) {
        printd("    image calibration error\r\n");
    }

    if (errors & LR1121_ERROR_HF_XOSC_START) {
        printd("    HF XOSC start error\r\n");
    }

    if (errors & LR1121_ERROR_LF_XOSC_START) {
        printd("    LF XOSC start error\r\n");
    }

    if (errors & LR1121_ERROR_PLL_LOCK) {
        printd("    PLL lock error\r\n");
    }

    if (errors & LR1121_ERROR_RX_ADC_OFFSET) {
        printd("    RX ADC offset error\r\n");
    }
}

static struct lr1121_lora_packet_params current_packet_params = {0x00};

void phy_setup(void)
{
    lr1121_set_tcxo_mode(LR1121_TCXO_TUNE_1_8_V, 320);
    log_status("lr1121_set_tcxo_mode");
    lr1121_clear_errors();

    lr1121_calibrate(LR1121_CALIBRATE_ALL);
    log_status("lr1121_calibrate");
    log_errors();

    lr1121_calibrate_image(LR1121_IMAGE_CALIBRATION_863, LR1121_IMAGE_CALIBRATION_870);
    log_status("lr1121_calibrate_image");
    log_errors();

    struct lr1121_lora_modulation_params modulation_params = {
        .spreading_factor = LR1121_LORA_MODULATION_SPREADING_FACTOR_7,
        .bandwidth = LR1121_LORA_MODULATION_BANDWIDTH_62,
        .coding_rate = LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_6,
        .ldro = LR1121_LORA_MODULATION_LDRO_DISABLED,
    };

    struct lr1121_lora_packet_params packet_params = {
        .preamble_length = 8,
        .header_type = LR1121_LORA_PACKET_HEADER_TYPE_EXPLICIT,
        .payload_length = 0,
        .crc = LR1121_LORA_PACKET_CRC_ENABLED,
        .iq = LR1121_LORA_PACKET_IQ_STANDARD,
    };

    lr1121_set_packet_type(LR1121_PACKET_TYPE_LORA);
    log_status("lr1121_set_packet_type");

    lr1121_lora_set_modulation_params(&modulation_params);
    log_status("lr1121_lora_set_modulation_params");

    lr1121_lora_set_packet_params(&packet_params);
    log_status("lr1121_lora_set_packet_params");
    (void)memcpy(&current_packet_params, &packet_params, sizeof(current_packet_params));

    struct lr1121_pa_config pa_config = {
        .pa_selection = LR1121_PA_LOW_POWER_PA,
        .reg_pa_supply = LR1121_PA_SUPPLY_INTERNAL_REGULATOR,
        .pa_duty_cycle = 0x05,
        .pa_hp_selection = 0x00,
    };

    lr1121_set_pa_config(&pa_config);
    log_status("lr1121_set_pa_config");

    lr1121_set_tx_params(14, LR1121_TX_RAMP_TIME_48_US);
    log_status("lr1121_set_tx_params");

    lr1121_set_rf_frequency(868750000);
    log_status("lr1121_set_rf_frequency");

    struct lr1121_dio_rf_switch_config rf_switch_config = {
        .enable = LR1121_RFSW0 | LR1121_RFSW1,
        .standby = 0x00,
        .rx = 0x00,
        .tx = LR1121_RFSW1,
        .tx_hp = LR1121_RFSW0,
        .tx_hf = LR1121_RFSW0 | LR1121_RFSW1,
    };

    lr1121_set_dio_as_rf_switch(&rf_switch_config);
    log_status("lr1121_set_dio_as_rf_switch");

    lr1121_set_dio_irq_params(LR1121_IRQ_TX_DONE | LR1121_IRQ_RX_DONE, 0x00);
    log_status("lr1121_set_dio_irq_params");
}

void phy_send(uint8_t *data, uint8_t size)
{
    lr1121_write_buffer8(data, size);

    struct lr1121_lora_packet_params packet_params = {0x00};
    (void)memcpy(&packet_params, &current_packet_params, sizeof(packet_params));
    packet_params.payload_length = size;
    lr1121_lora_set_packet_params(&packet_params);

    lr1121_set_tx(0x00);
}
