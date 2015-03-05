/*
 * termometr.cpp
 *
 *  Created on: 19 gru 2014
 *      Author: Adrian
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stm32l1xx.h"

#include "bsp.h"

#include "termometr.h"

//Peripherials
#include "lcd.h"
#include "gpio.h"
#include "timer.h"
#include "button.h"

//Drivers
#include "MCP980x.h"
#include "M41T56C64.h"
#include "LIS35DE.h"

//Application
#include "service.h"


/*---------------------------------------------------------------------------------------------------------------------+
 | local variables
 +---------------------------------------------------------------------------------------------------------------------*/

volatile uint32_t delayTicks=0;
volatile uint32_t ticks=0;
volatile uint8_t delayFlag=0;
volatile uint8_t buttonFlag=0;
volatile uint8_t msFlag=0;
volatile uint8_t serviceFlag=0;
volatile uint8_t alertFlag=0;


/*---------------------------------------------------------------------------------------------------------------------+
 | global functions
 +---------------------------------------------------------------------------------------------------------------------*/

void termometr_Init()
{
	gpioInitialize();

	// LCD Initialize
	LCD_Init();

	// Thermometer Initialize
	MCP980x_Init();

	// Timer2 Initialize
	Timer_Init();

	// Blue button Initialize
	Button_Init();

	// Service pin Initialize
	ServicePin_Init();

	// Accelerometer Initialize
	LIS35DE_Init();

	// Accelerometer Led Initialize
	gpioConfigurePin(LED_ACC_GPIO, LED_ACC_pin, GPIO_OUT_PP_2MHz);
}

/**
 * \brief	Application for digital thermometer/accelerometer/clock. Refreshes time or temperature once a second and print ot on LCD.
 * 			Reads all axis of accelerometer and calculates mean of them, and when this value reach some level, then led is switch on.
 * 			Activating service mode when interrupt in service pin is detected.
 */
void termometr_Run()
{
	float themperature=0;

	uint8_t time[3];

	char string[20];

	int8_t x;
	int8_t y;
	int8_t z;

	float srednia;

	while(1)
	{
		if(serviceFlag)
		{
			serviceMode();
			serviceFlag=0;
		}

		else if(msFlag)
		{
			LIS35DE_Read(&x, &y, &z);
			srednia = x*x + y*y + z*z;

			if(srednia>3800) alertFlag|=(1<<ALERT_ACC);
			else alertFlag&=~(1<<ALERT_ACC);

			msFlag=0;

			if(buttonFlag)
			{
				M41T56C64_ReadTime(time);
				M41T56C64_ConvertToInt(time);
				LCD_WriteTime(time);
			}
			else
			{
				themperature=MCP980x_Single_Measure();
				LCD_WriteFloat(&themperature,2,1);
				if(themperature>=30) alertFlag|=(1<<ALERT_TEMP);
				else alertFlag&=~(1<<ALERT_TEMP);
			}
		}

		if(alertFlag) LED_ACC_bb=1;
		else LED_ACC_bb=0;
	}
}

/**
 * \brief Delays designed time in milliseconds
 *
 * \param Number of milliseconds to wait
 */
void delay(uint32_t ms)
{
	delayTicks=0;
	delayFlag=1;
	while(delayTicks<ms);
	delayFlag=0;
}


/*---------------------------------------------------------------------------------------------------------------------+
| local functions implementation
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Counts ticks
 */
static void countTicks()
{
	delayTicks++;
}

/*---------------------------------------------------------------------------------------------------------------------+
 | Interrupts
 +---------------------------------------------------------------------------------------------------------------------*/

extern "C" void EXTI0_IRQHandler(void) __attribute((interrupt));
void EXTI0_IRQHandler(void)
{
	//Do something
	buttonFlag^=1;

	delay(50);

	//Clear flags
	EXTI->PR=EXTI_PR_PR0;
}

extern "C" void EXTI4_IRQHandler(void) __attribute((interrupt));
void EXTI4_IRQHandler(void)
{
	//Do something
	serviceFlag=1;

	//Clear flags
	EXTI->PR=EXTI_PR_PR4;
}

extern "C" void TIM2_IRQHandler(void) __attribute((interrupt));
void TIM2_IRQHandler(void)
{
	//Do something
	if(delayFlag==1) countTicks();

	ticks++;
	if(ticks>=1000){
		ticks=0;
		msFlag=1;
	}

	//Clear flags
	TIM2->SR = 0;
}

