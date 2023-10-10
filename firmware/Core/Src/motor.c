/*
 * motor.c
 *
 *  Created on: Oct 10, 2023
 *      Author: grego
 */

#include "usart.h"
#include "motor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tim.h"

int verifSpeed (int a)
{
	if (a>1)
	{
		HAL_UART_Transmit(&huart2, "Error\r\n", 7, HAL_MAX_DELAY);
		return 0;
	}
	else
	{
		return 1;
	}
}



int Motor_Loop(void){

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SpeedValue*2832/1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (1-SpeedValue)*2832/1000);
}



