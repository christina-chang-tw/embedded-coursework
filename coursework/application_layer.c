#include "uart.h"
#include "application_layer.h"

#include <util/delay.h>
#include <stdbool.h>

#define INT_DDR DDRD
#define INT_PORT PORTD
#define INT_PIN PIND
#define INT0_BIT 2
#define INT1_BIT 3

#define DEVICE_NUMBER 0
#define MAX_ADC_VALUE 1023


static bool ctrl1_state=0, ctrl2_state=0;

static void init_adc()
{
    ADMUX = _BV(REFS0); // AVCC reference
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // Set prescaler to 64
    ADCSRA |= _BV(ADEN); // Enable ADC
}

static void read_adc(uint8_t *value)
{
    uint16_t adc_val;
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    adc_val = ADC;
    value[0] = ADC >> 8;
    value[1] = ADC;
}

static uint16_t convert_adc(uint8_t *value)
{
    return (value[0] << 8 | value[1]);
}

static void init_pwm()
{ 
    DDRD |= _BV(PD5) | _BV(PD4); // enable output driver for OC1A and OC1B
    TCCR1A |= _BV(COM1A1);
    TCCR1A |= _BV(WGM12) | _BV(WGM11) | _BV(WGM10); // toggle OC1A on match
    TCCR1B |= _BV(CS11);
}


static uint16_t set_pwm(uint16_t value)
{
    OCR1A = value;
    OCR1B = value;
    return value;
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
    uint8_t dev;
    // uint8_t data[2];
    // uint16_t value;
    
    src_port = 0; // button 0
    dest_port = 2;
#if (DEVICE_NUMBER == 0)
    dev = 1;
#elif (DEVICE_NUMBER == 1)
    dev = 0;
#elif (DEVICE_NUMBER == 2)
    dev = 1;
#endif
    read_adc(data);

    // TL_transmit(src_port, dest_port, sizeof(data)/sizeof(uint8_t), data);
}


ISR (INT1_vect)
{
    uint8_t src_port, dest_port;
    uint8_t data[2];
    uint16_t value;

#if (DEVICE_NUMBER == 0)
    dev = 2;
#elif (DEVICE_NUMBER == 1)
    dev = 2;
#elif (DEVICE_NUMBER == 2)
    dev = 0;
#endif
    read_adc(data);


    
    // TL_transmit(src_port, dest_port, sizeof(data)/sizeof(uint8_t), data);
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

    init_adc();
    init_pwm();

    // TL_setup();
}