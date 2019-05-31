/*
 * my_ecap.h
 *
 *  Created on: May 3, 2019
 *      Author: jms29
 */
#include "DSP28x_Project.h"
#ifndef MY_ECAP_H_
#define MY_ECAP_H_

void InitECapture()
{
    ECap1Regs.ECEINT.all = 0x0000;          // Disable all capture interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;           // Clear all CAP interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

    //
    // Configure peripheral registers
    //
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;   // continus
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;     // Stop at 4 events
    ECap1Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP3POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 0;       //
    ECap1Regs.ECCTL1.bit.CTRRST2 = 0;       //
    ECap1Regs.ECCTL1.bit.CTRRST3 = 0;       //
    ECap1Regs.ECCTL1.bit.CTRRST4 = 0;       //
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;      // Enable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0x2;     // Pass through
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
//    ECap1Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads
    ECap1Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = interrupt
}




#endif /* MY_ECAP_H_ */
