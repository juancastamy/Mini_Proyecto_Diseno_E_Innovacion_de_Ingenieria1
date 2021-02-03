#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "inc/hw_gpio.h"


/**
 * main.c
*/
//https://forum.43oh.com/topic/7171-using-hardware-pwm-on-tiva-launchpad/
//https://github.com/mathmagson/microservo9g_tm4c123g/blob/master/main.c
//https://www.ti.com/seclit/ml/ssqu015/ssqu015.pdf
//https://github.com/YashBansod/ARM-TM4C-CCS/blob/master/TM4C123G%20LaunchPad%20ADC%20Potentiometer/main.c
//https://github.com/ntua-cslep/Motor-Control-with-Tiva-EK-TM4C123GXL
volatile uint32_t servo;

uint32_t COUNT[1];
uint32_t giro;
unsigned long ulPeriod;
volatile uint32_t load;
int main(void)
{
    giro=75;
    //----------------------------------------INICIALIZACION DEL RELOJ-------------------------------------------
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL |  SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //---------------------------------------INICIALIZACION DE PERIFERICOS---------------------------------------
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //---------------------------------------SETEO DE PINES COMO SALIDA (LEDS)----------------------------------
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

//----------------------------------PWM-----------------------------------------------
//--------------------INISCIALIZACION DEL SISTEMA-------------------------------------

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);//se activa el perifericos PWM1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);//se activa el periferico D


    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

//----------------------Se configura el  modo del pin---------------------------------
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); // se configura el pin PD0 como salidad de PWM
    GPIOPinConfigure(GPIO_PD0_M1PWM0);//se configura el pin PD0 como PWM1PWM0

//-------------------------Se desactiva el M0PWM0-----------------------------------

    ulPeriod = SysCtlClockGet() / 64; //se divide el reloj del sistema

    load=(ulPeriod/50)-1;//se divide el periodo por la frecuencia deseada del PWM y se realiza el ajuste con -1
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0,PWM_GEN_MODE_DOWN);//se configura el PWM para contar de un numero haca abajo
    PWMGenPeriodSet(PWM1_BASE,PWM_GEN_0,/*load*/load);//se especifica el periodo del PWM
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, giro*load/1000);

    PWMOutputState(PWM1_BASE,PWM_OUT_0_BIT,true); //se coloca el PWM como salida
    PWMGenEnable(PWM1_BASE,PWM_GEN_0);//se activa el PWM

//--------------------------------ADC-------------------------------------------------------------------
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTB_BASE,GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE,3);
    IntMasterEnable();

    while(1){

        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false));
        ADCSequenceDataGet(ADC0_BASE, 3, COUNT);
        if(COUNT[0]>=1 && COUNT[0]<1365){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
        }
        if(COUNT[0]>=1365 && COUNT[0]<=2730){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
        }
        if(COUNT[0]>2730 && COUNT[0]<=4096){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
        }
        giro=(((125-35)/4095)*COUNT[0])+35;
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, giro*load/1000); // SETA O DUTY CYCLE DO PWM
        SysCtlDelay(1000);
    }
	return 0;
}

/*
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 83;

    //SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // CLOCK 40 MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // CLOCK 80 MHZ
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // SETA O CLOCK DO PWM POR: CLOCK_CPU/64

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // HABILITA PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // HABILITA PD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // HABILITA PF

    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); // DEFINIE PWM NO PINO PD0
    GPIOPinConfigure(GPIO_PD0_M1PWM0); // CONFIGURA PD0 COMO M1PWM0

    // INICIO HABILITA CHAVES SW1 E SW2
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    // FIM HABILITA CHAVES SW1 E SW2

    ui32PWMClock = SysCtlClockGet() / 64; // PEGA CLOCK DEFINIDO PARA O PWM
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1; // PEGA O CLOCK DO PWM E DIVIDE PELO CLOCK COM QUE SE QUER TRABALHAR SUBTRAINDO 1 POR CONTAR AT� 0
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); // CONFIGURA O CONTADOR COMO DECRESCENTE
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load); // SETA O CONTADOR

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000); // DIVIDE-SE O CONTADOR POR 1000 E FAZ A MULTIPLICACAO PARA AJUSTE PARA 1.51ms (MEIO DA POSICAO DO SERVO)
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true); // CONFIGURA PWM Module 1 COMO SAIDA
    PWMGenEnable(PWM1_BASE, PWM_GEN_0); // HABILITA GERACAO DE PWM

    while(1)
    {

        if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) // VERIFICA SE O PINO PF4 FOI PRESSIONADO
        {
            ui8Adjust--;
            if (ui8Adjust < 56)
            {
                ui8Adjust = 56; // TRAVA NO LIMITE DE 1mS
            }
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000); // SETA O DUTY CYCLE DO PWM
        }

        if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)  // VERIFICA SE O PINO PF0 FOI PRESSIONADO
        {
            ui8Adjust++;
            if (ui8Adjust > 111)
            {
                ui8Adjust = 111; // TRAVA NO LIMITE DE 2mS
            }
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000); // SETA O DUTY CYCLE DO PWM
        }

        SysCtlDelay(100000); // SETA A VELOCIDADE DA REPETICAO SOMENTE
    }

}
*/