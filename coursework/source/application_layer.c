#include "../include/uart.h"
#include "../include/header.h"
#include "../include/hal.h"
#include "../include/transport_layer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define DEVICE_NUMBER 0

static uint8_t src_port, dest_port, dest_dev;
static al_data_t app_data;
static bool transmit_flag = 0;

bool receive_flag = 0;

ISR (INT0_vect)
{
    src_port = 0; // button 0
    dest_port = 2;
#if (DEVICE_NUMBER == 0)
    dest_dev = 1;
#elif (DEVICE_NUMBER == 1)
    dest_dev = 0;
#elif (DEVICE_NUMBER == 2)
    dest_dev = 1;
#endif
    app_data = read_adc();
    transmit_flag = 1;
}


ISR (INT1_vect)
{
    src_port = 1; // button 1
    dest_port = 2;
#if (DEVICE_NUMBER == 0)
    dest_dev = 2;
#elif (DEVICE_NUMBER == 1)
    dest_dev = 2;
#elif (DEVICE_NUMBER == 2)
    dest_dev = 0;
#endif
    app_data = read_adc();
    transmit_flag = 1;
}

int main()
{
    device_setup();
    tl_segment_tx trans_buf;
    tl_receive trans_rxdata;

    while(1)
    {
        // Dealing with transmitting data
        if (transmit_flag)
        {
            trans_buf = TL_send(dest_dev, src_port, dest_port, &app_data);
            
            // end of the transmission
            transmit_flag = 0;
        }

        // Dealing with packet re-transmission
        if (retransmit_flag)
        {
            retransmit_flag = 0;
        }


        // Dealing with the received package
        if (receive_flag)
        {
            // Data is received, start going up the layers
            trans_rxdata = TL_receive(dest_dev, &trans_buf);
            if (!trans_rxdata.segment.buf)
            {
                // Nothing in the buffer, something goes wrong
                // wait for the timeout of the sender.
            }
            // Need to decide if send an ACK first or set the app data first. 
            // Also, still need to do crash recovery. (How to do it?)
            set_pwm(trans_rxdata.app);

            // end of the receiving mode
            receive_flag = 0;
        }

    }
}

