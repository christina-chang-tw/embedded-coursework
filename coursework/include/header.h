/* Declaration of all struc */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DEBUG 0

extern bool retransmit_flag;
extern bool receive_flag;

/* App layer wrapper */
typedef struct
{
    uint8_t buf[2];
    uint8_t len;
}al_data_t;

/* Transport layer wrapper */
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


/* Network layer wrapper */
/* Data link layer wrapper */