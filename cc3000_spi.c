/*
 * cc3000_spi.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_platform.h"
#include "cc3000_spi.h"
#include "cc3000_flow_handler.h"
#include "cc3000_event_handler.h"

uint8 cc3000_spi_rx_buffer[CC300_SPI_RX_BUFFER_SIZE];
//*****************************************************************************
//
//! cc3000_spi_start_send
//!
//!  @param  hci_length		length of data to send
//!
//!  @return  returns if padding byte was required
//!
//!  @brief  disables IRQ
//
//*****************************************************************************
uint8 cc3000_spi_start_send(uint16 hci_length){

	uint16 spi_length = CC3000_SPI_HEADER_SIZE + hci_length;
	uint8 padding_byte_required = spi_length & 0x01;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_spi_start_send\n");
	debug_str("hci_length:\n");
	debug_int_hex_16bit(hci_length);
	debug_nl();
#endif

	if (padding_byte_required) {
		hci_length++;
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("(+1 padding byte)\n");
#endif
	}

	cc3000_interrupt_disable();
	cc3000_assert_cs();

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("Waiting for IRQ line to go low\n");
#endif

	while(cc3000_read_irq_pin());

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("Got it\n");
#endif

	cc3000_spi_send(SPI_OPERATION_WRITE);	//0x01
	cc3000_spi_send(hci_length >> 8);	// length msb
	cc3000_spi_send(hci_length & 0xff);	// length lsb
	cc3000_spi_send(0);	// busy 0
	cc3000_spi_send(0);	// busy 1
	return (padding_byte_required);
	// interrupts still off at this point
}
//*****************************************************************************
//
//! cc3000_spi_finish_send
//!
//!  @param  padding byte required
//!
//!  @return none
//!
//!  @brief  finishes the send and enables IRQ
//
//*****************************************************************************
void cc3000_spi_finish_send(uint8 padding_byte_required){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("cc3000_spi_finish_send\n");
#endif

	if (padding_byte_required) {
		cc3000_spi_send(0);
	}

	cc3000_deassert_cs();
	cc3000_interrupt_enable();
}
//*****************************************************************************
//
//! cc3000_spi_receive
//!
//!  @param  none
//!
//!  @return on success returns 1, on error returns 0
//!
//!  @brief  receives data from the cc3000
//
//*****************************************************************************
uint16 cc3000_spi_receive(void){

	uint16 count;
	uint8 spi_length;

	cc3000_assert_cs();

	cc3000_spi_send(SPI_OPERATION_READ);
	cc3000_spi_send(0);	// busy 0
	cc3000_spi_send(0);	// busy 1

	spi_length = cc3000_spi_recv();
	spi_length = spi_length << 8;
	spi_length += cc3000_spi_recv();


#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("SPI_READ:\n");
	debug_int(spi_length);
	debug_str(" bytes\n");
#endif


	if ((spi_length > 255) || (spi_length == 0)) {
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("ERR: spi length\n");
#endif
		return(0);
	}

	for (count = 0; count < spi_length; count++) {
		cc3000_spi_rx_buffer[count] = cc3000_spi_recv();
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_int_hex(cc3000_spi_rx_buffer[count]);
#endif
	}
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_nl();
#endif
	// CS left disabled
	return 1;
}

