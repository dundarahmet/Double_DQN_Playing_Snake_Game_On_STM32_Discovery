/*
 * pcd8544_stm32f407xx.c
 *
 *  Created on: May 20, 2021
 *      Author: ad
 */

/*
 *
 */

#include "pcd8544_stm32f407xx.h"


void LCD_Reset_Coordinate (void) {
	LCD_Command_Enabled() ;
	SPI2_Send_8_bit_data(FUNCTION_SET_BEGINNER | PD_CHIP_IN_ACTIVE | HORIZONTAL_ADDRESSING | H_VALUE_FOR_DISPALY_OR_Y_X_ADDRESS) ;
	SPI2_Send_8_bit_data(SET_DISPLAY_NORMAL_MODE) ;
	SPI2_Send_8_bit_data(SET_X_ADDRESS_0) ;
	SPI2_Send_8_bit_data(SET_Y_ADDRESS_BANK_0) ;

	LCD_Data_Enabled() ;
}

void LCD_Pin_Init (void) {
/*
 * 1. RST = Reset pin. = PB0
 * 3. D/C = Data/Command. = PB1
 * 6. VCC = PB5
 * 7. BL = PB7
*/
	RCC->AHB1ENR |= (0x2) ;						// Enable B port

	GPIOB->MODER |= (0x4405) ;					// set PB0, 1, 5 and 7 as output
	GPIOB->OTYPER &= ~(0xA3) ;					// set PB0, 1, 5 and 7's output types as push_pull

	GPIOB->ODR &= ~(0xA3) ;						// PB0, 1, 5, and 7 are pulled to low

	GPIOB->ODR |= (0x20) ;						// Power of the LCD is on
	LCD_Reset() ;
}

void LCD_BL_SetReset (uint8_t on_off) {
	if (on_off) {
		GPIOB->ODR |= (0x80) ;					// turn on back light of the LCD
	} else {
		GPIOB->ODR &= ~(0x80) ;					// turn off back light of the LCD
	}
}

void LCD_Reset (void) {
	GPIOB->ODR &= ~(0x1) ;
	delay_ms(100) ;
	GPIOB->ODR |= (0x1) ;

	LCD_BL_SetReset(1) ;

	LCD_Command_Enabled() ;
	SPI2_Send_8_bit_data(FUNCTION_SET_BEGINNER | PD_CHIP_IN_ACTIVE | HORIZONTAL_ADDRESSING | H_VALUE_FOR_TEMPERATURE_BIAS_VOP) ;
	SPI2_Send_8_bit_data(0xD2) ;		// VOP value
	SPI2_Send_8_bit_data(0x10) ;		// BS value. it doesn't effect anything
	SPI2_Send_8_bit_data(SET_VLCD_TEMPERATURE_COEFFICIENT_0) ;

	SPI2_Send_8_bit_data(FUNCTION_SET_BEGINNER | PD_CHIP_IN_ACTIVE | HORIZONTAL_ADDRESSING |  H_VALUE_FOR_DISPALY_OR_Y_X_ADDRESS) ;
	SPI2_Send_8_bit_data(SET_DISPLAY_NORMAL_MODE) ;
	LCD_Full_Eraser() ;
}

void LCD_Full_Eraser (void) {
	LCD_Reset_Coordinate() ;

	SPI2_Enable_SPE() ;
	for (uint16_t counter = 0; counter < 504; counter++) {
		SPI2_Write_DR_Register(0x0) ;
	}
	SPI2_Disable_SPE() ;

	LCD_Reset_Coordinate() ;
}

void LCD_Data_Enabled (void) {
	// D/C = Data/Command. = PB1
	delay_ms(1) ;
	GPIOB->ODR |= (0x2) ;
}
void LCD_Command_Enabled (void) {
	// D/C = Data/Command. = PB1
	delay_ms(1) ;
	GPIOB->ODR &= ~(0x2) ;						// enable Command
}



void HSE_168MHZ_Init (void) {

	//	ENABLE CLOCK SOURSE
	RCC->CR |= (0x10000) ;						// enable HSE
	while (!(RCC->CR & 0x20000)) { } 			// wait until HSE is ready.

	//	VOS
	RCC->CR |= (0x4000) ;						// set VOS to 1

	//	LATENCY
	FLASH->ACR &= ~(0x7) ;						// reset latency in the flash_acr
	FLASH->ACR |= (0x5) ;						// set latency to 5

	if ((FLASH->ACR & 0x7) != 0x5) {
		GPIOD->ODR |= (0x2000) ;				// light orange led to indicate that the
		return ; 								// latency settings is not successfully set.
	}

	//	PLL SETTINGS
	RCC->PLLCFGR |= (0x400000) ;				// selet HSE as PLL input source

	RCC->PLLCFGR &= ~(0x3F) ;					// reset PLLM
	RCC->PLLCFGR |= (0x8) ;						// set PLLM to 16

	RCC->PLLCFGR &= ~(0x7FC0) ;					// reset PLLN
	RCC->PLLCFGR |= (0x5400) ;					// set PLLN to 336

	RCC->PLLCFGR &= ~(0x30000) ;				// reset PLLP so that P value is set to 2

	RCC->PLLCFGR &= ~(0xF000000) ;				// reset PLLQ
	RCC->PLLCFGR |= (0x7000000) ;				// set PLLQ to 7

	//	PLL ENABLE
	RCC->CR |= (0x1000000) ;					// enable PLL
	while (!(RCC->CR & 0x2000000)) { }			// wait until PLL is ready.

	//	CLOCK SETTINGS
	RCC->CFGR &= ~(0x3) ;						// reset SW bits
	RCC->CFGR |= (0x2) ;						// select PLL as system clock source

	RCC->CFGR &= ~(0xF0) ;						// reset AHP prescaler so that AHP prescaler becomes 1

	RCC->CFGR &= ~(0xFC00) ;					// reset APB1 and also APB2
	RCC->CFGR |= (0x1400) ;						// set APB1 prescaler to 4
	RCC->CFGR |= (0x8000) ;						// set APB2 prescaler to 2

	if ((RCC->CFGR & 0xC) != 0x8) {
		GPIOD->ODR |= (0x4000) ;				// light red led to indicate that
		return ;								// clock switch is not successfully set.
	}

	//	DISABLE HSI
	RCC->CR &= ~(0x1) ;							// disable HSI for power consumption

	RCC->AHB2ENR |= (0x40) ;					// Enable random number generator's clock
	RNG->CR |= (0x4) ;							// Enable random number generator and its interrupt


	SystemCoreClockUpdate() ;					// update SystemCoreClock

}

void systick_init (void) {
	SysTick->LOAD = (SystemCoreClock / 1000) ;	// SYSCLK == HCLK
	SysTick->VAL = 0 ;
	SysTick->CTRL = 0x5 ;
}

void delay_ms (uint32_t delay) {
	for (; delay > 0; delay--) {
		while (!(SysTick->CTRL & 0x10000)) { }
	}
}

void LED_Init (void) {
	// LED pin using when data is transmitted to the LCD Screen.
	RCC->AHB1ENR |= (0x8) ;
	GPIOD->MODER = 0 ;							// reset GPIOD_MODER register
	GPIOD->MODER |= (0x15000000) ;				// set PD12, 13 and 14 as output
	GPIOD->OTYPER = 0 ;							// reset GPIOD_OTYPE so that PD12, 13 and 14 becomes push-pull
	GPIOD->ODR = 0 ;							// turn off PD12, 13 and 14.
}



void SPI2_Init (void) {
	RCC->AHB1ENR |= (1U << 1) ;					// Enable Port B clock
	GPIOB->MODER |= (0x8A000000) ;				// Set PB12, 13 and 15 as alternate function mode.
	GPIOB->OTYPER &= ~(0xB000) ;				// Set PB12, 13 and 15's output type as push pull
	GPIOB->OSPEEDR &= ~(0xCF000000) ;			// Set PB12, 13 and 15's output speed as low speed
	GPIOB->PUPDR |= (0x88000000) ;				// set PB13 and 15's pull-up/down resistor to pull-down
	GPIOB->AFR[1] |= (0x50550000) ;


	// SPI2 is connected to APB1
	// PCLK1 is running at 8Mhz.

	RCC->APB1ENR |= (0x4000) ; 					// Enable SPI2 clock
	SPI2->CR1 = 0 ;								// reset SPI2_CR1 register

	/*
	 * after reset, following configiration is set.
	 * Set CPHA to 0
	 * Set CPOL to 0
	 * Set Data Format to 8 bit
	 * Set to MSB transmitted first
	 * Set SSM bit to zero
	 * BIDIMODE=0
	 * RXONLY=0
	*/


	SPI2->CR1 |= (0x18) ;						// set baud rate to fpclk/16. fplck is 42Mhz so that
												// baud rate is 2Mhz.
	SPI2->CR1 |= (0x4) ;						// select Master configuration

	SPI2->CR2 |= (0x4) ;						// Set SSOE bit to 1

	SPI2_Enable_SPE() ;
	delay_ms(50) ;
	SPI2_Disable_SPE() ;
}

void SPI2_Write_DR_Register (uint8_t data) {
	while (!(SPI2->SR & 0x2)) { }			// wait until TXE is 1
	SPI2->DR = data ;
}
void SPI2_Send_8_bit_data (uint8_t data) {
	SPI2_Enable_SPE() ;
	SPI2_Write_DR_Register(data) ;
	SPI2_Disable_SPE() ;
}
void SPI2_Send_8_bit_pData (uint8_t * pData, uint32_t len) {
	SPI2_Enable_SPE() ;
	while (len > 0) {
		SPI2_Write_DR_Register(*pData) ;
		pData++ ;
		len-- ;
	}
	SPI2_Disable_SPE() ;
}

void SPI2_Enable_SPE (void) {
	SPI2->CR1 |= (0x40) ;
}
void SPI2_Disable_SPE (void) {
	while ( !(SPI2->SR & 0x2) && (SPI2->SR & 0x80)) { }
	SPI2->CR1 &= ~(0x40) ;
}
