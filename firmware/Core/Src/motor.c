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

Moteur motor;


void motor_Init(void){

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
	motor.last_speed=512;


	motor.state = STATE_ON;
}
void motor_set_speed(int speed){
	if(motor.state == STATE_OFF) motor_Init();
	//entrer une valeur entre -100 et 100

	if(speed>100) speed=100;
	if(speed<-100) speed=-100;

	motor.speed=(int)speed*5.12+512;

	if(motor.last_speed<motor.speed){
		for (int i = motor.last_speed; i<=motor.speed; i++){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1024-i);
			HAL_Delay(10);
		}
	}
	else if (motor.last_speed>motor.speed){
		for (int i = motor.last_speed; i>=motor.speed; i--){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1024-i);
			HAL_Delay(10);
		}
	}

	motor.last_speed=motor.speed;
}

void motor_PID_speed();

void motor_PID_current();



