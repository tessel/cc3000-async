/*
 * cc3000_hci.h
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc3000_platform.h"

#ifndef CC3000_HCI_H_
#define CC3000_HCI_H_

#define HCI_COMMAND_HEADER_LENGTH 	4
#define HCI_DATA_HEADER_LENGTH 		5

#define  HCI_TYPE_COMMAND   		0x01
#define  HCI_TYPE_DATA				0x02
#define  HCI_TYPE_PATCH         	0x03
#define  HCI_TYPE_EVENT          	0x04

extern uint16 cc3000_expect;

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
extern uint8 cc3000_hci_start_command(uint16 opcode, uint8 args_length);
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
extern void cc3000_hci_send_command(uint16 opcode, uint8 args_length, uint8 *args);
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
extern void cc3000_hci_end_command(void);
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
extern uint8 cc3000_hci_start_data(uint8 opcode, uint8 args_length, uint16 data_length);
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
extern void cc3000_hci_send_data(uint8 opcode, uint8 args_length, uint8 *args, uint16 data_length, uint8 *data);
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
extern void cc3000_hci_end_data(void);
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
extern void cc3000_hci_expect(uint16 command);
extern void cc3000_hci_send_uint32(uint32 u);
extern void cc3000_hci_send_uint16(uint16 u);
extern void cc3000_hci_send_uint8(uint8 u);
extern void cc3000_hci_send_stream(uint8 *u, uint8 size);

#endif /* CC3000_HCI_H_ */

#ifdef __cplusplus
}
#endif

