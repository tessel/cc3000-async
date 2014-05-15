/*
 * cc3000_event_handler.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_event_handler.h"
#include "cc3000_platform.h"
#include "cc3000_general.h"
#include "cc3000_hci.h"
#include "cc3000_spi.h"

// the desired state (is beeing increased after a command has been sent)
uint32 cc3000_desired_state = 0;
// the current state (is beeing increased after a expected command has been received)
uint32 cc3000_current_state = 0;
// buffer to hold the received payload
uint8 cc3000_return_buffer[CC300_SPI_RX_BUFFER_SIZE];
// number of bytes returned
uint8 cc3000_return_buffer_size = 0;

// do wee need to close a socket?
uint8 cc3000_close_socket = 0;

//prototype
void cc3000_set_return(uint8 *data, uint8 offset, uint8 size);

//*****************************************************************************
//
//! cc3000_event_handler
//!
//!  @param  data
//!
//!  @return  returns 1 when done
//!
//!  @brief  handles all incomming data from cc3000
//
//*****************************************************************************
uint8 cc3000_event_handler(uint8 *data){

	uint8 count;
	uint8 hci_type;
	uint8 buffer_count;
	uint16 hci_opcode;
	uint16 hci_args_length;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_event_handler\n");
#endif

	// determine the message type
	hci_type = data[HCI_TYPE_OFFSET];

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("type:\n");
	debug_int_hex(hci_type);
	debug_nl();
#endif

	switch (hci_type){

		case HCI_TYPE_EVENT:

			// determine opcode
			hci_opcode   = data[HCI_OPCODE_MSB_OFFSET];
			hci_opcode <<= 8;
			hci_opcode  += data[HCI_OPCODE_LSB_OFFSET];

			// determine args_length
			hci_args_length   = data[HCI_ARGS_LENGTH_MSB_OFFSET];
			hci_args_length <<= 8;
			hci_args_length  += data[HCI_ARGS_LENGTH_LSB_OFFSET];

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
			debug_str("opcode:\n");
			debug_int_hex_16bit(hci_opcode);
			debug_str("args_length:\n");
			debug_int_hex_16bit(hci_args_length);
#endif

			// did we expect the returned opcode?
			if(hci_opcode == cc3000_expect){
				//we expected this command so we can increase the current state
				cc3000_current_state++;
				cc3000_expect = 0;
				printf("***we got the expected command***\n\r");
			}
			// is this an unsolicited event?
			if(hci_opcode & HCI_EVNT_UNSOL_BASE){
				switch (hci_opcode){
					case HCI_EVNT_DATA_UNSOL_FREE_BUFF:
						//ToDo: clean this up
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
						debug_str("Free buffer!\n");
#endif
						buffer_count = data[5]; // 16 bits but who cares
						for (count = 0; count < buffer_count; count++) {
							uint16 my_buffers_free = data[5+2+2+(count*2)];
							cc3000_free_buffers += my_buffers_free;
						}
						break;
					default:
						break;
				}
			}else if(hci_opcode & HCI_EVNT_WLAN_UNSOL_BASE){
				switch (hci_opcode){
					case HCI_EVNT_WLAN_KEEPALIVE:
						break;
					case HCI_EVNT_WLAN_UNSOL_CONNECT:
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_UNSOL)
						debug_str("***we are connected***\n");
#endif
						cc3000_state.connected = 1;
						break;
					case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_UNSOL)
						debug_str("***we are disconnected***\n");
#endif
						cc3000_state.connected = 0;
						break;
					case HCI_EVNT_WLAN_UNSOL_INIT:
					case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
						break;
					case HCI_EVNT_WLAN_UNSOL_DHCP:
						cc3000_state.ip[0] = data[5];
						cc3000_state.ip[1] = data[6];
						cc3000_state.ip[2] = data[7];
						cc3000_state.ip[3] = data[8];
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_UNSOL)
						debug_str("***we got an IP***\n");
						debug_int(cc3000_state.ip[3]);
						debug_putc('.');
						debug_int(cc3000_state.ip[2]);
						debug_putc('.');
						debug_int(cc3000_state.ip[1]);
						debug_putc('.');
						debug_int(cc3000_state.ip[0]);
						debug_putc('.');
						debug_nl();
						// only in case of dhcp
						debug_str("from:\n");
						debug_int(data[16]);
						debug_putc('.');
						debug_int(data[15]);
						debug_putc('.');
						debug_int(data[14]);
						debug_putc('.');
						debug_int(data[13]);
						debug_nl();
#endif

						cc3000_state.dhcp_complete = 1;

						break;
					case HCI_EVNT_WLAN_ASYNC_PING_REPORT:
						break;
					case HCI_EVNT_BSD_TCP_CLOSE_WAIT:
						// ToDO: close the socket here
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_UNSOL)
						debug_str("we should close the socket now...\n\r");
#endif
						cc3000_close_socket = 1;
						break;
					default:
						break;
				}
			}else{
				// this is a "normal event"
				// we do not need to differentiate.. we just copy the received data into a buffer
				switch (hci_opcode){
					case HCI_CMND_READ_BUFFER_SIZE:
					case HCI_CMND_WLAN_CONFIGURE_PATCH:
					case HCI_NETAPP_DHCP:
					case HCI_NETAPP_PING_SEND:
					case HCI_NETAPP_PING_STOP:
					case HCI_NETAPP_ARP_FLUSH:
					case HCI_NETAPP_SET_DEBUG_LEVEL:
					case HCI_NETAPP_SET_TIMERS:
					case HCI_EVNT_NVMEM_READ:
					case HCI_EVNT_NVMEM_CREATE_ENTRY:
					case HCI_CMND_NVMEM_WRITE_PATCH:
					case HCI_NETAPP_PING_REPORT:
					case HCI_EVNT_MDNS_ADVERTISE:
					case HCI_CMND_SETSOCKOPT:
					case HCI_CMND_WLAN_CONNECT:
					case HCI_CMND_WLAN_IOCTL_STATUSGET:
					case HCI_EVNT_WLAN_IOCTL_ADD_PROFILE:
					case HCI_CMND_WLAN_IOCTL_DEL_PROFILE:
					case HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY:
					case HCI_CMND_WLAN_IOCTL_SET_SCANPARAM:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_START:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_STOP:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_SET_PREFIX:
					case HCI_CMND_EVENT_MASK:
					case HCI_EVNT_WLAN_DISCONNECT:
					case HCI_EVNT_SOCKET:
					case HCI_EVNT_BIND:
					case HCI_CMND_LISTEN:
					case HCI_EVNT_CLOSE_SOCKET:
					case HCI_EVNT_CONNECT:
					case HCI_EVNT_NVMEM_WRITE:
					case HCI_EVNT_READ_SP_VERSION:
					case HCI_EVNT_BSD_GETHOSTBYNAME:
					case HCI_EVNT_ACCEPT:
					case HCI_EVNT_RECV:
					case HCI_EVNT_RECVFROM:
					case HCI_EVNT_SENDTO:
					case HCI_EVNT_SELECT:
					case HCI_CMND_GETSOCKOPT:
					case HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS:
					case HCI_CMND_SIMPLE_LINK_START:
					case HCI_NETAPP_IPCONFIG:
						cc3000_set_return(data,HCI_ARGS_OFFSET,hci_args_length);
						break;
					default:
						break;
				}
			}
			break;

		case HCI_TYPE_DATA:

			// determine opcode
			hci_opcode   = data[HCI_OPCODE_LSB_OFFSET];

			// Payload length
			hci_args_length   = data[HCI_ARGS_LENGTH_MSB_OFFSET];
			hci_args_length <<= 8;
			hci_args_length  += data[HCI_ARGS_LENGTH_LSB_OFFSET];
			hci_args_length  += data[2];

			// did we expect the returned opcode?
			if(hci_opcode == cc3000_expect){
				//we expected this command so we can increase the current state
				cc3000_current_state++;
				cc3000_expect = 0;
			}

			switch (hci_opcode){
				case HCI_DATA_RECV:
					cc3000_set_return(data,2,hci_args_length);
					break;

				default:
					break;

			}
			break;


		default:
			break;
	}
	cc3000_deassert_cs();
	return 1;
}
//*****************************************************************************
//
//! cc3000_is_ready
//!
//!  @param  none
//!
//!  @return  	returns 1 when the expected message was received form the cc3000
//!
//!  @brief  	this is called by the flow_hanlder to determine if the expected
//!				data has been received
//
//*****************************************************************************
uint8 cc3000_is_ready(void){
	return (cc3000_desired_state == cc3000_current_state) ? 1 : 0;
}
//*****************************************************************************
//
//! cc3000_set_return
//!
//!  @param  	data	the returned data from the cc3000
//!
//!  @param  	offset	the offset of the payload
//!
//!  @param  	size	the length of the payload
//!
//!  @return  	none
//!
//!  @brief  	frees the received data from the header and saves the payload
//!				in the return buffer
//!
//
//*****************************************************************************
void cc3000_set_return(uint8 *data, uint8 offset, uint8 size){

	uint8 count;
	//cc3000_return_buffer_size = size;
	for(count = 0; count < size; count++){
		cc3000_return_buffer[count] = data[count+offset];
	}
}
//*****************************************************************************
//
//! cc3000_get_return_uint8
//!
//!  @param  	offset
//!
//!  @return  	returns one byte from the return buffer
//!
//!  @brief  	see: @return
//!
//
//*****************************************************************************
uint8 cc3000_get_return_uint8(uint8 offset){
	return cc3000_return_buffer[0+offset];
}
//*****************************************************************************
//
//! cc3000_get_return_int8
//!
//!  @param  	offset
//!
//!  @return  	returns one byte from the return buffer
//!
//!  @brief  	see: @return
//!
//
//*****************************************************************************
int8 cc3000_get_return_int8(uint8 offset){
	return cc3000_return_buffer[0+offset];
}
//*****************************************************************************
//
//! cc3000_get_return_uint32
//!
//!  @param  	offset
//!
//!  @return  	returns 4 bytes from the return buffer as uint32
//!
//!  @brief  	see: @return
//!
//
//*****************************************************************************
uint32 cc3000_get_return_uint32(uint8 offset){
	uint32 ret_data;

	ret_data   = cc3000_return_buffer[3+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[2+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[1+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[0+offset];

	return ret_data;
}
//*****************************************************************************
//
//! cc3000_get_return_int32
//!
//!  @param  	offset
//!
//!  @return  	returns 4 bytes from the return buffer as int32
//!
//!  @brief  	see: @return
//!
//
//*****************************************************************************
int32 cc3000_get_return_int32(uint8 offset){

	int32 ret_data;

	ret_data   = cc3000_return_buffer[3+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[2+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[1+offset];
	ret_data <<= 8;
	ret_data  += cc3000_return_buffer[0+offset];

	return ret_data;
}
//*****************************************************************************
//
//! cc3000_get_return_stream
//!
//!  @param  	data	pointer to buffer
//!
//!  @param  	offset	offset
//!
//!  @param  	size	bytes to receive
//!
//!  @return  	none
//!
//!  @brief  	fills the given buffer with a data stream
//!				and stops if the size exceeds the args_length
//!
//
//*****************************************************************************
void cc3000_get_return_stream(uint8*data, uint8 offset,uint8 size){

	uint8 count;
	for(count = 0; count < size; count++){
		//data[count] = cc3000_return_buffer[count+offset];
		*data++ = cc3000_return_buffer[count+offset];

		//if(count+offset == cc3000_return_buffer_size){
		//	printf("ERR: cc3000_get_return_stream\n\r");
		//	break;
		//}
	}
}
//*****************************************************************************
//
//! get_return_size
//!
//!  @param  	none
//!
//!  @return  	number of bytes in buffer
//!
//!  @brief  	returns the number of bytes in the return buffer to determine
//!				the size of a given return buffer
//!
//
//*****************************************************************************
uint8 get_return_size(void){
	return cc3000_return_buffer_size;
}

