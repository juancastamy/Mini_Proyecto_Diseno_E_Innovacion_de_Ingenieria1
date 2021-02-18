/*
 * PWM.h
 *
 *  Created on: 17/02/2021
 *      Author: juan
 */

#ifndef PWM_H_
#define PWM_H_

#define Period  320000 //(16000000/50) 50Hz
#define SERVO_STEPS         180     // Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN           5000//9500     // The minimum duty cycle for this servo
#define SERVO_MAX           28800//35100    // The maximum duty cycle

unsigned int servo_lut[SERVO_STEPS+1];

void InitServo(void);;
void ConfigureServo(void);
void SetServoPosition(uint32_t position);
void SetServoAngle(uint32_t angle);

#endif /* PWM_H_ */
