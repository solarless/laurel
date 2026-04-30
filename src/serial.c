#include "serial.h"

#include "led.h"
#include "platform.h"
#include "radio.h"

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/usart.h>

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE (1024)

#define FEND (0xc0)
#define FESC (0xdb)
#define TFEND (0xdc)
#define TFESC (0xdd)

static enum {
    STATE_GARBAGE,
    STATE_FRAME,
    STATE_DATA,
    STATE_ESCAPE,
} state;

uint8_t txbuffer[BUFFER_SIZE];
static uint8_t rxdata[BUFFER_SIZE];
static uint8_t rxcommand;
static uint16_t rxsize;
static bool rxdone;

void handle_uart_rx(void);
void handle_uart_dma_complete(void);

void serial_send(uint8_t *data, uint16_t length)
{
    int p = 0x00;
    txbuffer[p++] = FEND;
    txbuffer[p++] = 0x00;  /* Data frame */
    for (int i = 0; i < length; i++) {
        if (data[i] == FEND) {
            txbuffer[p++] = FESC;
            txbuffer[p++] = TFEND;
        } else if (data[i] == FESC) {
            txbuffer[p++] = FESC;
            txbuffer[p++] = TFESC;
        } else {
            txbuffer[p++] = data[i];
        }
    }
    txbuffer[p++] = FEND;

    dma_set_number_of_data(CH340X_DMA, CH340X_DMA_CHANNEL, p);
    dma_enable_channel(CH340X_DMA, CH340X_DMA_CHANNEL);

    for (int i = 0; i < p; i++)
        usart_send_blocking(CH340X_USART, txbuffer[i]);
}

void serial_process(void)
{
    if (!rxdone)
        return;

    if (rxcommand == 0x00)
        radio_send(rxdata, rxsize);
    rxsize = 0;
    rxdone = false;
}

void handle_uart_rx(void)
{
    uint8_t byte = usart_recv(CH340X_USART);

    switch (state) {
    case STATE_GARBAGE:
        if (byte == FEND)
            state = STATE_FRAME;
        break;

    case STATE_FRAME:
        rxcommand = byte;
        state = STATE_DATA;
        break;

    case STATE_DATA:
        if (byte == FEND) {
            rxdone = true;
            state = STATE_GARBAGE;
        } else if (byte == FESC) {
            state = STATE_ESCAPE;
        } else {
            rxdata[rxsize++] = byte;
        }
        break;

    case STATE_ESCAPE:
        if (byte == TFEND) {
            rxdata[rxsize++] = FEND;
            state = STATE_DATA;
        } else if (byte == TFESC) {
            rxdata[rxsize++] = FESC;
            state = STATE_DATA;
        } else {
            rxsize = 0;
            state = STATE_GARBAGE;
        }
        break;
    }
}

void handle_uart_dma_complete(void)
{
    dma_disable_channel(CH340X_DMA, CH340X_DMA_CHANNEL);
}
