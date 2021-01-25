#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"


/**
 * main.c
 */
//https://forum.43oh.com/topic/7171-using-hardware-pwm-on-tiva-launchpad/
uint32_t servo;
unsigned long ulPeriod;

int main(void)
{
    //----------------------------------------INICIALIZACION DEL RELOJ-------------------------------------------
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |  SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //---------------------------------------INICIALIZACION DE PERIFERICOS---------------------------------------
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    ulPeriod = SysCtlClockGet() / 50; //PWM frequency 50HZ

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_0,ulPeriod);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_0,ulPeriod/2);
    PWMGenEnable(PWM0_BASE,PWM_GEN_0);
    PWMOutputState(PWM0_BASE,PWM_OUT_0_BIT,true);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE3_AIN0);
    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3)
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE|ADC_CTL_END|ADC_CTL_CH0);
    ADCSequenceEnable(ADC0_BASE,0);

    //---------------------------------------SE INICIALIZAN UART0----------------------------------
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //---------------------------------------SETEAMOS LOS PINES DEL PUERTO A PARA COMUNICACION SERIAL--------------------
        GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
    //----------------------------------------CONFIGURACION DE RELOJ UART---------------------------------
        UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    ADCProcessorTrigger(ADC0_BASE,0);
    while(!ADCIntStatus(ADC0_BASE,0,false));
    ADCSequenceDataGet(ADC0_BASE, 0, servo);

	return 0;
}
