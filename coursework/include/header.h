/* Declaration of all struc */
#pragma once
#include <stdint.h>
#include <stdbool.h>

extern bool retransmit_flag;

typedef struct
{
    uint8_t buf[2];
}al_data_t;

typedef struct
{
    uint8_t control[2];
    uint8_t src_port;
    uint8_t dest_port;
    uint8_t len;
    uint8_t *data;
    uint8_t checksum[2];
}Segment;

typedef struct
{
    uint8_t *buf;
}tl_segment_tx;

typedef struct
{
    al_data_t app;
    tl_segment_tx segment;
}tl_receive;

