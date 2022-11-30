#include "../include/uart.h"
#include "../include/header.h"
#include "../include/hal.h"
#include "../include/transport_layer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define DEVICE_NUMBER 0

static bool ctrl1_state=0, ctrl2_state=0;
static uint8_t src_port, dest_port, dest_dev;
static al_data_t app_data;
static bool transmit_flag = 0;

bool receive_flag = 0;
enum state_machine state = app_layer;

ISR (INT0_vect)
{
    ctrl1_state = ~ctrl1_state;
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
    state = app_layer;
}


ISR (INT1_vect)
{
    ctrl2_state = ~ctrl2_state;
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
        // Dealing with the transmit side
        if (transmit_flag || retransmit_flag)
        {
            trans_buf = TL_send(dest_dev, src_port, dest_port, &app_data);
            
            // end of the transmission
            transmit_flag = 0;
            retransmit_flag = 0;
            
        }

        if (receive_flag)
        {
            // Data is received, start going up the layers
            trans_rxdata = TL_receive(dest_dev, &trans_buf);

            // Need to decide if send an ACK first or set the app data first. 
            // Also, still need to do crash recovery. (How to do it?)
            set_pwm(trans_rxdata.app);

            // end of the receiving mode
            receive_flag = 0;
        }

    }
}

