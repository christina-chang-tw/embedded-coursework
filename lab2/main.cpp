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

void put_hex(uint8_t inp){
	uint8_t char1 = (inp&0xF0)>>4;
	uint8_t char2 = inp&0x0F;
	if(char1<0x0A){
		put_ch(char1+48);
	}
	else{
		put_ch(char1+55);
	}
	if(char2<0x0A){
		put_ch(char2+48);
	}
	else{
		put_ch(char2+55);
	}
}

void tick()
{
	while (ctrl.rfm12_state != STATE_RX_IDLE)
	{
		put_str("\r\n\r\n WAITING \r\n\r\n");
		_delay_ms(200);
	}

	if (ctrl.txstate == STATUS_OCCUPIED)
	{
		// Start Transmitting
		put_str("\r\n");
		put_str("TRANSMITTING: ");
		put_str((char*) tv);
		put_str("\r\n");
		rfm12_start_tx();
	}
}

int main(void)
{
	uint16_t transmit_counter = 0;
	uint8_t *bufptrentire;
	uint8_t *bufptr;
	init_uart0();   //Initialise the uart
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	uint8_t tv[] = "demo_master";
	sei();           //interrupts on

	while(1)
	{
		transmit_counter++;
		if (rfm12_rx_status() == STATUS_OCCUPIED)
		{

			put_ch('\r');
			put_ch('\n');
			put_str("RX DATA, ");

			bufptrentire = rfm12_rx_entire(); //get the address of the current rx buffer

			// dump buffer contents to uart
			put_str("L=0x");
			put_hex(rfm12_rx_len());
			put_ch(' ');
			put_hex(bufptrentire[1]);
			put_hex(bufptrentire[2]);
			put_ch('|');
			uint8_t i;
			for (i=1;i<rfm12_rx_len();i++)
			{
				put_hex(bufptrentire[i]);
			}
			for(;i<32;i++){
					put_ch(' ');
					put_ch(' ');
			}
			// Print length info about the received data
			bufptr = rfm12_rx_buffer();
			//Put Textual form limiting to printable chars
			for (i=0;i<rfm12_rx_len()-3;i++)
			{
				if(bufptr[i]==0){
					put_ch(32);
				}
				else if(bufptr[i]<32||bufptr[i]>126){
					put_ch(219);
				}
				else{
					put_ch(bufptr[i]);
				}
			}
			for(;i<32;i++){
					put_ch(' ');
					put_ch(' ');
			}
			put_ch('\r');
			put_ch('\n');

			// tell the implementation that the buffer
			// can be reused for the next data.
			// Fill buffer with junk

			for (i=0;i<RFM12_TRX_FRAME_SIZE;i++)
			{
				bufptr[i]=255;
			}
			rfm12_rx_clear();
			_delay_ms(1);
		}

		if(transmit_counter>1000){
			transmit_counter=0;
			uint8_t tx_buff_status = rfm12_tx(sizeof(tv), 0, tv);
			if(tx_buff_status == RFM12_TX_OCCUPIED){
					put_str("f.S=");

					switch (ctrl.rfm12_state) {
						case STATE_RX_IDLE:
							put_ch ('i');
							break;
						case STATE_RX_ACTIVE:
							put_ch ('r');
							break;
						case STATE_TX:
							put_ch ('t');
							break;
						default:
							put_ch ('?');
					}
					put_str("\r\n");
			}
			else if(tx_buff_status == RFM12_TX_ERROR){
					put_str("e.\r\n");
			}
			else{
					put_str("t.");
			}
		}
		//rfm12 needs to be called from your main loop periodically.
		//it checks if the rf channel is free (no one else transmitting), and then
		//sends packets, that have been queued by rfm12_tx above.
		rfm12_tick();
		_delay_us(500);

	}
}