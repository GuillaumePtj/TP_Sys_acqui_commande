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

Moteur motor;

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
	//entrer une valeur entre -3000 et 3000

	speed = speed/30; //convertit en valeur entre -100 et 100

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

/*Mesure du courant*/

//Courant de phase de U_Imes sur PA1
//Courant de phase de V_Imes sur PB1
//Courant de phase de W_Imes sur PB0

uint32_t adc_value = 0;
uint16_t ADC_buffer;// [ADC_BUF_SIZE];
char courant[50];

void current_Init(void)
{
	if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED))
	{
		HAL_UART_Transmit(&huart2,"\t Erreur de calibration \r\n", 30, HAL_MAX_DELAY);
	}
	if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, &ADC_buffer, 1))
	{
		HAL_UART_Transmit(&huart2,"\t Erreur de DMA \r\n", 25, HAL_MAX_DELAY);
	}

	HAL_TIM_Base_Start(&htim1);
}

float current_Mesure(void){
	//Fonction de transfert du capteur
	// I = (1/Sn)(3300*Vm/4095 - V0)
		//avec Sn la sensibilité nominale 50 mv/A
		// Vm la valeur de la tension mesurée
		// V0 la tension d'alimentation 3.3/2 V

	float courant_A = 0.016*ADC_buffer -33;

	sprintf(courant, "Courant : %d et %f A \r\n", ADC_buffer, courant_A);
	HAL_UART_Transmit(&huart2, (uint8_t *)courant, strlen(courant), HAL_MAX_DELAY);
	return courant_A;
}


/*Mesure de la vitesse*/

//Mesure de l'encodeur A sur PA6
//Mesure de l'encodeur B sur PA4
//Mesure de l'encodeur Z sur PC8

uint32_t raw_speed;
uint32_t new_raw_speed;
uint32_t frequence;
char vitesse[50];

uint32_t speed_Mesure(void){
	raw_speed = __HAL_TIM_GET_COUNTER(&htim3);
	HAL_Delay(10);
	new_raw_speed = __HAL_TIM_GET_COUNTER(&htim3);
	frequence = (new_raw_speed - raw_speed)*735/495;

	if(frequence < 5000)
	{
		sprintf(vitesse, "Vitesse : %d rpm \r\n", -frequence);
		HAL_UART_Transmit(&huart2, (uint8_t *)vitesse, strlen(vitesse), HAL_MAX_DELAY);
	}
	else
	{
		frequence = -(new_raw_speed - raw_speed)*735/495;
		sprintf(vitesse, "Vitesse : %d rpm \r\n", frequence);
		HAL_UART_Transmit(&huart2, (uint8_t *)vitesse, strlen(vitesse), HAL_MAX_DELAY);
	}
	return frequence;
}

/* Asservissement de vitesse */

uint32_t PID_speed(int speed_command)
{
	int speed = speed_Mesure();
	int error = speed_command - speed;

	//P+I.Ts/(z-1) avec P = 0.305 et I = 0.064
	int P_speed = 0.305;
	int I_speed = 0.064;
	int Ts = 1/100;

	int sortie;
	sortie = P_speed + I_speed.Ts/(error-1);
	return sortie;
}


