#include "transport_layer.h"

#include <util/delay.h>
#include <stdbool.h>

static Segment segment_buffer[20];
static seq_num = 0;

static void parity_check (uint8_t *checksum, uint8_t len, uint8_t* buf)
{
    bool parity = 0;
    uint8_t temp;
    for (int i = 0; i < len; ++i)
    {
        temp = buf[i];
        while (temp)
        {
            parity = ~parity;
            temp = temp & (temp - 1);
        }
    }
    
    checksum[0] = 0;
    checksum[1] = parity;
}



static void timeout_setup()
{
    TCCR2A |= _BV(WGM21);
    TCCR2B |= _BV(CS22);
    OCR2A = 25156 // timeout = 3s
}


void TL_setup()
{
    timeout_setup()
}

uint8_t TL_send (uint8_t dev, uint8_t src_port, uint8_t dest_port, uint8_t len, uint8_t* buf)
{
    TIMSK2 |= _BV(OCIE1A); /* Enable output compare interrupt A */
    uint8_t tx_buf[len+7], tick, state;
    Segment *segment;
    uint8_t checksum[2];

    // Compute the checksum
    parity_check(checksum, len, buf);

    // Header of the segment
    segment->control[0] = control[0];
    segment->control[1] = control[1];
    segment->src_port = src_port;
    segment->dest_port = dest_port;
    segment->len = len;

    // Data of the segment
    segment->data = (uint8_t *)malloc(len*sizeof(uint8_t));
    for (int i = 0; i < len; ++i)
    {
        segment->data[i] = buf[i];
    }

    // Checksum of the segment
    segment->checksum[0] = checksum[0];
    segment->checksum[1] = checksum[1];
}


void TL_retransmission(int seq_num)
{
    // retransmit the segment at that sequence number
}


void TL_receive (uint8_t dev, uint8_t *len, uint8_t* rx_buf)
{
    Segment *segment;

    if (rx_buf[1])
    {
        // We know that this is an ACK package, don't send an ACK back!
    } else 
    {
        segment->control[0] = seq_num; 
        segment->control[1] = rx_buf[0]; // contain the incoming sequence number
        segment->dest_port = rx_buf[2];
        segment->src_port = rx_buf[3]; // swap dest and src ports around
        segment->len = 0;
        
    }

}


void TL_serialize(uint8_t* tx_buf, Segment *seg)
{
    uint8_t data_length = seg->len;
    tx_buf = (uint8_t *)malloc((data_length+7)*sizeof(uint8_t));

    tx_buf[0] = seg->control[0];
    tx_buf[1] = seg->control[1];
    tx_buf[2] = seg->src_port;
    tx_buf[3] = seg->dest_port;
    tx_buf[4] = data_length;
    
    for (int i = 5; i < data_length + 5; ++i)
    {
        tx_buf[i] = seg->data[i-5];
    }

    tx_buf[5 + data_length] = seg->checksum[0];
    tx_buf[6 + data_length] = seg->checksum[1];
}