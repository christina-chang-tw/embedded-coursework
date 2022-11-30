// Transport Layer
# pragma once

#include "../include/header.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


tl_segment_tx TL_send (const uint8_t dev, const uint8_t src_port, const uint8_t dest_port, const al_data_t* ap_buf);
void TL_socket ();
tl_receive TL_receive (const uint8_t dev, const tl_segment_tx* rx_buf);
void TL_serialize(tl_segment_tx* tx_buf, Segment *seg);
void TL_free_buffer(tl_segment_tx *buf);
void TL_retransmission();
// Trasport Layer