/*
 * cc3000_flow_handler.h
 *
 *  Created on: 06.09.2013
 *      Author: Johannes
 */

#ifndef CC3000_FLOW_HANDLER_H_
#define CC3000_FLOW_HANDLER_H_

#include "cc3000_platform.h"

#ifdef __cplusplus
extern "C"
{
#endif

// define all the different states the cc3000 needs to execute
#define STATE_IDLE			0
#define STATE_BUFFER_SIZE	1
#define STATE_EVENT_MASK	2
#define STATE_CON_POLICY	3
#define STATE_WL_CONNECT	4
#define STATE_WL_SCAN_PARA	5
#define STATE_WL_SCAN		6
#define STATE_WL_ADD_P		7
#define STATE_WL_DEL_P		8
#define STATE_SC_OPEN		10
#define STATE_SC_CONNECT	11
#define STATE_SC_REC_REQ	12
#define STATE_SC_REC_DATA	13
#define STATE_SC_TRX_DATA	14
#define STATE_SC_BIND		15
#define STATE_SC_LISTEN		16
#define STATE_SC_ACCEPT		17
#define STATE_SC_SOCOPT		18
#define STATE_SC_SELECT		19
#define STATE_SC_CLOSE		20
#define STATE_NA_DHCP		21
#define STATE_POWERDOWN		99

//*****************************************************************************
//
//! cc3000_flow_handler
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	this is the main handler for the cc3000;
//!				it must be called periodically from the main loop;
//!				it can set new states automatically when the previous has been
//!				executed, or it can set the state to idle if nothing is to do
//!
//
//*****************************************************************************
extern void cc3000_flow_handler(void);
//*****************************************************************************
//
//! cc3000_fh_set_active
//!
//!  @param  	state	next state to be executed
//!
//!  @return  	none
//!
//!  @brief  	sets the next state to be executed
//!
//
//*****************************************************************************
extern void cc3000_fh_set_active(uint8 state);

#ifdef __cplusplus
}
#endif

#endif /* CC3000_FLOW_HANDLER_H_ */
