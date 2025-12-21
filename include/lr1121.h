#ifndef _LR1121_H
#define _LR1121_H

#include <stdint.h>

void lr1121_wait_busy(void);

void lr1121_get_status(uint8_t *stat1, uint8_t *stat2, uint32_t *irq_status);

struct lr1121_version {
    uint8_t hw_version;
    uint8_t use_case;
    uint8_t fw_major;
    uint8_t fw_minor;
};

void lr1121_get_version(struct lr1121_version *version);

void lr1121_write_buffer8(uint8_t *data, uint8_t size);

void lr1121_read_buffer8(uint8_t offset, uint8_t *buffer, uint8_t size);

#define LR1121_ERROR_LF_RC_CALIBRATION (0x0001)
#define LR1121_ERROR_HF_RC_CALIBRATION (0x0002)
#define LR1121_ERROR_ADC_CALIBRATION (0x0004)
#define LR1121_ERROR_PLL_CALIBRATION (0x0008)
#define LR1121_ERROR_IMAGE_CALIBRATION (0x0010)
#define LR1121_ERROR_HF_XOSC_START (0x0020)
#define LR1121_ERROR_LF_XOSC_START (0x0040)
#define LR1121_ERROR_PLL_LOCK (0x0080)
#define LR1121_ERROR_RX_ADC_OFFSET (0x0100)

void lr1121_get_errors(uint16_t *errors);

void lr1121_clear_errors(void);

#define LR1121_CALIBRATE_LF_RC (0x01)
#define LR1121_CALIBRATE_HF_RC (0x02)
#define LR1121_CALIBRATE_PLL (0x04)
#define LR1121_CALIBRATE_ADC (0x08)
#define LR1121_CALIBRATE_IMG (0x10)
#define LR1121_CALIBRATE_PLL_TX (0x20)
#define LR1121_CALIBRATE_ALL (0x3f)

void lr1121_calibrate(uint8_t params);

#define LR1121_IMAGE_CALIBRATION_863 (0xd7)
#define LR1121_IMAGE_CALIBRATION_870 (0xdb)

void lr1121_calibrate_image(int8_t frequency1, uint8_t frequency2);

struct lr1121_dio_rf_switch_config {
    uint8_t enable;
    uint8_t standby;
    uint8_t rx;
    uint8_t tx;
    uint8_t tx_hp;
    uint8_t tx_hf;
};

#define LR1121_RFSW0 (0x01)
#define LR1121_RFSW1 (0x02)
#define LR1121_RFSW2 (0x04)
#define LR1121_RFSW3 (0x08)
#define LR1121_RFSW4 (0x10)

void lr1121_set_dio_as_rf_switch(struct lr1121_dio_rf_switch_config *config);

#define LR1121_IRQ_TX_DONE (0x00000004)
#define LR1121_IRQ_RX_DONE (0x00000008)
#define LR1121_IRQ_PREAMBLE_DETECTED (0x00000010)
#define LR1121_IRQ_SYNC_WORD_OR_HEADER_VALID (0x00000020)
#define LR1121_IRQ_HEADER_ERROR (0x00000040)
#define LR1121_IRQ_CRC_ERROR (0x00000080)
#define LR1121_IRQ_CAD_DONE (0x00000100)
#define LR1121_IRQ_CAD_DETECED (0x00000200)
#define LR1121_IRQ_TIMEOUT (0x00000400)
#define LR1121_IRQ_LR_FHSS_HOP (0x00000800)
#define LR1121_IRQ_LOW_BATTERY_DETECTED (0x00200000)
#define LR1121_IRQ_COMMAND_ERROR (0x00400000)
#define LR1121_IRQ_ERROR (0x00800000)
#define LR1121_IRQ_FSK_LENGTH_ERROR (0x01000000)
#define LR1121_IRQ_FSK_ADDRESS_ERROR (0x02000000)
#define LR1121_IRQ_LORA_RX_TIMESTAMP (0x04000000)
#define LR1121_IRQ_ALL (0x07e00ffc)

void lr1121_set_dio_irq_params(uint32_t irq1, uint32_t irq2);

#define LR1121_TCXO_TUNE_1_6V (0x00)
#define LR1121_TCXO_TUNE_1_7V (0x01)
#define LR1121_TCXO_TUNE_1_8V (0x02)
#define LR1121_TCXO_TUNE_2_2V (0x03)
#define LR1121_TCXO_TUNE_2_4V (0x04)
#define LR1121_TCXO_TUNE_2_7V (0x05)
#define LR1121_TCXO_TUNE_3_0V (0x06)
#define LR1121_TCXO_TUNE_3_3V (0x07)

void lr1121_set_tcxo_mode(uint8_t tune, uint32_t timeout);

#define LR1121_STANDBY_RC (0x00)
#define LR1121_STANDBY_XOSC (0x01)

void lr1121_set_standby(uint8_t config);

void lr1121_get_rx_buffer_status(uint8_t *size, uint8_t *offset);

void lr1121_set_rx(uint32_t timeout);

void lr1121_set_tx(uint32_t timeout);

void lr1121_set_rf_frequency(uint32_t frequency);

#define LR1121_PACKET_TYPE_NONE (0x00)
#define LR1121_PACKET_TYPE_GFSK (0x01)
#define LR1121_PACKET_TYPE_LORA (0x02)
#define LR1121_PACKET_TYPE_SIGFOX_UPLINK (0x03)
#define LR1121_PACKET_TYPE_LR_FHSS (0x04)

void lr1121_set_packet_type(uint8_t type);

struct lr1121_lora_modulation_params {
    uint8_t spreading_factor;
    uint8_t bandwidth;
    uint8_t coding_rate;
    uint8_t ldro;
};

#define LR1121_LORA_MODULATION_SPREADING_FACTOR_5 (0x05)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_6 (0x06)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_7 (0x07)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_8 (0x08)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_9 (0x09)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_10 (0x0a)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_11 (0x0b)
#define LR1121_LORA_MODULATION_SPREADING_FACTOR_12 (0x0c)

#define LR1121_LORA_MODULATION_BANDWIDTH_62 (0x03)
#define LR1121_LORA_MODULATION_BANDWIDTH_125 (0x04)
#define LR1121_LORA_MODULATION_BANDWIDTH_250 (0x05)
#define LR1121_LORA_MODULATION_BANDWIDTH_500 (0x06)
#define LR1121_LORA_MODULATION_BANDWIDTH_203 (0x0d)
#define LR1121_LORA_MODULATION_BANDWIDTH_406 (0x0e)
#define LR1121_LORA_MODULATION_BANDWIDTH_812 (0x0f)

#define LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_5 (0x01)
#define LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_6 (0x02)
#define LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_7 (0x03)
#define LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_8 (0x04)
#define LR1121_LORA_MODULATION_CODING_RATE_LONG_4_5 (0x05)
#define LR1121_LORA_MODULATION_CODING_RATE_LONG_4_6 (0x06)
#define LR1121_LORA_MODULATION_CODING_RATE_LONG_4_8 (0x07)

#define LR1121_LORA_MODULATION_LDRO_DISABLED (0x00)
#define LR1121_LORA_MODULATION_LDRO_ENABLED (0x01)

void lr1121_lora_set_modulation_params(struct lr1121_lora_modulation_params *params);

struct lr1121_lora_packet_params {
    uint16_t preamble_length;
    uint8_t header_type;
    uint8_t payload_length;
    uint8_t crc;
    uint8_t iq;
};

#define LR1121_LORA_PACKET_HEADER_TYPE_EXPLICIT (0x00)
#define LR1121_LORA_PACKET_HEADER_TYPE_IMPLICIT (0x01)

#define LR1121_LORA_PACKET_CRC_DISABLED (0x00)
#define LR1121_LORA_PACKET_CRC_ENABLED (0x01)

#define LR1121_LORA_PACKET_IQ_STANDARD (0x00)
#define LR1121_LORA_PACKET_IQ_INVERTED (0x01)

void lr1121_lora_set_packet_params(struct lr1121_lora_packet_params *params);

#define LR1121_TX_RAMP_TIME_16_US (0x00)
#define LR1121_TX_RAMP_TIME_32_US (0x01)
#define LR1121_TX_RAMP_TIME_48_US (0x02)
#define LR1121_TX_RAMP_TIME_64_US (0x03)
#define LR1121_TX_RAMP_TIME_80_US (0x04)
#define LR1121_TX_RAMP_TIME_96_US (0x05)
#define LR1121_TX_RAMP_TIME_112_US (0x06)
#define LR1121_TX_RAMP_TIME_128_US (0x07)
#define LR1121_TX_RAMP_TIME_144_US (0x08)
#define LR1121_TX_RAMP_TIME_160_US (0x09)
#define LR1121_TX_RAMP_TIME_176_US (0x0a)
#define LR1121_TX_RAMP_TIME_192_US (0x0b)
#define LR1121_TX_RAMP_TIME_208_US (0x0c)
#define LR1121_TX_RAMP_TIME_240_US (0x0d)
#define LR1121_TX_RAMP_TIME_272_US (0x0e)
#define LR1121_TX_RAMP_TIME_304_US (0x0f)

void lr1121_set_tx_params(uint8_t power, uint8_t ramp_time);

struct lr1121_pa_config {
    uint8_t pa_selection;
    uint8_t reg_pa_supply;
    uint8_t pa_duty_cycle;
    uint8_t pa_hp_selection;
};

#define LR1121_PA_LOW_POWER_PA (0x00)
#define LR1121_PA_HIGH_POWER_PA (0x01)
#define LR1121_PA_HIGH_FREQUENCY_PA (0x02)

#define LR1121_PA_SUPPLY_INTERNAL_REGULATOR (0x00)
#define LR1121_PA_SUPPLY_VBAT (0x01)

void lr1121_set_pa_config(struct lr1121_pa_config *config);

void lr1121_set_rx_boosted(uint8_t boosted);

#endif
