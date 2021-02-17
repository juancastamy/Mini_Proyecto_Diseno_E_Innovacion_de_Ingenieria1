/*
 * Code created using CCS v6 with TI compiler. Used TivaWare.
 * This code configures a Wide Timer to output a PWM of 50Hz.
 * By changing the duty it's possible to control a servo position
 */
/*
 * main.h
 *
 *  Created on: Aug 14, 2015
 *      Author: secakmak
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
#include "driverlib/adc.h"

#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "DelayTimer.h"

#define Period  320000 //(16000000/50) 50Hz
#define SERVO_STEPS         180     // Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN           11500//9500     // The minimum duty cycle for this servo
#define SERVO_MAX           40100//35100    // The maximum duty cycle

unsigned int servo_lut[SERVO_STEPS+1];
volatile uint32_t giro;
uint32_t COUNT[1];
//-------------SERVO DRIVER--------------------------------------------------------------------------------------------------------
void InitServo(void) //PC5
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinConfigure(GPIO_PC5_WT0CCP1);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);
    TimerLoadSet(WTIMER0_BASE, TIMER_B, (Period-1));
    TimerMatchSet(WTIMER0_BASE, TIMER_B, (Period-9600));

    TimerMatchSet(WTIMER0_BASE, TIMER_B, 1);
    TimerEnable(WTIMER0_BASE, TIMER_B);
}

void ConfigureServo(void)
{
    unsigned int servo_stepval, servo_stepnow;
    unsigned int i;

    servo_stepval   = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
    servo_stepnow   = SERVO_MIN;

    for (i = 0; i < (SERVO_STEPS+1); i++)
    {
        servo_lut[i] = (Period-servo_stepnow);
        servo_stepnow += servo_stepval;
    }
}

void SetServoPosition(uint32_t position)
{
    TimerMatchSet(WTIMER0_BASE, TIMER_B, position);
}

void SetServoAngle(uint32_t angle)
{
    SetServoPosition(servo_lut[angle]);
}
//------------------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);

    timerInit();

    InitServo();
    ConfigureServo();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTB_BASE,GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE,3);
    IntMasterEnable();
    SetServoAngle(0);

    delay(1000);

    while(1){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false));
        ADCSequenceDataGet(ADC0_BASE, 3, COUNT);
        giro=COUNT[0]*180/4095;
        SetServoAngle(180);

    }

}
