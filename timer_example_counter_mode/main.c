/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @brief Simple timer example project with interrupt. Timer is in counter mode.
 * 
 * In this example TIMER2 interrupt is enabled. The TIMER interrupt handler will toggle a LED whenever
 * the TIMER counter is equal to a value of compare register CC[0] or compare register CC[[1]. The timer
 * is set to 16 bit so it will overflow at value 65536 and start from zero. CC[0] and
 * CC[1] register values can be adjusted to change the LED duty cycle.
 *
 * When the timer is in counter mode, it will only increment when it is 
 * manually incremented with the  NRF_TIMER2->TASKS_COUNT = 1;  command
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"

#define GPIO_TOGGLE_PIN              (LED_0)       /*!< gpio pin to toggle after delay. */


void start_timer(void)
{		
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Counter;              // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                             // clear the task first to be usable for later
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;		   //Set counter to 16 bit resolution
	NRF_TIMER2->CC[0] = 10000;                               //Set value for TIMER2 compare register 0
	NRF_TIMER2->CC[1] = 5;                                   //Set value for TIMER2 compare register 1
		
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);
		
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER2
}
		
/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 */
void TIMER2_IRQHandler(void)
{
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;	       //Clear compare register 0 event	
		nrf_gpio_pin_set(GPIO_TOGGLE_PIN);         //Set LED
  }
	
	if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0))
  {
		NRF_TIMER2->EVENTS_COMPARE[1] = 0;	       //Clear compare register 1 event
		nrf_gpio_pin_clear(GPIO_TOGGLE_PIN);       //Clear LED
  }
}

int main(void)
{
	nrf_gpio_cfg_output(GPIO_TOGGLE_PIN);       //Set LED pin as output
	start_timer();                              //Configure and start timer
	
	while(true) 
	{
		NRF_TIMER2->TASKS_COUNT = 1;        //Increment the timer
		nrf_delay_us(10);                   //Create 10us delay before incrementing the timer again
	}
}
