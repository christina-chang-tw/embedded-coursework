#include "transport_layer.h"

uint8_t src_port;
uint8_t dest_port;

static bool parity_check (uint8_t len, uint8_t* buf)
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
    return parity;
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

    // Compute the checksum
    uint8_t checksum = parity_check(len, buf);

    // Header of the segment
    tx_buf[0] = control[0];
    tx_buf[1] = control[1];
    tx_buf[2] = src_port;
    tx_buf[3] = dest_port;
    tx_buf[4] = len;

    // Data of the segment
    for (int i = 0; i < len; ++i)
    {
        tx_buf[5+i] = buf[i];
    }

    // Checksum of the segment
    tx_buf[5+len] = checksum;

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