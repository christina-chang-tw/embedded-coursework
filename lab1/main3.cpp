/** \file main.cpp
 * \author Edward Longman
 * \version 1.0
 * \date 2020/10/20
 */


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "rfm12.h"
#include "./uart.h"


//the control struct
extern rfm12_control_t ctrl;

int main(void)
{
	uint16_t transmit_counter = 0;
	uint8_t *bufptr;
	init_uart0();   //Initialise the uart
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	uint8_t tv[] = "Hello hell";
	sei();           //interrupts on

	while(1)
	{
		transmit_counter++;
		if (rfm12_rx_status() == STATUS_OCCUPIED)
		{

			// Print length info about the received data
			bufptr = rfm12_rx_buffer();
			//Put Textual form limiting to printable chars
			for (int i=0;i<rfm12_rx_len();i++)
			{		
				put_ch(bufptr[i]);
			}
			put_str("\r\n");
			rfm12_rx_clear();
			_delay_ms(1);
		}

		if(transmit_counter>1000){
			transmit_counter=0;
			uint8_t tx_buff_status = rfm12_tx(sizeof(tv), 0, tv);
			if(tx_buff_status == RFM12_TX_OCCUPIED){
					put_str("Device state =");

					switch (ctrl.rfm12_state) {
						case STATE_RX_IDLE:
							put_ch ('IDLE');
							break;
						case STATE_RX_ACTIVE:
							put_ch ('RX ACTIVE');
							break;
						case STATE_TX:
							put_ch ('TX');
							break;
						default:
							put_ch ('?');
					}
					put_str("\r\n");
			}
			else if(tx_buff_status == RFM12_TX_ERROR){
					put_str("ERROR\r\n");
			}
			else{
					put_str("TX_TRANSMIT\r\n");
			}
		}
		//rfm12 needs to be called from your main loop periodically.
		//it checks if the rf channel is free (no one else transmitting), and then
		//sends packets, that have been queued by rfm12_tx above.
		rfm12_tick();
		_delay_us(500);

	}
}