#include "../include/uart.h"
#include "../include/header.h"
#include "../include/hal.h"
#include "../include/transport_layer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define DEVICE_NUMBER 0

static application app;
static transport trans;
bool receive_flag = 0;

ISR (INT0_vect)
{
    app.src_port = 0; // button 0
    app.dest_port = 2;
#if (DEVICE_NUMBER == 0)
    app.dest_dev = 1;
#elif (DEVICE_NUMBER == 1)
    app.dest_dev = 0;
#elif (DEVICE_NUMBER == 2)
    app.dest_dev = 1;
#endif
    app.tx_data = read_adc();
    app.transmit_flag = 1;
}


ISR (INT1_vect)
{
    app.src_port = 1; // button 1
    app.dest_port = 2;
#if (DEVICE_NUMBER == 0)
    app.dest_dev = 2;
#elif (DEVICE_NUMBER == 1)
    app.dest_dev = 2;
#elif (DEVICE_NUMBER == 2)
    app.dest_dev = 0;
#endif
    app.tx_data = read_adc();
    app.transmit_flag = 1;
}


int main()
{
    device_setup();
    tl_segment rx;
    Status status;
    uint8_t dest_dev;
    
    while(1)
    {
        // Dealing with transmitting data
        if (app.transmit_flag)
        {
            TL_send(app, &trans);
            
            // end of the transmission
            app.transmit_flag = 0;
        }

        // Dealing with packet re-transmission and acknowledgement
        // Only start from transport layer downward with already assembled segment
        if (tl_retransmit_flag || trans.ack_flag)
        {
            tl_retransmit_flag = 0;
        }


        // Dealing with the received package
        if (receive_flag)
        {
            // Data is received, start going up the layers
            status = TL_receive(dest_dev, &rx, &trans, &app.rx_data);
            if (status == SUCCESS)
            {
                set_pwm(app.rx_data);
            }

            // Need to decide if send an ACK first or set the app data first. 
            // Also, still need to do crash recovery. (How to do it?)
            

            // end of the receiving mode
            receive_flag = 0;
        }

    }
}

