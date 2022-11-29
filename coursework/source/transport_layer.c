#include "../include/transport_layer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

static uint8_t seq_num = 0;
bool retransmit_flag = 0;

static uint16_t sum_checksum (uint8_t len, const uint8_t* buf)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; ++i)
    {
        sum += buf[i];
    }
    return sum;
}

static void increment_seqnum()
{
    if (seq_num >= 256 || seq_num == 0)
    {
        seq_num = 1;
    } else
    {
        ++seq_num;
    }
}

ISR(TIMER2_COMPA_vect)
{
    retransmit_flag = 1;
}


tl_segment_tx TL_send (uint8_t dev, uint8_t src_port, uint8_t dest_port, uint8_t len, al_data_t* ap_buf)
{
    increment_seqnum();
    TIMSK2 |= _BV(OCIE1A); /* Enable output compare interrupt A */
    uint8_t tick, state;
    tl_segment_tx tx_buf;
    uint16_t checksum;

    tx_buf.buf = (uint8_t *)malloc((len+7)*sizeof(uint8_t));

    // Header of the segment
    tx_buf.buf[0] = seq_num;
    tx_buf.buf[1] = 0x00;
    tx_buf.buf[2] = src_port;
    tx_buf.buf[3] = dest_port;
    tx_buf.buf[4] = len;

    // Data of the segment
    for (int i = 5; i < len+5; ++i)
    {
        tx_buf.buf[i] =  ap_buf->buf[i-5];
    }

    // Checksum of the segment
    checksum = sum_checksum(len+5, tx_buf.buf);
    tx_buf.buf[5+len] = (uint8_t)(checksum >> 4);
    tx_buf.buf[6+len] = (uint8_t)checksum;
    return tx_buf;
}


tl_receive TL_receive (uint8_t dev, uint8_t len, tl_segment_tx* rx_buf)
{
    tl_receive received;
    tl_segment_tx ack;
    uint16_t checksum = 0;
    uint8_t app_length = len-7;

    /* Check if the received package is an ACK*/
    if ((int8_t)rx_buf->buf[1])
    {
        // We know that this is an ACK package, check if it is the correct ACK!
        if ((int8_t)rx_buf->buf[0] != seq_num)
        {
            // Incorrect ACK, What?! Time to go through a long process again
            retransmit_flag = 1;
        } 
        return;
    }

    /* Check if the checksum is correct */
    checksum = sum_checksum(len-2, rx_buf->buf);
    if (rx_buf->buf[len-2] != (uint8_t)(checksum >> 4) || rx_buf->buf[len-1] != (uint8_t)(checksum))
    {
        // Oh no, wrong checksum...
        retransmit_flag = 1;
        return;
    }

    /* Everything is correct - send an ACK back */
    increment_seqnum();
    ack.buf = (uint8_t *)malloc((len)*sizeof(uint8_t));
    ack.buf[0] = seq_num;
    ack.buf[1] = rx_buf->buf[0];
    ack.buf[2] = rx_buf->buf[2];
    ack.buf[3] = rx_buf->buf[3];
    ack.buf[4] = rx_buf->buf[4];
    for (uint8_t i = 5; i < app_length+5; ++i)
    {
        received.app.buf[i-5] = ack.buf[i] = rx_buf->buf[i-5];
    }
    
    // Checksum of the segment
    checksum = sum_checksum(len-2, ack.buf);
    ack.buf[len-2] = (uint8_t)(checksum >> 4);
    ack.buf[len-1] = (uint8_t)checksum;

    received.segment = ack;
    return received;
}

void TL_free_buffer(tl_segment_tx *buf)
{
    free(buf);
}