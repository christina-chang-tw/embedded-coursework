#include "../include/transport_layer.h"
#include "../include/uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>


static uint8_t seq_num = 0;
bool retransmit_flag = 0;

static uint16_t sum_checksum (const uint8_t len, const uint8_t* buf)
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
    // retransmit_flag = 1;
    state = net_layer; // just use the previous assembled package
}


tl_segment_tx TL_send (const uint8_t dev, const uint8_t src_port, const uint8_t dest_port, const al_data_t* ap_buf)
{
    increment_seqnum();
    TIMSK2 |= _BV(OCIE1A); /* Enable output compare interrupt A */
    tl_segment_tx tx_buf;
    uint16_t checksum;
    uint8_t len = ap_buf->len;

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

    tx_buf.len = len + 7;

#if(DEBUG)
    for (uint8_t i = 0; i < tx_buf.len ; ++i)
    {
        char buf[5];
        put_str("TL send segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(tx_buf.buf[i], buf, 10));
        put_str("\r\n");
    }
#endif
    return tx_buf;
}

tl_receive TL_receive (const uint8_t dev, const tl_segment_tx* rx_buf)
{
    tl_receive received;
    tl_segment_tx ack;
    uint16_t checksum = 0;
    received.app_len = rx_buf->buf[4];

    // /* Check if the received package is an ACK*/
    // if ((int8_t)rx_buf->buf[1])
    // {
    //     // We know that this is an ACK package, check if it is the correct ACK!
    //     if ((int8_t)rx_buf->buf[0] != seq_num)
    //     {
    //         // Incorrect ACK, What?! Time to go through a long process again
    //         retransmit_flag = 1;
    //         received.segment.buf = NULL;
    //         return received;
    //     } 
    // }

    // /* Check if the checksum is correct */
    // checksum = sum_checksum(rx_buf->len-2, rx_buf->buf);
    // if (rx_buf->buf[rx_buf->len-2] != (uint8_t)(checksum >> 4) || rx_buf->buf[rx_buf->len-1] != (uint8_t)(checksum))
    // {
    //     // Oh no, wrong checksum...
    //     retransmit_flag = 1;
    //     received.segment.buf = NULL;
    //     return received;
    // }

    /* Everything is correct - send an ACK back */
    increment_seqnum();
    ack.buf = (uint8_t *)malloc((received.seg_len)*sizeof(uint8_t));
    ack.buf[0] = seq_num;
    ack.buf[1] = rx_buf->buf[0];
    ack.buf[2] = rx_buf->buf[2];
    ack.buf[3] = rx_buf->buf[3];
    ack.buf[4] = rx_buf->buf[4];
    ack.len = received.app_len + 7;
    for (uint8_t i = 0; i < received.app_len; ++i)
    {
        received.app.buf[i] = rx_buf->buf[i+5];
        ack.buf[i+5] = 0;
    }
    
    // Checksum of the segment
    checksum = sum_checksum(rx_buf->len-2, ack.buf);
    ack.buf[rx_buf->len-2] = (uint8_t)(checksum >> 4);
    ack.buf[rx_buf->len-1] = (uint8_t)checksum;

#if(DEBUG)
    for (uint8_t i = 0; i < ack.len ; ++i)
    {
        char buf[5];
        put_str("TL receive segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(ack.buf[i], buf, 10));
        put_str("\r\n");
    }
#endif

    received.segment = ack;
    return received;
}

void TL_free_buffer(tl_segment_tx *buf)
{
    free(buf);
}