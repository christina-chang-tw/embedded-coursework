#include "transport_layer.h"

uint8_t src_port;
uint8_t dest_port;

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

void TL_socket()
{
    // Save the source/destination port information
    // src_port = 
    // dest_port = 
}

uint8_t TL_transmit (uint8_t* control, uint8_t len, uint8_t* buf)
{
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

    // Send the data to the Network Layer at constant rate.

    return state;

}

void TL_receive (uint8_t *len, uint8_t* rx_buf)
{
    uint8_t total_len;
    // Receive the data from the Network Layer


    // Check if no corruption in the received segment
    *len = rx_buf[4];


    // Start extracting data
    

}