/*
 * MPU6050.h
 *
 *  Created on: 17/02/2021
 *      Author: juan
 */


#ifndef MPU6050_H_
#define MPU6050_H_

//
// A boolean that is set when a MPU6050 command has completed.
//
volatile bool g_bMPU6050Done;

//
// I2C master instance
//
tI2CMInstance g_sI2CMSimpleInst;

//
//Device frequency
//
float fAccel[3], fGyro[3];
tMPU6050 sMPU6050;


void InitI2C0(void);
void MPU6050Callback(void *pvCallbackData, uint_fast8_t ui8Status);
void I2CMSimpleIntHandler(void);
void MPU6050INIT(void);
void MPU6050READDATA (float *x, float *y, float *z);

#endif /* MPU6050_H_ */
