// Transport Layer
# pragma once

#include "../include/header.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXIMUM_TRY_NUM 10

void TL_send (const application app, transport *trans);
Status TL_receive (const uint8_t dev, const tl_segment* rx_seg, transport* trans, al_data_t* app_data);
void TL_free_buffer(tl_segment *buf);
void TL_reset_attempts();
// Trasport Layer