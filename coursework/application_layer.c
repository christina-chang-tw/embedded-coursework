#include "uart.h"
#include "application_layer.h"

#include <util/delay.h>
#include <stdbool.h>

#define NUM_DDR DDRD
#define INT_PORT PORTD


static bool ctrl1_state=0, ctrl2_state=0;

ISR (INT0_vect)
{
    uint8_t ctrl1 = (PIND & _BV(2)) >> 2;
    put_str("In INT0 Loop\r\n");

    // If the PD2 read high, the control button 1 is pressed.
    if (ctrl1)
    {
        ctrl1_state = ~ctrl1_state;
        put_str("Ctr1 is ");
        put_ch((char) (ctrl1+48));
        put_str("\r\n");
        // Send data down to TL to change the state of LED
    }
}

ISR (INT1_vect)
{
    uint8_t ctrl2 = (PIND & _BV(3)) >> 3;
    put_str("In Interrupt Loop\r\n");

    // If the PD3 read high, the control button 1 is pressed.
    if (ctrl2)
    {
        ctrl2_state = ~ctrl2_state;
        put_str("Ctr2 is ");
        put_ch((char) (ctrl2+48));
        // Send data down to TL to change the state of LED
    }
}

static void interrupt_setup(int num)
{
    if (num == 0)
    {
        EIMSK |= _BV(INT0); // Enable INT0 interrupt mask
        EICRA |= _BV(ISC01) | _BV(ISC00); // enable the interrupt on falling edge
    }
    else if (num == 1)
    {
        EIMSK |= _BV(INT1); // Enable INT1 interrupt mask
        EICRA |= _BV(ISC11) | _BV(ISC10); // enable the interrupt on falling edge
    }
    else if (num == 2)
    {
        EIMSK |= _BV(INT2); // Enable INT2 interrupt mask
        EICRA |= _BV(ISC21) | _BV(ISC20); // enable the interrupt on falling edge
    }
}


int main()
{
    
    init_uart0();    //init uart
    sei(); // enable interrupt

    _delay_ms(10);
    put_str("In main\r\n");
    NUM_DDR = 0x00;
    INT_PORT |= _BV(2) | _BV(3);
    interrupt_setup(0);
    interrupt_setup(1);

    while(1);
    
}