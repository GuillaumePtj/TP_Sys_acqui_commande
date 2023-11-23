/*
 * motor.h
 *
 *  Created on: Oct 10, 2023
 *      Author: grego
 */

#ifndef SRC_MOTOR_H_
#define SRC_MOTOR_H_

#define STATE_ON 1
#define STATE_OFF 0

#define ZERO_MOTOR_SPEED 512

typedef struct Moteur{
	int state;
	int speed;
	int last_speed;
}Moteur;

void Motor_Set_Speed(int speed);
void Motor_Stop(void);
void Motor_Init(void);
float Current_Mesure(void);
uint32_t Speed_Mesure(void);
void Current_Init(void);

uint32_t PID_Speed(int speed_command);

#endif /* SRC_MOTOR_H_ */
