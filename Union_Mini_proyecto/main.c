#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "sensorlib/i2cm_drv.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/mpu6050.h"

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"

#include "PWM.h"
#include "MPU6050.h"

#include <math.h>

/**
 * main.c
 */
float x,y,z;
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
    InitServo();//Se inicializa el PWM
    ConfigureServo();//Se configura el servo
    SetServoAngle(90);//Se coloca el servo en 90 grados
    InitI2C0();
    MPU6050INIT();
    while(1)
    {
        MPU6050READDATA(&x,&y,&z);
        //SetServoAngle(uint32_t angle)
    }
}
