// Transport Layer
# pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct 
{
    uint8_t control[2];
    uint8_t src_port;
    uint8_t dest_port;
    uint8_t len;
    uint8_t *data;
    uint8_t checksum[2];

}Segment;

uint8_t TL_transmit (uint8_t* control, uint8_t len, uint8_t* buf);
void TL_socket ();
void TL_receive(uint8_t* len, uint8_t* rx_buffer);

// Trasport Layer