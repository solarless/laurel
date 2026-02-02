#include <stddef.h>

#include <libopencm3/stm32/usart.h>

#include "pinout.h"
#include "serial.h"
#include "systick.h"

void serial_send(uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        usart_send_blocking(CH340X_USART, data[i]);
    }
}

enum {
    MODE_TIMEOUT,
    MODE_CRLF,
};

static uint8_t data[1024];
static uint16_t data_size;

static uint8_t byte_received_flag;
static uint32_t byte_received_timestamp;

static uint8_t mode;
static serial_received_handler received_handler;

void serial_receive_timeout(serial_received_handler handler)
{
    data_size = 0;
    mode = MODE_TIMEOUT;
    received_handler = handler;
}

void serial_receive_crlf(serial_received_handler handler)
{
    data_size = 0;
    mode = MODE_CRLF;
    received_handler = handler;
}

#define FRAME_TIMEOUT_MS (10)

static void timeout_process(void)
{
    uint32_t now = systick_get_counter();
    if (now - byte_received_timestamp >= FRAME_TIMEOUT_MS) {
        if (received_handler == NULL) {
            return;
        }

        received_handler(data, data_size);
        data_size = 0;
        byte_received_flag = 0;
    }
}

static void crlf_process(void)
{
    if (data_size < 2) {
        return;
    }

    if (data[data_size - 2] == '\r' && data[data_size - 1] == '\n') {
        if (received_handler == NULL) {
            return;
        }

        received_handler(data, data_size - 2);
        data_size = 0;
        byte_received_flag = 0;
    }
}

void serial_process(void)
{
    if (!byte_received_flag) {
        return;
    }

    switch (mode) {
    case MODE_TIMEOUT:
        timeout_process();
        break;
    case MODE_CRLF:
        crlf_process();
        break;
    }
}

void usart1_isr(void)
{
    byte_received_flag = 1;
    byte_received_timestamp = systick_get_counter();
    data[data_size++] = usart_recv(CH340X_USART);
}
