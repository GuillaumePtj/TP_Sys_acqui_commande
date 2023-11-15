/*
 * motor.c
 *
 *  Created on: Oct 10, 2023
 *      Author: grego
 */

#include "adc.h"
#include "usart.h"
#include "motor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tim.h"

#define ADC_BUF_SIZE 8


Moteur motor;
uint32_t adc_value = 0;
uint16_t ADC_buffer [ADC_BUF_SIZE];
char courant[50];



void motor_Init(void){

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
	motor.last_speed=512;


	motor.state = STATE_ON;
}

void motor_Stop(void){
	motor_set_speed(0);

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

	motor.state = STATE_OFF;
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

//Courant de phase de U_Imes sur PA1
//Courant de phase de V_Imes sur PB1
//Courant de phase de W_Imes sur PB0

void current_Mesure(void){
	HAL_ADC_Start_DMA(&hadc1, ADC_buffer, ADC_BUF_SIZE);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adc_value = HAL_ADC_GetValue(&hadc1);

	sprintf(courant, "Courant : %d mA \r\n", adc_value);
	HAL_UART_Transmit(&huart2, (uint8_t *)courant, strlen(courant), HAL_MAX_DELAY);
}

//Mesure de l'encodeur A sur PA6
//Mesure de l'encodeur B sur PA4
//Mesure de l'encodeur Z sur PC8

uint32_t raw_speed;
uint32_t new_raw_speed;
uint32_t frequence;
char vitesse[50];

void speed_Mesure(void){
	while(1){
		raw_speed = __HAL_TIM_GET_COUNTER(&htim3);
		HAL_Delay(10);
		new_raw_speed = __HAL_TIM_GET_COUNTER(&htim3);
		frequence = (new_raw_speed - raw_speed)*735/495;
		sprintf(vitesse, "Vitesse : %d rpm \r\n", frequence);
		HAL_UART_Transmit(&huart2, (uint8_t *)vitesse, strlen(vitesse), HAL_MAX_DELAY);
	}

}

void motor_PID_speed();

void motor_PID_current();



