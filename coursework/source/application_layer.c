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
static transport trans[20];
static uint8_t current_index = 0; // current seg that is being used
static uint8_t last_index = 0; // the last seg that is being put onto


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
    transport rx_trans;
    tl_segment rx;
    Status status;
    uint8_t dest_dev = 0;

    rx.buf[0] = 1;
    rx.buf[1] = 1;
    rx.buf[2] = 2;
    rx.buf[3] = 0;
    rx.buf[4] = 2;
    rx.buf[5] = 0;
    rx.buf[6] = 0;
    rx.buf[7] = 0;
    rx.buf[8] = 6;
    rx.len = 9;
    
    while(1)
    {
        // Dealing with transmitting data
        if (app.transmit_flag)
        {
            TL_send(app, &trans[last_index]);
            ++last_index;

            // increase the current index
            ++current_index;
            
            // end of the transmission
            app.transmit_flag = 0;
        }

        // Dealing with packet re-transmission and acknowledgement
        // Only start from transport layer downward with already assembled segment
        if (tl_retransmit_flag || trans[current_index].ack_flag)
        {
            put_str("retransmitting data ...\r\n");
            tl_retransmit_flag = 0;
        }


        // Dealing with the received package
        if (receive_flag)
        {
            // Data is received, start going up the layers
            status = TL_receive(dest_dev, &rx, &rx_trans, &app.rx_data);

            // Currently setting the app first
            if (status == SUCCESS)
            {
                set_pwm(app.rx_data);
            }

            // end of the receiving mode
            receive_flag = 0;
        }

    }
}

