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

typedef struct Moteur{
	int state;
	int speed;
	int last_speed;
}Moteur;

void motor_set_speed(int speed);
void motor_Stop(void);
void motor_Init(void);

#endif /* SRC_MOTOR_H_ */
