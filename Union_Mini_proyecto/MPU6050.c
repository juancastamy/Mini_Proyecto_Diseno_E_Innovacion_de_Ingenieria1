/*
 * MPU6050.c
 *
 *  Created on: 17/02/2021
 *      Author: juan
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/mpu6050.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include <math.h>
#include "MPU6050.h"

//------------------------------INICIALIZACION DE I2C PARA FUNCIONAMIENTO DE MAESTRO CON SENSORLIB---------------------------------------
void InitI2C0(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);//Se inicializa el periferico de I2C 0
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);//Se restea el periferico I2C0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//Se activa el periferico para los pines del I2C0
                                                //en este caso es el puerto B
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);//se configura el pin B2 como SCL del I2C0
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);//se configura el pin B3 como SDA del I2C0
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);//Se selecciona la funcion del Pin B2
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);//Se selecciona la funcion del Pin B3
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);//se iniciliza el I2C0 como master
                                                           //se configura el reloj con el reloj del sistema y se configura una velocidad
                                                           //de 400kbps para la transmicion de datos
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;//se limpia el FIFO del I2C0
    I2CMInit(&g_sI2CMSimpleInst, I2C0_BASE, INT_I2C0, 0xff, 0xff, SysCtlClockGet());//Se inicializa el driver del I2C como master
}
//-----------------------------CALLBACK PARA MODULO MPU6050-------------------------------------------------
void MPU6050Callback(void *pvCallbackData, uint_fast8_t ui8Status)
{
    if (ui8Status != I2CM_STATUS_SUCCESS)//verifica si sucede error
    {
    }
    g_bMPU6050Done = true;//La variable se vuelve true cuando se completa la transmicion de datos
}
//------------------------------INTERRUPCION DEL I2C0------------------------------------------------------------
void I2CMSimpleIntHandler(void)
{
    I2CMIntHandler(&g_sI2CMSimpleInst);//Llama la interrupcion del I2C
}
//------------------------------INICIALIZACION DEL MODULO MPU6050--------------------------------------------
void MPU6050INIT(void)
{
    g_bMPU6050Done = false;//se coloca la variable en false para realizar el callback
    MPU6050Init(&sMPU6050, &g_sI2CMSimpleInst, 0x68, MPU6050Callback, &sMPU6050);//se activa el modulo MPU6050
                                                                                 //el primer valor ingresa al grupo MPU6050
                                                                                 //El segundo valor es la activacion de la interrupcion I2C0 en modo maestro
                                                                                 //El terser valor es la direccion del MPU6050
                                                                                 //Es el llamado del callback para verificar si termino el proceso
                                                                                 //Es la variable que se manda al callback para esperrar si ocurre error o no
    while (!g_bMPU6050Done) //se espera que la variable MPU6050Done no sea false
    {
    }

    //
    // Configure the MPU6050 for +/- 4 g accelerometer range.
    //
    g_bMPU6050Done = false;//se coloca la variable en false para realizar el callback
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_ACCEL_CONFIG, ~MPU6050_ACCEL_CONFIG_AFS_SEL_M,
        MPU6050_ACCEL_CONFIG_AFS_SEL_4G, MPU6050Callback, &sMPU6050);//Se configura ACCELEROMETRO
                                                                     //Primer valor entra al registro de MPU6050
                                                                     //El segundo valor es la dirrecion del registro que se desea configurar
                                                                     //El tercer valor realiza un and con la configuracion actual colocada volviendo todos los bits 0
                                                                     //El cuarto valor configura el accelerometro como se desea que trabaje, en este caso esta trabajando a 4g
                                                                     //El quinto valor es el llamado del callback para verificar si termino el proceso
                                                                     //El sexto es la variable que se manda al callback para esperrar si ocurre error o no
    while (!g_bMPU6050Done)//se espera que la variable MPU6050Done no sea false
    {
    }


    g_bMPU6050Done = false;//se coloca la variable en false para realizar el callback
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_PWR_MGMT_1, 0x00,
                           0b00000010 & MPU6050_PWR_MGMT_1_DEVICE_RESET, MPU6050Callback, &sMPU6050);//Se configura el PWR_MGMT_1
                                                                                                     //Primer valor entra al registro de MPU6050
                                                                                                     //El segundo valor es la dirrecion del registro que se desea configurar
                                                                                                     //El tercer valor realiza un and con la configuracion actual colocada volviendo todos los bits 0
                                                                                                     //El cuarto valor configura a la que se desea tabajar, en este caso la frecuencia esta especificado por el
                                                                                                     // cambio del acelerometro
                                                                                                     //El quinto valor es el llamado del callback para verificar si termino el proceso
                                                                                                     //El sexto es la variable que se manda al callback para esperrar si ocurre error o no
    while (!g_bMPU6050Done)//se espera que la variable MPU6050Done no sea false
    {
    }

    g_bMPU6050Done = false;//se coloca la variable en false para realizar el callback
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_PWR_MGMT_2, 0x00, 0x00, MPU6050Callback, &sMPU6050);//Se configura el PWR_MGMT_2
                                                                                                    //Primer valor entra al registro de MPU6050
                                                                                                    //El segundo valor es la dirrecion del registro que se desea configurar
                                                                                                    //El tercer valor realiza un and con la configuracion actual colocada volviendo todos los bits 0
                                                                                                    //El cuarto valor configura a la que se desea tabajar, en este caso se setea en 0 ya que no se utiliza el modo SLEEP
                                                                                                    //El quinto valor es el llamado del callback para verificar si termino el proceso
                                                                                                    //El sexto es la variable que se manda al callback para esperrar si ocurre error o no
    while (!g_bMPU6050Done)//se espera que la variable MPU6050Done no sea false
    {
    }
}
//------------------------------------------------FUNCION PARA LEER DATOS---------------------------------------------
void MPU6050READDATA (float *x, float *y, float *z)
{
    g_bMPU6050Done = false;
    MPU6050DataRead(&sMPU6050, MPU6050Callback, &sMPU6050);//solicita realizar una lectura del MPU6050
    while (!g_bMPU6050Done)//espera que este listo para realizar la lectura
    {
    }
    MPU6050DataAccelGetFloat(&sMPU6050, &fAccel[0], &fAccel[1], &fAccel[2]);//Se obtienen los datos del accelerometro en X,Y y Z
    MPU6050DataGyroGetFloat(&sMPU6050, &fGyro[0], &fGyro[1], &fGyro[2]);//Se obtienen los datos del giroscopio en X,Y y Z
    *z = fGyro[2];//es el giro(roll) en el eje Z
    *x = (atan2(fAccel[0], sqrt (fAccel[1] * fAccel[1] + fAccel[2] * fAccel[2]))*180.0)/3.14;//Es el giro(roll) en el eje x
    *y = (atan2(fAccel[1], sqrt (fAccel[0] * fAccel[0] + fAccel[2] * fAccel[2]))*180.0)/3.14;//es el giro(roll) en el eje y
}
