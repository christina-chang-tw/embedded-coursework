#include <interrupt.h>

static bool ctrl1_state, ctrl2_state;

ISR (s_vect)
{
    uint8_t ctrl1 = (PIND & _BV(2)), ctrl2 = (PIND & _BV(3));

    // If the PD2 read high, the control button 1 is pressed.
    if (ctrl1)
    {
        ctrl1_state = ~ctrl1_state;

        // Send data down to TL to change the state of LED
    }

    // If the PD3 read high, the control button 1 is pressed.
    if (ctrl2)
    {
        ctrl2_state = ~ctrl2_state;

        // Send data down to TL to change the state of LED
    }
}


void AL_task_handler()
{
    sei(); // enable interrupt

    DDRD = 0x00;
    PORTD = 0xFF;
    EIMSK |= _BV(INT0);
}