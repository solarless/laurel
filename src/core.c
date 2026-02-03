#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core.h"
#include "lr1121.h"
#include "radio.h"
#include "serial.h"
#include "systick.h"

static void transparent_serial_received_handler(uint8_t *data, uint32_t size);
static void transparent_radio_received_handler(uint8_t *data, uint8_t size);
static void transparent_radio_sent_handler(void);
static void enter_transparent_mode(void);
static void transparent_process(void);

static void command_serial_received_handler(uint8_t *data, uint32_t size);
static void enter_commmand_mode(void);
static void command_process(void);

enum {
    MODE_TRANSPARENT,
    MODE_COMMAND,
};

static uint8_t mode;

static bool switch_sequence_started;
static uint32_t switch_sequence_start_timestamp;

#define SWITCH_SEQUENCE_TIMEOUT_MS (1000)

static void transparent_serial_received_handler(uint8_t *data, uint32_t size)
{
    switch_sequence_started = false;

    if (size == 5 && memcmp(data, "+++\r\n", 3) == 0) {
        switch_sequence_started = true;
        switch_sequence_start_timestamp = systick_get_counter();
        return;
    }

    radio_send(data, size, transparent_radio_sent_handler);
}

static void transparent_radio_received_handler(uint8_t *data, uint8_t size)
{
    serial_send(data, size);
    radio_receive(transparent_radio_received_handler);
}

static void transparent_radio_sent_handler(void)
{
    radio_receive(transparent_radio_received_handler);
}

static void enter_transparent_mode(void)
{
    mode = MODE_TRANSPARENT;

    serial_receive_timeout(transparent_serial_received_handler);
    radio_receive(transparent_radio_received_handler);
}

static void transparent_process(void)
{
    if (switch_sequence_started) {
        uint32_t now = systick_get_counter();
        if (now - switch_sequence_start_timestamp >= SWITCH_SEQUENCE_TIMEOUT_MS) {
            switch_sequence_started = false;
            enter_commmand_mode();
        }
    }
}

/* ================================================================ */

static void command_serial_received_handler(uint8_t *data, uint32_t size)
{
    if (memcmp(data, "AT", 2) != 0) {
        printf("\r\nERROR\r\n");
        return;
    }

    data += 2;
    size -= 2;

    if (memcmp(data, "O", 1) == 0) {
        lr1121_lora_set_modulation_params(&radio_modulation_params);
        lr1121_lora_set_packet_params(&radio_packet_params);
        enter_transparent_mode();
    }

    // Spreading factor

    else if (memcmp(data, "+SF?", 4) == 0) {
        printf("Spreading factor = %02x\r\n", radio_modulation_params.spreading_factor);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+SF=", 4) == 0) {
        data += 0x04;
        int sf;
        if (sscanf((char *)data, "%x", &sf) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (sf < LR1121_LORA_MODULATION_SPREADING_FACTOR_5 || sf > LR1121_LORA_MODULATION_SPREADING_FACTOR_12) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_modulation_params.spreading_factor = sf;
        printf("\r\nOK\r\n");
    }

    // Bandwidth

    else if (memcmp(data, "+BW?", 4) == 0) {
        printf("Bandwidth = %02x\r\n", radio_modulation_params.bandwidth);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+BW=", 4) == 0) {
        data += 0x04;
        int bw;
        if (sscanf((char *)data, "%x", &bw) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (bw < LR1121_LORA_MODULATION_BANDWIDTH_62 || bw > LR1121_LORA_MODULATION_BANDWIDTH_500) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_modulation_params.bandwidth = bw;
        printf("\r\nOK\r\n");
    }

    // Coding rate

    else if (memcmp(data, "+CR?", 4) == 0) {
        printf("Coding rate = %02x\r\n", radio_modulation_params.coding_rate);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+CR=", 4) == 0) {
        data += 0x04;
        int cr;
        if (sscanf((char *)data, "%x", &cr) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (cr < LR1121_LORA_MODULATION_CODING_RATE_SHORT_4_5 || cr > LR1121_LORA_MODULATION_CODING_RATE_LONG_4_8) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_modulation_params.coding_rate = cr;
        printf("\r\nOK\r\n");
    }

    // LDRO

    else if (memcmp(data, "+LDRO?", 6) == 0) {
        printf("LDRO = %02x\r\n", radio_modulation_params.ldro);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+LDRO=", 6) == 0) {
        data += 0x06;
        int ldro;
        if (sscanf((char *)data, "%x", &ldro) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (ldro != LR1121_LORA_MODULATION_LDRO_DISABLED && ldro != LR1121_LORA_MODULATION_LDRO_ENABLED) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_modulation_params.ldro = ldro;
        printf("\r\nOK\r\n");
    }

    // CRC

    else if (memcmp(data, "+CRC?", 5) == 0) {
        printf("CRC = %02x\r\n", radio_packet_params.crc);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+CRC=", 5) == 0) {
        data += 0x05;
        int crc;
        if (sscanf((char *)data, "%x", &crc) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (crc != LR1121_LORA_PACKET_CRC_DISABLED && crc != LR1121_LORA_PACKET_CRC_ENABLED) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_packet_params.crc = crc;
        printf("\r\nOK\r\n");
    }

    // IQ

    else if (memcmp(data, "+IQ?", 4) == 0) {
        printf("IQ = %02x\r\n", radio_packet_params.iq);
        printf("\r\nOK\r\n");
    }

    else if (memcmp(data, "+IQ=", 4) == 0) {
        data += 0x04;
        int iq;
        if (sscanf((char *)data, "%x", &iq) != 1) {
            printf("\r\nERROR\r\n");
            return;
        }
        if (iq != LR1121_LORA_PACKET_IQ_STANDARD && iq != LR1121_LORA_PACKET_IQ_INVERTED) {
            printf("Invalid value\r\n");
            printf("\r\nERROR\r\n");
            return;
        }
        radio_packet_params.iq = iq;
        printf("\r\nOK\r\n");
    }

    else {
        printf("\r\nERROR\r\n");
    }
}

static void enter_commmand_mode(void)
{
    mode = MODE_COMMAND;

    serial_receive_crlf(command_serial_received_handler);
    radio_idle();

    printf("Entered command mode\r\n");
}

static void command_process(void)
{
}

void core_setup(void)
{
    enter_transparent_mode();
}

void core_process(void)
{
    switch (mode) {
    case MODE_TRANSPARENT:
        transparent_process();
        break;
    case MODE_COMMAND:
        command_process();
        break;
    }
}
