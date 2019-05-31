/*
 * my_motor.c
 *
 *  Created on: May 29, 2019
 *      Author: jms29
 */
#include "DSP28x_Project.h"
#define IN1 GPIO32
#define IN2 GPIO33

#define TRUE 1
#define FALSE 0

void InitMotor(void)
{
    EALLOW;
    // init gpio, and stop state
    GpioCtrlRegs.GPBPUD.bit.IN1 = 0;
    GpioDataRegs.GPBCLEAR.bit.IN1 = 1;
    GpioCtrlRegs.GPBMUX1.bit.IN1 = 0;
    GpioCtrlRegs.GPBDIR.bit.IN1 = 1;

    GpioCtrlRegs.GPBPUD.bit.IN2 = 0;
    GpioDataRegs.GPBCLEAR.bit.IN2 = 1;
    GpioCtrlRegs.GPBMUX1.bit.IN2 = 0;
    GpioCtrlRegs.GPBDIR.bit.IN2 = 1;
    EDIS;
}

void MotorForward(void)
{
    GpioDataRegs.GPBCLEAR.bit.IN1 = 1;
    GpioDataRegs.GPBSET.bit.IN2 = 1;
}

void MotorStart(void)
{
    MotorForward();
}

void MotorStop(void)
{
    GpioDataRegs.GPBCLEAR.bit.IN1 = 1;
    GpioDataRegs.GPBCLEAR.bit.IN2 = 1;
}


void MotorReverse(void)
{
    GpioDataRegs.GPBSET.bit.IN1 = 1;
    GpioDataRegs.GPBCLEAR.bit.IN2 = 1;
}


int IsMotorRunning(void)
{
    if((GpioDataRegs.GPBDAT.bit.IN1 == 1 && GpioDataRegs.GPBDAT.bit.IN2 == 0)
            || (GpioDataRegs.GPBDAT.bit.IN1 == 0 && GpioDataRegs.GPBDAT.bit.IN2 == 1))
    {
        return TRUE;
    }

    if((GpioDataRegs.GPBDAT.bit.IN1 == 0 && GpioDataRegs.GPBDAT.bit.IN2 == 0)
               || (GpioDataRegs.GPBDAT.bit.IN1 == 1 && GpioDataRegs.GPBDAT.bit.IN2 == 1))
    {
        return FALSE;
    }

    return FALSE;
}

int IsMotorForward(void)
{
    if(GpioDataRegs.GPBDAT.bit.IN1 == 0 && GpioDataRegs.GPBDAT.bit.IN2 == 1){
        return TRUE;
    }else{
        return FALSE;
    }
}

int IsMotorReverse(void)
{
    if(GpioDataRegs.GPBDAT.bit.IN1 == 1 && GpioDataRegs.GPBDAT.bit.IN2 == 0){
        return TRUE;
    }else{
        return FALSE;
    }
}

void EnableMotorStart(void)
{
    // enable xint3
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1;
}

void DisableMotorStart(void)
{
    // disable xint3
    PieCtrlRegs.PIEIER12.bit.INTx1 = 0;
}

void EnablePwmChange(void)
{
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable  XINT3
    PieCtrlRegs.PIEIER12.bit.INTx3 = 1;          // XINT4
}

void DisablePwmChange(void)
{
    PieCtrlRegs.PIEIER12.bit.INTx2 = 0;          // Enable  XINT3
    PieCtrlRegs.PIEIER12.bit.INTx3 = 0;          // XINT4
}

