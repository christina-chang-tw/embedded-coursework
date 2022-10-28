#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include the library for the RFM12 module and the UART
#include "rfm12.h"
#include "tick.h"

#include "librfm12/src/include/rfm12_hw.h"
#include "librfm12/src/include/rfm12_defaults.h"
#include "librfm12/src/include/rfm12_core.h"
#include "librfm12/src/include/rfm12_spi.c"
#include "librfm12/src/include/rfm12_spi_linux.c"


void csma_0_persistent()
{
    uint16_t status;

    if (ctrl.rfm12_state != STATE_RX_IDLE) return;

    RFM12_INT_OFF();
    status = rfm12_read(RFM12_CMD_STATUS);
    RFM12_INT_ON();

    //check if we see a carrier
    while (status & RFM12_STATUS_RSSI)
    {
        _delay_ms(200); //if yes, then wait and check again
        RFM12_INT_OFF();
        status = rfm12_read(RFM12_CMD_STATUS);
        RFM12_INT_ON();
    }

    if (ctrl.txstate == STATUS_OCCUPIED)
        rfm12_start_tx();
}

void csma_1_persistent()
{
    uint16_t status;

    if (ctrl.rfm12_state != STATE_RX_IDLE) return;

    RFM12_INT_OFF();
    status = rfm12_read(RFM12_CMD_STATUS);
    RFM12_INT_ON();

    //check if we see a carrier
    while (status & RFM12_STATUS_RSSI)
    {
        _delay_ms(200); //if yes, then wait and check again
        RFM12_INT_OFF();
        status = rfm12_read(RFM12_CMD_STATUS);
        RFM12_INT_ON();
    }

    if (ctrl.txstate == STATUS_OCCUPIED)
        rfm12_start_tx();
}

void csma_p_persistent()
{
    uint16_t status;

    if (ctrl.rfm12_state != STATE_RX_IDLE) return;

    RFM12_INT_OFF();
    status = rfm12_read(RFM12_CMD_STATUS);
    RFM12_INT_ON();

    //check if we see a carrier
    while (status & RFM12_STATUS_RSSI)
    {
        _delay_ms(200); //if yes, then wait and check again
        RFM12_INT_OFF();
        status = rfm12_read(RFM12_CMD_STATUS);
        RFM12_INT_ON();
    }

    if (ctrl.txstate == STATUS_OCCUPIED)
        rfm12_start_tx();
}