#include "uart.h"
#include "application_layer.h"

#include <util/delay.h>
#include <stdbool.h>

#define INT_DDR DDRD
#define INT_PORT PORTD
#define INT_PIN PIND
#define INT0_BIT 2
#define INT1_BIT 3

#define INPUT_DDR DDRA
#define INPUT_PORT PORTA
#define INPUT_PIN PINA
#define INPUT0_BIT 0
#define INPUT1_BIT 1
#define INPUT2_BIT 2

#define DEVICE_NUMBER 1


static bool ctrl1_state=0, ctrl2_state=0;

static uint16_t read_adc()
{
    ADCSRA |= _BV(ADSC);
    return ADC;
}

static void timeout_setup()
{ 
    // Enable timer 1
    TCCR1A = _BV(COM1A0) | _BV(WGM10);
    TCCR1B = _BV(WGM13) | _BV(CS11);
}


static void set_pwm(uint8_t value)
{
    OCR1A = value;
    TIMSK2 |= _BV(OCIE2A);
}


static void ext_interrupt_setup(int num)
{
    switch (num)
    {
    case 0: 
        EIMSK |= _BV(INT0); // Enable INT0 interrupt mask
        EICRA |= _BV(ISC01) | _BV(ISC00); // enable the interrupt on falling edge
        break;
    case 1:
        EIMSK |= _BV(INT1); // Enable INT1 interrupt mask
        EICRA |= _BV(ISC11) | _BV(ISC10); // enable the interrupt on falling edge
        break;
    case 2:
        EIMSK |= _BV(INT2); // Enable INT2 interrupt mask
        EICRA |= _BV(ISC21) | _BV(ISC20); // enable the interrupt on falling edge
        break;
    }
}


ISR (INT0_vect)
{
    uint8_t src_port, dest_port;
    uint8_t data[2];
    uint16_t value;
    
    src_port = 0 + DEVICE_NUMBER;
    dest_port = 3; // need to be changed
    value = read_adc();

    data[0] = value >> 8;
    data[1] = value;

    TL_transmit(src_port, dest_port, sizeof(data)/sizeof(uint8_t), data);
}


ISR (INT1_vect)
{
    uint8_t src_port, dest_port;
    uint8_t data[2];
    uint16_t value;
    
    src_port = 1 + DEVICE_NUMBER;
    dest_port = 3; // need to be changed
    value = read_adc();

    data[0] = value >> 8;
    data[1] = value;

    TL_transmit(src_port, dest_port, sizeof(data)/sizeof(uint8_t), data);
}


void AL_device_setup()
{
    init_uart0();    //init uart
    sei(); // enable interrupt

    /* For interrupt */
    INT_DDR = 0x00;
    INT_PORT |= _BV(INT0_BIT) | _BV(INT1_BIT); // set up pull-up resistors
    ext_interrupt_setup(0);
    ext_interrupt_setup(1);

    /* For GPIO input */
    INPUT_DDR &= ~ _BV(INPUT0_BIT);
    INPUT_PORT |= _BV(INPUT0_BIT);

    TL_setup();
}