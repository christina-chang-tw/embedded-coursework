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
static tl_segment_tx buf;
static al_data_t app_data;

void device_setup()
{
    init_uart0();    //init uart
    sei(); // enable interrupt

    /* For interrupt */
    INT_DDR = 0x00;
    INT_PORT |= _BV(INT0_BIT) | _BV(INT1_BIT); // set up pull-up resistors
    ext_interrupt_setup(0);
    ext_interrupt_setup(1);

    init_adc();
    init_pwm();
    timeout_setup();
}

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
}


ISR (INT1_vect)
{
#if (DEVICE_NUMBER == 0)
    dest_dev = 2;
#elif (DEVICE_NUMBER == 1)
    dest_dev = 2;
#elif (DEVICE_NUMBER == 2)
    dest_dev = 0;
#endif
    app_data = read_adc();
}

int main()
{
    device_setup();
    while(1);
}

