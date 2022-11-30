/* Declaration of all struc */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DEBUG 1

enum state_machine {app_layer=0, trans_layer=1, net_layer=2, link_layer=3, phy_layer=4, idle=5};

extern enum state_machine state;

extern bool retransmit_flag;
extern bool receive_flag;

typedef struct
{
    uint8_t buf[2];
    uint8_t len;
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
    uint8_t len;
}tl_segment_tx;

typedef struct
{
    al_data_t app;
    uint8_t app_len;
    tl_segment_tx segment;
    uint8_t seg_len;
}tl_receive;



