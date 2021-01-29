#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/mpu6050.h"
#include <math.h>

//#include "uart.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
/**
 * main.c
 */
float fAccel[3], fGyro[3];
volatile bool g_bMPU6050Done;
tI2CMInstance g_sI2CMSimpleInst;
///---------------Se tomo la inicializacion del MPU6050 del manual de TIVAWARE-----------------------------
//https://www.ti.com/lit/ug/spmu371e/spmu371e.pdf?ts=1611376130603&ref_url=https%253A%252F%252Fwww.google.com%252F
//https://github.com/mathmagson/mpu6050_tm4c123g/blob/master/main.c
tI2CMInstance g_sI2CMSimpleInst;

void ConfigureUART(void) { // Fun��o retirada do exemplo hello.c
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    //UARTStdioConfig(0, 115200, 16000000);

}
void I2CInit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    //clear I2C FIFOs
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
    I2CMInit(&g_sI2CMSimpleInst, I2C0_BASE, INT_I2C0, 0xff, 0xff, SysCtlClockGet());
}
void
I2CMSimpleIntHandler(void)
{
//
// Call the I2C master driver interrupt handler.
//
I2CMIntHandler(&g_sI2CMSimpleInst);
}
//
// A boolean that is set when a MPU6050 command has completed.
//

//
// The function that is provided by this example as a callback when MPU6050
// transactions have completed.
//


void MPU6050Callback(void *pvCallbackData, uint_fast8_t ui8Status)
    {
//
// See if an error occurred.
//
    if(ui8Status != I2CM_STATUS_SUCCESS)
    {
        //
        // An error occurred, so handle it here if required.
        //
    }
//
// Indicate that the MPU6050 transaction has completed.
//
    g_bMPU6050Done = true;
    }
//
// The MPU6050 example.
//

void MPU6050Example(void)
    {
        float fAccel[3], fGyro[3];

        tMPU6050 sMPU6050;
//
// Initialize the MPU6050. This code assumes that the I2C master instance
// has already been initialized.
//
        g_bMPU6050Done = false;
        MPU6050Init(&sMPU6050, &g_sI2CMSimpleInst, 0x68, MPU6050Callback,&sMPU6050);

        while(!g_bMPU6050Done)
            {
            }
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
//
// Configure the MPU6050 for +/- 4 g accelerometer range.
//
        g_bMPU6050Done = false;
        MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_ACCEL_CONFIG,
                                ~MPU6050_ACCEL_CONFIG_AFS_SEL_M,
                                MPU6050_ACCEL_CONFIG_AFS_SEL_4G, MPU6050Callback,&sMPU6050);
        while(!g_bMPU6050Done)
            {
            }
//
// Loop forever reading data from the MPU6050. Typically, this process
// would be done in the background, but for the purposes of this example,
// it is shown in an infinite loop.
//
        while(1)
        {
//
// Request another reading from the MPU6050.
//
            g_bMPU6050Done = false;
            MPU6050DataRead(&sMPU6050, MPU6050Callback, &sMPU6050);
            while(!g_bMPU6050Done)
            {
            }
            //
            // Get the new accelerometer and gyroscope readings.
            //
            MPU6050DataAccelGetFloat(&sMPU6050, &fAccel[0], &fAccel[1],
            &fAccel[2]);
            MPU6050DataGyroGetFloat(&sMPU6050, &fGyro[0], &fGyro[1], &fGyro[2]);
            //
            // Do something with the new accelerometer and gyroscope readings.
            //
        }
    }

int main()
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |  SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //---------------------------------------INICIALIZACION DE PERIFERICOS---------------------------------------
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlIntEnable();
    //---------------------------------------SETEO DE PINES COMO SALIDA (LEDS)----------------------------------
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    I2CInit();
    MPU6050Example();
    return(0);
}