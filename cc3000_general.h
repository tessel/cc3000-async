/*
 * cc3000_general.h
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_platform.h"

#ifndef CC3000_GENERAL_H_
#define CC3000_GENERAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define CC3000_SW_VERSION			0.03

#define CC3000_IS_HW_NIT			0x01
#define CC3000_IS_BUFFER_SIZE		0x02

#define CC3000_WS_EVENT_MASK		0x01
#define CC3000_WS_CONN_POLICY		0x02
#define CC3000_WS_SCAN_PARAM		0x04
#define CC3000_WS_GET_SCAN			0x08
#define CC3000_WS_CONNECT			0x10

typedef struct _cc3000_state
{
	uint8 init_status;
	uint8 wlan_status;
	uint8 smart_config_complete;
	uint8 connected;
	uint8 dhcp_complete;
	uint8 ip[4];
}__cc3000_state;
extern __cc3000_state cc3000_state;

extern uint8 cc3000_free_buffers;
extern uint16 cc3000_buffer_length;

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
extern void cc3000_start(uint8 patches_request);
extern void cc3000_stop();
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
extern void cc3000_general_req_buffer_size(void);
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
extern uint8 cc3000_general_read_buffer_size(uint8 *hci_free_buffers, uint16 *hci_buffer_length);

extern uint8 * uns32_to_stream( uint8 * i, uint32 n);
extern uint8 * uns16_to_stream( uint8 * i, uint16 n);
extern uint16 stream_to_uns16(uint8 * i);
extern uint32 stream_to_uns32(uint8 * i);

#ifdef __cplusplus
}
#endif

#endif /* CC3000_GENERAL_H_ */
