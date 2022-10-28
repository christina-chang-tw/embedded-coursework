#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include the library for the RFM12 module and the UART
#include "rfm12.h"
#include "tick.h"

uint8_t *bufptr;
char str[100];	//the string want to send
uint16_t p = 0;	//the position of string

int main(void)
{
	init_uart0();    //init uart
	_delay_ms(100);  //delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	uint8_t tv[] = "pippo\n\r"; //the message to send
	sei();           //interrupts on
	int i = 0;

   while(1)
   {
        i++;
		if (i == 10000)
		{
			put_str("TRANSMITTING: ");
			put_str((char*) tv);
			rfm12_tx(sizeof(tv), 0, tv);
			i = 0;
		}

		csma_0_persistent();
		rfm12_poll();

		if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			put_str("NEW PACKET:  ");
			bufptr = rfm12_rx_buffer();
			for (int j = 0; j < rfm12_rx_len(); j++)
			{
				put_ch(bufptr[j]);
			}
			put_str("\r\n");
			rfm12_rx_clear();
			_delay_ms(100);
		}
   }
}
