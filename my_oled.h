/*
 * my_oled.h
 *
 *  Created on: May 7, 2019

 */
#include "DSP28x_Project.h"

#ifndef MY_OLED_H_
#define MY_OLED_H_



extern void OledReset(void);
extern void OledFull(void);
extern void OledClear(void);
extern void OledTest(void);

extern void InitOledGpio();
extern void InitOled();

extern void OledSetPosition(unsigned char row, unsigned char column);
extern void OledSetBlock(unsigned char row, unsigned char column);

extern void OledPutChar(unsigned char row, unsigned char column, char character);
extern void OledPutWord(unsigned char row, unsigned char column, char* msg);
extern void OledPutNumber(unsigned char row , unsigned char column, long int number);

extern void OledClearBlock(unsigned char row, unsigned char column);
extern void OledClearChar(unsigned char row, unsigned char column);
extern void OledClearRowPartial(unsigned char row, unsigned char column_start, unsigned char column_end);
extern void OledClearToRowEnd(unsigned char row, unsigned char column_start);
extern void OledClearToRowStart(unsigned char row, unsigned char column_end);

extern void SpiReadWrite(unsigned char data);
extern void OledSendData(unsigned char data);
extern void OledSendCommand(unsigned char command);

#endif /* MY_OLED_H_ */
