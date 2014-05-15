/*
 * cc3000_platform.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 *
 *  Platform dependent functions
 *
 */

#include "cc3000_platform.h"

// guess what.. i am too fast.. so i need a delay
// if this is defined, then we add a little delay
// before CS_ASSERT and after CS_DEASSERT and before cc3000_read_irq_pin
//#define CS_IS_TOO_FAST

// handle debug output
void debug_str(const char *string){
	printf("%s\r",string);
}
void debug_int(unsigned int wert){
	printf("%i",wert);
}
void debug_int_hex(unsigned char wert){
	printf("0x%02X ",wert);
}
void debug_putc(unsigned char wert){
	printf("%c",wert);
}
void debug_nl(void){
	printf("\n\r");
}
void debug_int_hex_16bit(uint16 wert){
	printf("0x%04X\n\r",wert);
}


//*****************************************************************************
//
//! delay_us
//!
//!  @param  usec    delay time in microseconds
//!
//!  @brief  delay in microseconds
//
//*****************************************************************************
#define BOARD_MCK_MHZ	48
void delay_us( uint32 usec){
	uint32 loop;
	loop = (BOARD_MCK_MHZ * usec)/6;
	while(loop--){
		__asm__("NOP");
	}
}

//*****************************************************************************
//
//! cc3000_hw_setup
//!
//!  @brief  hardware setup for startup
//
//*****************************************************************************
void cc3000_hw_setup(void){
	// Configure port initial values and directions (EN = 0, WL_EN = 0, CS = 1)
	// Init SPI
	// Init IRQ
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_hw_setup\n");
#endif
}
//*****************************************************************************
//
//! cc3000_set_pin_WL_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
void cc3000_set_pin_WL_EN(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_set_pin_WL_EN\n");
#endif

}
//*****************************************************************************
//
//! cc3000_clear_pin_WL_EN
//!
//!  @brief  sets the enable pin
//
//*****************************************************************************
void cc3000_clear_pin_WL_EN(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_clear_pin_WL_EN\n");
#endif

}
//*****************************************************************************
//
//! cc3000_read_irq_pin
//!
//!  @brief    reads the state of the IRQ pin
//!
//!  @return   if pin is high 1 is returned else 0 is returned
//
//*****************************************************************************
uint8 cc3000_read_irq_pin(void) {
#ifdef CS_IS_TOO_FAST
	delay_us(100);
#endif
	return 0;
}
//*****************************************************************************
//
//! cc3000_interrupt_enable
//!
//!  @brief  enables IRQ
//
//*****************************************************************************
void cc3000_interrupt_enable(void){
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_interrupt_ENable\n");
#endif

	if(!cc3000_read_irq_pin()){
		cc3000_isr_flag = 1;
	}


}
//*****************************************************************************
//
//! cc3000_interrupt_disable
//!
//!  @brief  disables IRQ
//
//*****************************************************************************
void cc3000_interrupt_disable(void){
#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_interrupt_DISable\n");
#endif
}
//*****************************************************************************
//
//! cc3000_assert_cs
//!
//!  @brief  asserts CS
//
//*****************************************************************************
void cc3000_assert_cs(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_Assert_cs\n");
#endif
	// Set pin low
#ifdef CS_IS_TOO_FAST
	delay_us(50);
#endif
}
//*****************************************************************************
//
//! cc3000_deassert_cs
//!
//!  @brief  deasserts CS
//
//*****************************************************************************
void cc3000_deassert_cs(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_DEAssert_cs\n");
#endif
#ifdef CS_IS_TOO_FAST
	delay_us(50);
#endif
	// Set pin high
}

//*****************************************************************************
//
//! cc3000_spi_send
//!
//!  @param  data    data to send
//!
//!  @brief  sends 1 byte to the CC3000
//
//*****************************************************************************
void cc3000_spi_send(uint8 data){
	
}

//*****************************************************************************
//
//! cc3000_spi_recv
//!
//!  @return  returns 1 byte
//!
//!  @brief  receives 1 byte from CC3000
//
//*****************************************************************************
uint8 cc3000_spi_recv(void){
	return 0;
}
