/*
 * cc3000_platform.h
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CC3000_PLATFORM_H_
#define CC3000_PLATFORM_H_

#include <stdio.h>

#define CC3000_TRACE_LEVEL_ALL		0xff
#define CC3000_TRACE_LEVEL_DEBUG	0x08
#define CC3000_TRACE_LEVEL_INFO		0x04
#define CC3000_TRACE_LEVEL_UNSOL	0x02
#define CC3000_TRACE_LEVEL_STARTUP	0x01
#define CC3000_TRACE_LEVEL_NONE	0

//#define CC3000_TRACE_LEVEL			(CC3000_TRACE_LEVEL_UNSOL |CC3000_TRACE_LEVEL_INFO)
#define CC3000_TRACE_LEVEL	CC3000_TRACE_LEVEL_ALL

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed long int32;
typedef unsigned long uint32;

extern void debug_int(unsigned int wert);
extern void debug_int_hex(unsigned char wert);
extern void debug_putc(unsigned char wert);
extern void debug_nl(void);
extern void debug_int_hex_16bit(uint16 wert);
extern void debug_str(const char *string);

extern volatile uint8 cc3000_isr_flag;

//*****************************************************************************
//
//! delay_us
//!
//!  @param  usec    delay time in microseconds
//!
//!  @brief  delay in microseconds
//
//*****************************************************************************
extern void delay_us( uint32 usec);
//*****************************************************************************
//
//! cc3000_hw_setup
//!
//!  @brief  hardware setup for startup
//
//*****************************************************************************
extern void cc3000_hw_setup(void);
//*****************************************************************************
//
//! cc3000_set_pin_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
extern void cc3000_set_pin_EN(void);
//*****************************************************************************
//
//! cc3000_clear_pin_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
extern void cc3000_clear_pin_EN(void);
//*****************************************************************************
//
//! cc3000_set_pin_WL_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
extern void cc3000_set_pin_WL_EN(void);
//*****************************************************************************
//
//! cc3000_clear_pin_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
extern void cc3000_clear_pin_WL_EN(void);
//*****************************************************************************
//
//! cc3000_read_irq_pin
//!
//!  @brief    reads the state of the IRQ pin
//!
//!  @return   if pin is high 1 is returned else 0 is returned
//
//*****************************************************************************
extern uint8 cc3000_read_irq_pin(void);
//*****************************************************************************
//
//! cc3000_interrupt_enable
//!
//!  @brief  enables IRQ
//
//*****************************************************************************
extern void cc3000_interrupt_enable(void);
//*****************************************************************************
//
//! cc3000_interrupt_disable
//!
//!  @brief  disables IRQ
//
//*****************************************************************************
extern void cc3000_interrupt_disable(void);
//*****************************************************************************
//
//! cc3000_assert_cs
//!
//!  @brief  asserts CS
//
//*****************************************************************************
extern void cc3000_assert_cs(void);
//*****************************************************************************
//
//! cc3000_deassert_cs
//!
//!  @brief  deasserts CS
//
//*****************************************************************************
extern void cc3000_deassert_cs(void);
//*****************************************************************************
//
//! cc3000_spi_send
//!
//!  @param  data    data to send
//!
//!  @brief  sends 1 byte to the CC3000
//
//*****************************************************************************
extern void cc3000_spi_send(uint8 data);
//*****************************************************************************
//
//! cc3000_spi_recv
//!
//!  @return  returns 1 byte
//!
//!  @brief  receives 1 byte from CC3000
//
//*****************************************************************************
extern uint8 cc3000_spi_recv(void);

#endif /* CC3000_PLATFORM_H_ */

#ifdef __cplusplus
}
#endif

