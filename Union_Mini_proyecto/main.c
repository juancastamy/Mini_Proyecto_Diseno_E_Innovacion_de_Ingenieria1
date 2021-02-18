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

#include "driverlib/uart.h"
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

#include "utils/uartstdio.h"
#include "uartstdio.h"
#include "PWM.h"
#include "MPU6050.h"

#include <math.h>

/**
 * main.c
 */
//se obtuvo la libreria de uart para itulizar UARTprintf de https://github.com/YashBansod/ARM-TM4C-CCS/blob/master/TM4C123G%20LaunchPad%20UART%20Stdio/main.c
///---------------Se tomo la inicializacion del MPU6050 del manual de TIVAWARE-----------------------------
//https://www.ti.com/lit/ug/spmu371e/spmu371e.pdf?ts=1611376130603&ref_url=https%253A%252F%252Fwww.google.com%252F
//https://github.com/mathmagson/mpu6050_tm4c123g/blob/master/main.c
//https://e2e.ti.com/support/microcontrollers/other/f/908/t/683082?tisearch=e2e-sitesearch&keymatch=CCS%252FTM4C123GH6PM%2520MPU6050
//https://e2e.ti.com/support/tools/ccs/f/81/t/754471?CCS-EK-TM4C123GXL-How-to-add-interrupt-handlers-in-CCS-
//https://e2e.ti.com/support/microcontrollers/other/f/908/t/381843
//https://e2e.ti.com/support/microcontrollers/other/f/908/t/564858?CCS-MPU6050-Sensorlib
//---------------se tomo la inicializacion del PWM del manual de TIVAWARE -------------------------------------
//https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-general-purpose-timers/wide-timer-pwm---servo-control---example
//https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-general-purpose-timers/servo-control-in-any-gpio
//https://github.com/YashBansod/ARM-TM4C-CCS/blob/master/TM4C123G%20LaunchPad%20PWM%20using%20GP%20Timer/main.c
//--------------------------------SE DEFINEN LAS VARIABLES A UTILIZAR EN EL PID--------------------------------------
float ek;
float entrada=90;
float ek_1 = 0;
float Ek_1 = 0;
float ed = 0;
float Ek = 0;
float uk;
float IN;
//--------------------------------SE DEFINEN LAS COSNTANTES DEL PID----------------------------------------------------
float Kp=1; //1;
float Ki=0.0001;
float Kd=8;
float x,y,z;

void ConfigureUART(void) { // Função retirada do exemplo hello.c
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );
    UARTStdioConfig(0, 115200, SysCtlClockGet());
}

void control_pid (float *uk, float x)
{
    ek = entrada - x;
    ed = ek - ek_1;
    Ek = Ek_1+ek;
    Ek_1=Ek;
    ek_1=ek;
    *uk = Kp*ek + Ki*Ek + Kd*ed;
}


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
    InitServo();//Se inicializa el PWM
    ConfigureServo();//Se configura el servo
    SetServoAngle(90);//Se coloca el servo en 90 grados
    InitI2C0();
    MPU6050INIT();
    ConfigureUART();
    while(1)
    {
        MPU6050READDATA(&x,&y,&z);
        //UARTprintf("Ang. X: %d | Ang. Y: %d | Ang. Z: %d\n", (int)x, (int)y, (int)z);
        control_pid(&uk, (x));
        SetServoAngle(uk);
        UARTprintf("Ang. uk: %d | Ang. x: %d \n", (int)uk, (int)(x));
    }
}



