#include <string.h>
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "my_keyboard.h"
#include "my_ecap.h"
#include "my_oled.h"
#include "my_motor.h"

#define EPWM1_TIMER_TBPRD  2000  // Period register
#define EPWM1_MAX_CMPA     1950
#define EPWM1_MIN_CMPA     800
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB     800

#define KP 2
#define KI 0.5
#define KD 0.1

#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0


typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
} EPWM_INFO;

EPWM_INFO epwm1_info;

// Globals
Uint32  ECap1IntCount;
Uint32 temp[3] = {};
Uint32 peroid = 0;

Uint32 current_rpm = 0;
Uint32 desired_rpm = 160;
char direction[20] = "none";
char status[20] = "stop";


Uint32 cmpa = 0;
Uint32 cmpb = 0;

//
// Defines that configure the period for each timer
//

int error=0;
int error1=0;
int error2=0;
int duk=0;
//
// Defines that keep track of which way the compare value is moving
//

int xint3_count=0;
int xint4_count=0;
int xint5_count=0;
int xint6_count=0;
int xint7_count=0;

// Function Prototypes

void InitEPwm1Example(void);
void UpdateCompare(EPWM_INFO *epwm_info, int change);

void ResetCompare(EPWM_INFO *epwm_info);



void OledRefreshValue(void);

void InitECapture(void);

void MainLoop(void);
void PidControl(Uint32 desired_rpm, Uint32 current_rpm);
void InitPid(void);
void ResetPid(void);

__interrupt void ecap1_isr(void);
__interrupt void epwm1_isr(void);

__interrupt void xint3_isr(void);
__interrupt void xint4_isr(void);
__interrupt void xint5_isr(void);
__interrupt void xint6_isr(void);
__interrupt void xint7_isr(void);

__interrupt void cpu_timer0_isr(void);

void OledCreateMenu(void);

//
// Main
//
void main(void)
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    //     init display
    InitOledGpio();
    InitOled();
    OledCreateMenu();

    InitECap1Gpio();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm1Gpio();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    InitPid();
    InitEPwm1Example();
    InitECapture();
    InitKeyboard();
    InitMotor();
    InitCpuTimers();

    // 10ms
    ConfigCpuTimer(&CpuTimer0, 150, 100000);
    CpuTimer0Regs.TCR.all = 0x4000; //write-only instruction to set TSS bit = 0



    ECap1IntCount = 0;

    // PIE vector table
    EALLOW;
    PieVectTable.ECAP1_INT = &ecap1_isr;
    PieVectTable.EPWM1_INT = &epwm1_isr;

    PieVectTable.TINT0 = &cpu_timer0_isr;

    PieVectTable.XINT3 = &xint3_isr;
    PieVectTable.XINT4 = &xint4_isr;
    PieVectTable.XINT5 = &xint5_isr;
    PieVectTable.XINT6 = &xint6_isr;
    PieVectTable.XINT7 = &xint7_isr;
    EDIS;

    // Cpu interrupts enable
    IER |= M_INT1;
    IER |= M_INT3;
    IER |= M_INT4;
    IER |= M_INT6;
    IER |= M_INT13;

    // PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // enable PIEIER TINT0

    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;

    PieCtrlRegs.PIEIER6.bit.INTx1=1;     // Enable PIE Group 6, INT 1
    PieCtrlRegs.PIEIER6.bit.INTx2=1;     // Enable PIE Group 6, INT 2

    EINT;    // Enable Global Interrupts
    ERTM;   // Enable Global realtime interrupt DBGM

    for(;;)
    {
      MainLoop();
      DELAY_US(2000);
    }
}



__interrupt void epwm1_isr(void)
{
    EPwm1Regs.ETCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}



__interrupt void ecap1_isr(void)
{
    ECap1IntCount++;

    temp[0] = ECap1Regs.CAP2-ECap1Regs.CAP1; // Calculate 1st period
    temp[1] = ECap1Regs.CAP3-ECap1Regs.CAP2; // Calculate 2nd period
    temp[2] = ECap1Regs.CAP4-ECap1Regs.CAP3; // Calculate 3rd period

    Uint32 sum = 0;
    unsigned char count  = 0;
    unsigned char i = 0;
    for(i=0;i<3;i++)
    {
    //  (60000, 190000)
        if(temp[i] > 60000 && temp[i] < 190000)
        {
            sum += temp[i];
            count ++;
        }
    }

    if(count >= 1)
    {
        peroid = sum / count;
        current_rpm =  24064171 / peroid ;
    }

    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    ECap1Regs.ECCTL2.bit.REARM = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}



// xint4_isr - increse
__interrupt void xint4_isr(void)
{
    xint3_count++;
    DELAY_US(20000);
    if(GpioDataRegs.GPBDAT.bit.GPIO43 == 0)
    {
        desired_rpm += 40;
        if(desired_rpm >= 360){
            desired_rpm = 360;
        }
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

//
// xint5_isr - decrese duty cycle
//
__interrupt void xint5_isr(void)
{
    xint4_count++;
    DELAY_US(20000);
    if(GpioDataRegs.GPBDAT.bit.GPIO44 == 0)
    {
       desired_rpm -= 40;
       if(desired_rpm <= 160){
           desired_rpm = 160;
       }
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}


// xint3_isr - start
__interrupt void xint3_isr(void)
{
    xint5_count ++;
    DELAY_US(20000);
    if(GpioDataRegs.GPBDAT.bit.GPIO42 == 0){
        MotorForward();
        strcpy(direction, "F");
        strcpy(status, "start");
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// xint7_stop
__interrupt void xint7_isr(void)
{
    xint6_count ++;
    DELAY_US(20000);
    if(GpioDataRegs.GPBDAT.bit.GPIO46 == 0){
        MotorStop();
        desired_rpm = 160;
        ResetPid();
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
// forward/ backward
__interrupt void xint6_isr(void)
{
    DELAY_US(200000);
    if(GpioDataRegs.GPBDAT.bit.GPIO45 == 0){
        // current forward
        if(IsMotorForward())
        {
            xint7_count++;
            // set backward
            MotorReverse();
            strcpy(direction, "R");
        }
        // current backward
        else if(IsMotorReverse())
        {
            // set forward
            MotorForward();
            strcpy(direction, "F");
        }
        else{;}
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// 100ms
__interrupt void cpu_timer0_isr(void)
{
    PidControl(desired_rpm, current_rpm);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


// display and interrupt management
void MainLoop(void)
{
    // motor running
    if(IsMotorRunning())
    {
        DisableMotorStart();
        EnablePwmChange();
        OledRefreshValue();
    }

    if(!IsMotorRunning())
    {
        EnableMotorStart();
        DisablePwmChange();

        current_rpm = 0;
        strcpy(status, "stop");
        strcpy(direction, "none");

        // clear current speed
        OledClearToRowEnd(0, 8);
        // clear desired speed
        OledClearToRowEnd(1, 8);
        // clear motor status
        OledClearToRowEnd(2, 8);
        OledPutWord(2, 8, status);
        // clear direction
        OledClearToRowEnd(3, 8);
        OledPutWord(3, 8, direction);
    }
}

void OledCreateMenu(void)
{
    OledPutWord(0, 0, "cur_rpm:");
    OledPutWord(1, 0, "set_rpm:");
    OledPutWord(2, 0, "status:");
    OledPutWord(3, 0, "dir:");
}
void PidControl(Uint32 desired_rpm, Uint32 current_rpm)
{
    error = desired_rpm - current_rpm;
    duk = KP * error + KI * error1 + KD * error2;
    error2 = error1; error1 = error;
    UpdateCompare(&epwm1_info, duk);
}


void ResetCompare(EPWM_INFO *epwm_info)
{
    epwm_info->EPwmRegHandle->CMPA.half.CMPA =  EPWM1_MIN_CMPA;
    epwm_info->EPwmRegHandle->CMPB = EPWM1_MIN_CMPB;
}


void OledRefreshValue(void)
{
    if(duk > -5 && duk < 5)
    {
        strcpy(status, "steady");
    }

    else if(duk > 5)
    {
        strcpy(status, "increase");
    }

    else if(duk < -5)
    {
        strcpy(status, "decrease");
    }


    // clear current speed
    OledClearToRowEnd(0, 8);
    OledPutNumber(0, 8, current_rpm);

    // clear desired speed
    OledClearToRowEnd(1, 8);
    OledPutNumber(1, 8, desired_rpm);
    // clear motor status
    OledClearToRowEnd(2, 8);
    OledPutWord(2, 8, status);

    // clear direction
    OledClearToRowEnd(3, 8);
    OledPutWord(3, 8, direction);
}

void InitPid(void)
{
    error = 0;
    error1 = 0;
    error2 = 0;
    duk = 0;
}

void ResetPid(void)
{
    InitPid();
}

void UpdateCompare(EPWM_INFO *epwm_info, int change)
{
    epwm_info->EPwmRegHandle->CMPA.half.CMPA += change;
    epwm_info->EPwmRegHandle->CMPB += change;
    if(epwm_info->EPwmRegHandle->CMPA.half.CMPA >
           epwm_info->EPwmMaxCMPA)
    {
        epwm_info->EPwmRegHandle->CMPA.half.CMPA = EPWM1_MAX_CMPA ;
    }
    if(epwm_info->EPwmRegHandle->CMPA.half.CMPA <
               epwm_info->EPwmMinCMPA)
    {
        epwm_info->EPwmRegHandle->CMPA.half.CMPA = EPWM1_MIN_CMPA;
    }

    if(epwm_info->EPwmRegHandle->CMPB > epwm_info->EPwmMaxCMPB)
    {
        epwm_info->EPwmRegHandle->CMPB = EPWM1_MAX_CMPB;
    }


    if(epwm_info->EPwmRegHandle->CMPB < epwm_info->EPwmMinCMPB)
    {
        epwm_info->EPwmRegHandle->CMPB = EPWM1_MIN_CMPB;
    }

    cmpa = epwm_info->EPwmRegHandle->CMPA.half.CMPA;
    cmpb = epwm_info->EPwmRegHandle->CMPB;


}
void InitEPwm1Example()
{
    //
    // Setup TBCLK
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;

    //
    // Setup shadow register load on ZERO
    //
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Set Compare values
    //
    EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;    // Set compare A value
    EPwm1Regs.CMPB = EPWM1_MIN_CMPB;              // Set Compare B value

    //
    // Set actions
    //
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;      // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;      // Set PWM1B on Zero
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;    // Clear PWM1B on event B, up count

    //
    // Interrupt where we will change the Compare Values
    //
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

    //
    // Information this example uses to keep track of the direction the
    // CMPA/CMPB values are moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //

    //
    // Start by increasing CMPA & CMPB
    //
    epwm1_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
    epwm1_info.EPwm_CMPB_Direction = EPWM_CMP_UP;

    epwm1_info.EPwmTimerIntCount = 0;      // Zero the interrupt counter
    epwm1_info.EPwmRegHandle = &EPwm1Regs; //Set the pointer to the ePWM module
    epwm1_info.EPwmMaxCMPA = EPWM1_MAX_CMPA;  // Setup min/max CMPA/CMPB values
    epwm1_info.EPwmMinCMPA = EPWM1_MIN_CMPA;
    epwm1_info.EPwmMaxCMPB = EPWM1_MAX_CMPB;
    epwm1_info.EPwmMinCMPB = EPWM1_MIN_CMPB;
}

