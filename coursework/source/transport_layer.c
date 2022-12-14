#include "../include/transport_layer.h"
#include "../include/uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>


static uint8_t seq_num = 0;
static uint8_t tick = 0;
static uint8_t try_num = 0;

bool tl_retransmit_flag = 0;


typedef struct
{
    size_t length;
    size_t capacity;
    transport *buff;
} transport_vector;

transport_vector create_transport_vector()
{
    return {0, 0, NULL};
}

void push_back(transport_vector *vec, transport obj)
{
    if (len == capacity)
    {
        vec->capacity <<= 1;
        vec->buff = realloc(vec->buff, sizeof(transport)*vec->capacity);
    }
    vec->buff[(vec->length)++] = obj;
}

transport pop_back(transport_vector *vec)
{
    return vec->buff[--(vec->length)];
}

void delete_transport_vector(transport_vector *vec)
{
    free(vec->buff);
}


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
    // max tick = internal f_c / (prescaler * max of 8-bit) 12*10^6/(1024*256)
    // Timeout in transport layer = 1s
    ++ tick;
    if (tick == 46 && try_num < MAXIMUM_TRY_NUM)
    {
        tl_retransmit_flag = 1;
        tick = 0; // reset tick
        ++ try_num;
    }   
}

void TL_reset_attempts()
{
    try_num = 0;
}

void TL_send (const application app, transport *trans)
{
    increment_seqnum();
    uint16_t checksum;
    uint8_t len = app.tx_data.len;

    trans->buf.buf = (uint8_t *)malloc((len+7)*sizeof(uint8_t));

    // Header of the segment
    trans->buf.buf[0] = seq_num;
    trans->buf.buf[1] = 0x00;
    trans->buf.buf[2] = app.src_port;
    trans->buf.buf[3] = app.dest_port;
    trans->buf.buf[4] = len;

    // Data of the segment
    for (int i = 5; i < len+5; ++i)
    {
        trans->buf.buf[i] =  app.tx_data.buf[i-5];
    }

    // Checksum of the segment
    checksum = sum_checksum(len+5, trans->buf.buf);
    trans->buf.buf[5+len] = (uint8_t)(checksum >> 4);
    trans->buf.buf[6+len] = (uint8_t)checksum;
    trans->buf.len = len + 7;
    trans->dev = app.dest_dev;

#if(DEBUG_MSG)
    for (uint8_t i = 0; i < trans->buf.len ; ++i)
    {
        char buf[5];
        put_str("TL send segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(trans->buf.buf[i], buf, 10));
        put_str("\r\n");
    }
    put_str("\r\n");
#endif
}

Status TL_receive (const uint8_t dev, const tl_segment* rx_seg, transport* trans, al_data_t* app_data)
{
    uint16_t checksum = 0;
    uint8_t len = rx_seg->len;
    app_data->len = rx_seg->buf[4];

    /* Handle ACK segment*/
    if ((int8_t)rx_seg->buf[1])
    {
        /* Check the sequence number is correct*/
        if ((int8_t)rx_seg->buf[0] != seq_num)
        {
            return ERROR;
        }

        /* Check if the checksum is correct */
        checksum = sum_checksum(len-2, rx_seg->buf);

        if (rx_seg->buf[len-2] != (uint8_t)(checksum >> 4) || rx_seg->buf[len-1] != (uint8_t)(checksum))
        {
            // Oh no, wrong checksum.put_str("\r\n");..
            return ERROR;
        }

        // Correct ACK, disable the interrupt
        TIMSK2 &= ~_BV(OCIE1A);
        return SUCCESS;
    }

    /* Handle other segments */
    checksum = sum_checksum(len-2, rx_seg->buf);

    if (rx_seg->buf[len-2] != (uint8_t)(checksum >> 4) || rx_seg->buf[len-1] != (uint8_t)(checksum))
    {
        // Oh no, wrong checksum... wait for the other end to time out
        return ERROR;
    }

    increment_seqnum();
    trans->buf.buf = (uint8_t *)malloc((len)*sizeof(uint8_t));
    trans->buf.buf[0] = seq_num;
    trans->buf.buf[1] = rx_seg->buf[0];
    trans->buf.buf[2] = rx_seg->buf[2];
    trans->buf.buf[3] = rx_seg->buf[3];
    trans->buf.buf[4] = rx_seg->buf[4];
    trans->buf.len = app_data->len + 7;
    for (uint8_t i = 0; i < app_data->len; ++i)
    {
        app_data->buf[i] = rx_seg->buf[i+5];
        trans->buf.buf[i+5] = 0;
    }
    
    // Checksum of the segment
    checksum = sum_checksum(len-2, trans->buf.buf);
    trans->buf.buf[len-2] = (uint8_t)(checksum >> 4);
    trans->buf.buf[len-1] = (uint8_t)checksum;
    trans->ack_flag = 1;
    trans->dev = dev;

#if(DEBUG_MSG)    
    for (uint8_t i = 0; i < trans->buf.len ; ++i)
    {
        char buf[5];
        put_str("TL receive segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(trans->buf.buf[i], buf, 10));
        put_str("\r\n");
    }
    put_str("\r\n");
#endif
    return SUCCESS;
}

void TL_free_buffer(tl_segment *buf)
{
    free(buf->buf);
    free(buf);
}