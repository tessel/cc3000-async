/*
 * _main.c
 *
 *  Created on: 09.09.2013
 *      Author: Johannes
 */


#include "cc3000_event_handler.h"
#include "cc3000_flow_handler.h"
#include "cc3000_general.h"
#include "cc3000_spi.h"
#include "cc3000_wlan.h"

extern int main( void )
{

	uint8 one_time_call = 0;
	//init the system
	//...

	// setupt the hardware to interact with the cc3000
	// this stes the pins and starts SPI
	cc3000_hw_setup();
	// general startup routine
	cc3000_general_startup(0);
	// set the flow_handler to the next step
	cc3000_fh_set_active(STATE_BUFFER_SIZE);

	//...

	while(1){
		// ----- CC3000 ----- //
		//if(cc3000_isr_flag){
		// ToDo: sometimes the irq itself does not work.. need to be fixed
		// we check if an irq occured or the IRQ pin is low (BETA)
		if(!cc3000_read_irq_pin() || cc3000_isr_flag){
			// disable interrupt
			cc3000_interrupt_disable();
			// reset the isr flag
			cc3000_isr_flag = 0;
			// receive the data
			cc3000_spi_receive();
			// enable interrupt
			cc3000_interrupt_enable();
			// process the recieved data
			cc3000_event_handler(cc3000_spi_rx_buffer);
		}

		// general handler
		cc3000_flow_handler();


		if(cc3000_state.dhcp_complete){

			// so we are connected and got an IP from the AP's DHCP
			// we could now start a socket.. or do other stuff
			// e.g.
			if(!one_time_call){
				one_time_call = 1;
				cc3000_fh_set_active(STATE_SC_OPEN);
			}


		}
	}

	return 0;
}
