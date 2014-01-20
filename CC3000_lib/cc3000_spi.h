/*
 * cc3000_spi.h
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CC3000_SPI_H_
#define CC3000_SPI_H_

#define SPI_OPERATION_WRITE 0x01
#define SPI_OPERATION_READ  0x03

#define CC3000_SPI_HEADER_SIZE		5
#define CC300_SPI_RX_BUFFER_SIZE	250

extern uint8 cc3000_spi_rx_buffer[CC300_SPI_RX_BUFFER_SIZE];

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
extern uint8 cc3000_spi_start_send(uint16 hci_length);
//*****************************************************************************
//
//! cc3000_spi_finish_send
//!
//!  @param  padding byte required
//!
//!  @return none
//!
//!  @brief  disables IRQ
//
//*****************************************************************************
extern void cc3000_spi_finish_send(uint8 padding_byte_required);
//*****************************************************************************
//
//! cc3000_spi_receive
//!
//!  @param  none
//!
//!  @return on success returns 1, on error returns 0
//!
//!  @brief  disables IRQ
//
//*****************************************************************************
extern uint16 cc3000_spi_receive(void);

#endif /* CC3000_SPI_H_ */

#ifdef __cplusplus
}
#endif

