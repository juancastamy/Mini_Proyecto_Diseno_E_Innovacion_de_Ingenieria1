/*
 * PWM.c
 *
 *  Created on: 17/02/2021
 *      Author: juan
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "utils/cmdline.h"
#include "driverlib/interrupt.h"
#include "PWM.h"

//---------------------------------------------INICIALIZACION PWM----------------------------------------------------
void InitServo(void) //Se inicializa la señal del PWM que se controlara con un timer 0 en el puerto PC5
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //Se inicializa el puerto C del controlador
    GPIOPinConfigure(GPIO_PC5_WT0CCP1);//Se configura el pin C5 como WT0CCP1, el cual permite una señal
                                       //PWM controlada con el Timer 0, el cual varia el Duty Cycle para esto
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);//Se configura el Pin C5 Como salida del Duty Cycle del Timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);//Se activa el Timer 0 para que saque la señal por un pin
                                                  // y poder utilizarla como PWM
    TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);//se configura el Timer 0, en este caso se
                                                                       //utilizara el lado B del timer0
    TimerLoadSet(WTIMER0_BASE, TIMER_B, (Period-1));//se especifica el periodo del timer0
    TimerMatchSet(WTIMER0_BASE, TIMER_B, (Period-9600));//se configura la variacion del duty cycle del timer

    TimerMatchSet(WTIMER0_BASE, TIMER_B, 1);
    TimerEnable(WTIMER0_BASE, TIMER_B);
}

//-----------------------------------------CONFIGURACION A GRADOS--------------------------------------------------
void ConfigureServo(void)
{
    unsigned int servo_stepval, servo_stepnow;// se especifican variable a utilizar
    unsigned int i;

    servo_stepval   = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );// se realiza una convesion para setear a grados
                                                                // los movimientos del servo con el PWM
    servo_stepnow   = SERVO_MIN;//se conloca el valor minimo del servo en la variable servo_stepnow

    for (i = 0; i < (SERVO_STEPS+1); i++)
    {
        servo_lut[i] = (Period-servo_stepnow);//se realiza un array para mapear los datos del servo
        servo_stepnow += servo_stepval;//se actualiza el valor de stepnow
    }
}
//--------------------------------------UPLOAD DEL DATO AL SERVO-------------------------------------------------
void SetServoPosition(uint32_t position)
{
    TimerMatchSet(WTIMER0_BASE, TIMER_B, position);//se envia el valor que se coloque en position al servo
}

void SetServoAngle(uint32_t angle)
{
    SetServoPosition(servo_lut[angle]);//se eniva la posicion del angulo a la variable servo_lut
}

