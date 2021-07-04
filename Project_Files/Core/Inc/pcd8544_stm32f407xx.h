/*
 * pcd8544_stm32f407xx.h
 *
 *  Created on: May 20, 2021
 *      Author: ad
 */

#ifndef INC_PCD8544_STM32F407XX_H_
#define INC_PCD8544_STM32F407XX_H_


#include <stdint.h>
#include "stm32f407xx.h"

// 					Instruction sets

// 1. NOP function
#define NOP														(uint8_t) 0x0

// 2. Function Sets
#define FUNCTION_SET_BEGINNER									(uint8_t) 0x20

#define PD_CHIP_IN_ACTIVE										(uint8_t) 0x0
#define PD_CHIP_IN_POWER_DOWN_MODE								(uint8_t) 0x4

#define HORIZONTAL_ADDRESSING									(uint8_t) 0x0
#define VERTICAL_ADDRESSING										(uint8_t) 0x2

#define H_VALUE_FOR_DISPALY_OR_Y_X_ADDRESS						(uint8_t) 0x0
// 3. Display Configuration
#define SET_DISPLAY_NORMAL_MODE									(uint8_t) 0xC
#define SET_DISPLAY_INVERSE_VIDEO_MODE							(uint8_t) 0xD

// 4. Y Address
#define SET_Y_ADDRESS_BANK_0									(uint8_t) 0x40

// 5. X Address
#define SET_X_ADDRESS_0											(uint8_t) 0x80

#define H_VALUE_FOR_TEMPERATURE_BIAS_VOP						(uint8_t) 0x1

// 6. TEMPERATURE SETTING
#define SET_VLCD_TEMPERATURE_COEFFICIENT_0						(uint8_t) 0x4


void LCD_Reset_Coordinate (void) ;
void LCD_Pin_Init (void) ;
void LCD_BL_SetReset (uint8_t on_off) ;
void LCD_Reset (void) ;
void LCD_Full_Eraser (void) ;
void LCD_Data_Enabled (void) ;
void LCD_Command_Enabled (void) ;


void HSE_168MHZ_Init (void) ;
void systick_init (void) ;
void delay_ms (uint32_t delay) ;

void SPI2_Init (void) ;
void SPI2_Send_8_bit_pData (uint8_t * pData, uint32_t len) ;
void SPI2_Send_8_bit_data (uint8_t data) ;
void SPI2_Write_DR_Register (uint8_t data) ;
void SPI2_Enable_SPE (void) ;
void SPI2_Disable_SPE (void) ;

void LED_Init (void) ;

#endif /* INC_PCD8544_STM32F407XX_H_ */
