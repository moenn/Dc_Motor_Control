/*
 * my_keyboard.c
 *
 *  Created on: May 3, 2019
 *      Author: jms29
 */
#include "DSP28x_Project.h"
void InitKeyboard(void);

void InitKeyboard(void)
{
    //
    // GPIO 42-46 as input
    //
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO43 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO43 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 0x10;

    GpioCtrlRegs.GPBPUD.bit.GPIO44 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO44 = 0x10;

    GpioCtrlRegs.GPBPUD.bit.GPIO45 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO45 = 0x10;

    GpioCtrlRegs.GPBPUD.bit.GPIO46 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO46 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO46 = 0x10;

    GpioCtrlRegs.GPBPUD.bit.GPIO42 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO42 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO42 = 0x10;


    GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 42;
    GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 43;
    GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 44;
    GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 45;
    GpioIntRegs.GPIOXINT7SEL.bit.GPIOSEL = 46;

    // gpio 40 to gpio 47 sampling peroid 510 * SYSCLKOUT
    GpioCtrlRegs.GPBCTRL.bit.QUALPRD1 = 0xFF;

    EDIS;

    //
    // Configure
    //
    XIntruptRegs.XINT3CR.bit.POLARITY = 0;      // Falling edge interrupt
    XIntruptRegs.XINT4CR.bit.POLARITY = 0;
    XIntruptRegs.XINT5CR.bit.POLARITY = 0;
    XIntruptRegs.XINT6CR.bit.POLARITY = 0;
    XIntruptRegs.XINT7CR.bit.POLARITY = 0;
    //
    // Enable XINT
    //
    XIntruptRegs.XINT3CR.bit.ENABLE = 1;
    XIntruptRegs.XINT4CR.bit.ENABLE = 1;
    XIntruptRegs.XINT5CR.bit.ENABLE = 1;
    XIntruptRegs.XINT6CR.bit.ENABLE = 1;
    XIntruptRegs.XINT7CR.bit.ENABLE = 1;
    //

    // Enable pie group 12 - xint3 xint4 xint5 xin6 xint7
    //
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable  XINT3
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // XINT4
    PieCtrlRegs.PIEIER12.bit.INTx3 = 1;          // XINT5
    PieCtrlRegs.PIEIER12.bit.INTx4 = 1;          // XINT6
    PieCtrlRegs.PIEIER12.bit.INTx5 = 1;          // XINT7
    IER |= M_INT12;  // enable cpu group 12 interrupt
}
