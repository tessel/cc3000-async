/*
 * cc3000_hci.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_hci.h"
#include "cc3000_platform.h"
#include "cc3000_spi.h"

uint8 padding_byte_required;
uint16 cc3000_expect;

//*****************************************************************************
//
//! cc3000_hci_start_command
//!
//!  @param  opcode
//!
//!  @param  args_length
//!
//!  @return  returns if padding byte was required
//!
//!  @brief  cc3000_hci_start_command
//
//*****************************************************************************
uint8 cc3000_hci_start_command(uint16 opcode, uint8 args_length){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_INFO)
	debug_str("cc3000_hci_start_command\n");
	debug_str("opcode:\n");
	debug_int_hex_16bit(opcode);
#endif

	padding_byte_required = cc3000_spi_start_send(HCI_COMMAND_HEADER_LENGTH + args_length);

	cc3000_spi_send(HCI_TYPE_COMMAND);
	cc3000_spi_send(opcode & 0xff);
	cc3000_spi_send(opcode >> 8);
	cc3000_spi_send(args_length);

	return padding_byte_required;

}
//*****************************************************************************
//
//! cc3000_hci_send_command
//!
//!  @param  opcode
//!
//!  @param  args_length
//!
//!  @param  args
//!
//!  @return  returns if padding byte was required
//!
//!  @brief  cc3000_hci_send_command
//
//*****************************************************************************
void cc3000_hci_send_command(uint16 opcode, uint8 args_length, uint8 *args){

	uint8 padding_byte;
	uint16 count;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_send_command\n");
	debug_str("opcode:\n");
	debug_int_hex_16bit(opcode);
#endif

	padding_byte = cc3000_spi_start_send(HCI_COMMAND_HEADER_LENGTH + args_length);

	cc3000_spi_send(HCI_TYPE_COMMAND);
	cc3000_spi_send(opcode & 0xff);
	cc3000_spi_send(opcode >> 8);
	cc3000_spi_send(args_length);

	if (args_length > 0) {
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("HCI args:\n");
#endif
		for (count = 0; count < args_length; count++) {
			cc3000_spi_send(args[count]);
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
			debug_int_hex(args[count]);
#endif
		}
	}
	cc3000_spi_finish_send(padding_byte);
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_nl();
	debug_str("Done.\n");
#endif

}
//*****************************************************************************
//
//! cc3000_hci_end_command
//!
//!  @param  none
//!
//!  @return  none
//!
//!  @brief  cc3000_hci_end_command
//
//*****************************************************************************
void cc3000_hci_end_command(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_end_command\n");
#endif
	cc3000_spi_finish_send(padding_byte_required);

}
//*****************************************************************************
//
//! cc3000_hci_start_data
//!
//!  @param  opcode
//!
//!  @param  args_length
//!
//!  @param  data_length
//!
//!  @return  returns if padding byte was required
//!
//!  @brief  cc3000_hci_start_data
//
//*****************************************************************************
uint8 cc3000_hci_start_data(uint8 opcode, uint8 args_length, uint16 data_length){

	uint16 payload_length;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_start_data\n");
	debug_str("opcode = ");
	debug_int_hex(opcode);
	debug_nl();
#endif

	payload_length = args_length + data_length;

	padding_byte_required = cc3000_spi_start_send(HCI_DATA_HEADER_LENGTH + payload_length);

	cc3000_spi_send(HCI_TYPE_DATA);
	cc3000_spi_send(opcode);
	cc3000_spi_send(args_length);
	cc3000_spi_send(payload_length & 0xff);
	cc3000_spi_send(payload_length >> 8);

	return padding_byte_required;

}
//*****************************************************************************
//
//! cc3000_hci_send_data
//!
//!  @param  opcode
//!
//!  @param  args_length
//!
//!  @param  args
//!
//!  @param  data_length
//!
//!  @param  data
//!
//!  @return  none
//!
//!  @brief  cc3000_hci_send_data
//
//*****************************************************************************
void cc3000_hci_send_data(uint8 opcode, uint8 args_length, uint8 *args, uint16 data_length, uint8 *data){

	uint8 padding_byte;
	uint16 payload_length;
	uint16 count;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_send_data\n");
	debug_str("opcode:");
	debug_int_hex(opcode);
	debug_nl();
#endif

	payload_length = args_length + data_length;

	padding_byte = cc3000_spi_start_send(HCI_DATA_HEADER_LENGTH + payload_length);

	cc3000_spi_send(HCI_TYPE_DATA);
	cc3000_spi_send(opcode);
	cc3000_spi_send(args_length);
	cc3000_spi_send(payload_length & 0xff);
	cc3000_spi_send(payload_length >> 8);
	if (args_length > 0) {
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("HCI args:");
#endif
		for (count = 0; count < args_length; count++) {
			cc3000_spi_send(args[count]);
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
			debug_int_hex(args[count]);
#endif
		}
	}

	if (data_length > 0) {
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
		debug_str("HCI data:");
#endif
		for (count = 0; count < data_length; count++) {
			cc3000_spi_send(data[count]);
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
			debug_int_hex(data[count]);
#endif
		}
	}

	cc3000_spi_finish_send(padding_byte);

}
//*****************************************************************************
//
//! cc3000_hci_end_data
//!
//!  @param  none
//!
//!  @return  none
//!
//!  @brief  cc3000_hci_end_data
//
//*****************************************************************************
void cc3000_hci_end_data(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_end_data\n");
#endif
	cc3000_spi_finish_send(padding_byte_required);

}
//*****************************************************************************
//
//! cc3000_hci_expect
//!
//!  @param  command
//!
//!  @return  none
//!
//!  @brief  cc3000_hci_expect
//
//*****************************************************************************
void cc3000_hci_expect(uint16 command){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hci_expect\n");
#endif
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_INFO)
	debug_str("expecting:\n");
	debug_int_hex_16bit(command);
#endif
	cc3000_expect = command;

}


void cc3000_hci_send_uint32(uint32 u) {

	cc3000_spi_send((uint8) (u & 0xff));
	cc3000_spi_send((uint8) ((u >> 8 ) & 0xff));
	cc3000_spi_send((uint8) ((u >> 16 ) & 0xff));
	cc3000_spi_send((uint8) ((u >> 24 ) & 0xff));
}

void cc3000_hci_send_uint16(uint16 u) {

	cc3000_spi_send((uint8) (u & 0xff));
	cc3000_spi_send((uint8) ((u >> 8 ) & 0xff));
}

void cc3000_hci_send_uint8(uint8 u) {

	cc3000_spi_send(u);
}

void cc3000_hci_send_stream(uint8 *u, uint8 size){

	uint8 count;
	for(count = 0; count < size; count++){
		cc3000_spi_send(u[count]);
	}

}
