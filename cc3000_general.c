/*
 * cc3000_general.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_general.h"
#include "cc3000_platform.h"
#include "cc3000_event_handler.h"
#include "cc3000_spi.h"
#include "cc3000_hci.h"

#include <stdio.h>

// the global state of the cc3000
__cc3000_state cc3000_state;

//uint8 free_buffers;
//uint16 buffer_length;
uint8 cc3000_free_buffers;
uint16 cc3000_buffer_length;
//*****************************************************************************
//
//! cc3000_general_startup
//!
//!  @param  	patched_required
//!
//!  @return  	none
//!
//!  @brief  	startup routine for the cc3000
//!
//
//*****************************************************************************
void cc3000_start(uint8 patches_request) {

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_STARTUP)
	debug_str("cc3000_general_startup\n");
#endif


	cc3000_state.connected = 0;
	cc3000_state.dhcp_complete = 0;
	cc3000_state.init_status = 0;
	cc3000_state.wlan_status = 0;
	cc3000_state.smart_config_complete = 0;

	cc3000_clear_pin_WL_EN();
	delay_us(50000);

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("Waiting for IRQ line to go high\n");
#endif
	// todo: put timeout here
	while (cc3000_read_irq_pin() != 1);

	cc3000_set_pin_WL_EN();

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("Waiting for IRQ line to go low\n");
#endif
	// todo: put timeout here
	while (cc3000_read_irq_pin() != 0);

	cc3000_assert_cs();

	// Time for the magic start up sequence
	delay_us(50);
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("Sending start command\n");
#endif
	cc3000_spi_send(SPI_OPERATION_WRITE);	//0x01
	cc3000_spi_send(0);	// length msb
	cc3000_spi_send(5);	// length lsb
	cc3000_spi_send(0);	// busy 0

	delay_us(100);

	cc3000_spi_send(0);	// busy 1
	cc3000_spi_send(HCI_TYPE_COMMAND);
	cc3000_spi_send(HCI_CMND_SIMPLE_LINK_START & 0xff);
	cc3000_spi_send(HCI_CMND_SIMPLE_LINK_START >> 8);
	cc3000_spi_send(1);		// 1 byte payload
	cc3000_spi_send(patches_request);

	cc3000_deassert_cs();

	cc3000_desired_state++;
	cc3000_hci_expect(HCI_CMND_SIMPLE_LINK_START);
	
	// now turn interrupts on
	cc3000_interrupt_enable();
}

void cc3000_stop() {
	cc3000_interrupt_disable();
	cc3000_clear_pin_WL_EN();

	// todo: put timeout here
	while (cc3000_read_irq_pin() != 1);
}

//*****************************************************************************
//
//! cc3000_general_req_buffer_size
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	requests the buffers of the cc3000
//!
//
//*****************************************************************************
void cc3000_general_req_buffer_size(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_general_req_buffer_size\n");
#endif
	cc3000_desired_state++;
	cc3000_hci_send_command(HCI_CMND_READ_BUFFER_SIZE, 0, 0);
	cc3000_hci_expect(HCI_CMND_READ_BUFFER_SIZE);
}
//*****************************************************************************
//
//! cc3000_general_read_buffer_size
//!
//!  @param  	hci_free_buffers 	pointer to global free buffers
//!
//!  @param  	hci_buffer_length	pointer to global buffer length
//!
//!  @return  	status of cc3000 buffers???
//!
//!  @brief  	reads the buffers of the cc3000
//!
//
//*****************************************************************************
uint8 cc3000_general_get_buffer_size(){
	cc3000_free_buffers = cc3000_get_return_uint8(0);
	cc3000_buffer_length = cc3000_get_return_uint16(1);

	cc3000_state.init_status |= CC3000_IS_BUFFER_SIZE;

	return 0;
}

