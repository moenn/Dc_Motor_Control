/*
 * my_oled.c
 *
 *  Created on: May 7, 2019
 */
#include "DSP28x_Project.h"
#include "my_character.h"
#include <stdlib.h>

#ifndef MY_OLED_H_
#define MY_OLED_H_

#define SCL GPIO59
#define SDA GPIO60
#define RST GPIO61
#define DC GPIO62
#define CS GPIO63

void SpiReadWrite(unsigned char data);
void OledSendData(unsigned char data);
void OledSendCommand(unsigned char command);

void OledSetPosition(unsigned char row, unsigned char column);
// 重置 OLED
void OledReset(void)
{
  GpioDataRegs.GPBCLEAR.bit.RST = 1;
  DELAY_US(50000); // 50ms
  GpioDataRegs.GPBSET.bit.RST = 1;
}

// 点亮整个屏幕
void OledFull(void)
{
    unsigned char row=0;
    unsigned char i=0;

    for (row=0;row<8;row++)
    {
        OledSetPosition(0, row);
        for (i=0;i<128;i++)
        {
            OledSendData(0xFF);
        }
    }
}
// 熄灭整个屏幕
void OledClear(void)
{
  unsigned char row=0;
  unsigned char i=0;

  for (row=0; row<8; row++)
  {
    OledSetPosition(row, 0);
    for (i=0;i<128;i++)
    {
      OledSendData(0x00);
    }
  }
}

// OLED 测试函数
void OledTest(void)
{
    unsigned char row=0;
    unsigned char i;

    for(row=0;row<8;row++)
    {
        OledSetPosition(row, 0);
        for(i=0;i<128;i++){
            OledSendData(0xff);
            DELAY_US(10000);
        }
    }

    for(row=0;row<8;row++)
    {
        OledSetPosition(row, 0);
        for(i=0;i<128;i++){
            OledSendData(0x00);
            DELAY_US(10000);
        }

    }
}

// 初始化与 OLED 相连的 GPIO 引脚
void InitOledGpio()
{
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.SCL = 0;
    GpioCtrlRegs.GPBMUX2.bit.SCL = 0; // gpio
    GpioDataRegs.GPBCLEAR.bit.SCL = 1; // clear
    GpioCtrlRegs.GPBDIR.bit.SCL = 1; // output

    GpioCtrlRegs.GPBPUD.bit.SDA = 0;
    GpioCtrlRegs.GPBMUX2.bit.SDA = 0; // gpio
    GpioDataRegs.GPBCLEAR.bit.SDA = 1; // clear
    GpioCtrlRegs.GPBDIR.bit.SDA = 1; // output

    GpioCtrlRegs.GPBPUD.bit.RST = 0;
    GpioCtrlRegs.GPBMUX2.bit.RST = 0; // gpio
    GpioDataRegs.GPBCLEAR.bit.RST = 1; // clear
    GpioCtrlRegs.GPBDIR.bit.RST = 1; // output

    GpioCtrlRegs.GPBPUD.bit.DC = 0;
    GpioCtrlRegs.GPBMUX2.bit.DC = 0; // gpio
    GpioDataRegs.GPBCLEAR.bit.DC = 1; // clear
    GpioCtrlRegs.GPBDIR.bit.DC = 1; // output

    GpioCtrlRegs.GPBPUD.bit.CS = 0;
    GpioCtrlRegs.GPBMUX2.bit.CS = 0; // gpio
    GpioDataRegs.GPBCLEAR.bit.CS = 1; // clear
    GpioCtrlRegs.GPBDIR.bit.CS = 1; // output
    EDIS;

}

// 初始化 OLED
void InitOled()
{

    GpioDataRegs.GPBSET.bit.CS = 1;
    GpioDataRegs.GPBSET.bit.SCL = 1;
    OledReset();

    OledSendCommand(0xAE); 
    OledSendCommand(0x00); // set low column address
    OledSendCommand(0x10); // set high column address
    OledSendCommand(0x40); // set display start line
    // OledSendCommand(0x81); 
    // oled_send_data(0xFF); 
    OledSendCommand(0xA1); // 
    OledSendCommand(0xC8); //
    OledSendCommand(0xA6); // 
    OledSendCommand(0xA8); // 
    OledSendCommand(0x3F);
    OledSendCommand(0xD3); // 
    OledSendCommand(0x00);
    OledSendCommand(0xD5); // 
    OledSendCommand(0x80);
    OledSendCommand(0xD9); // 
    OledSendCommand(0xF1); // 
    OledSendCommand(0xDA); // 
    OledSendCommand(0x12); // 
    OledSendCommand(0xDB); //
    OledSendCommand(0x00); // 

    OledSendCommand(0x20); // 
    OledSendCommand(0x02); //

    OledSendCommand(0x8d); // 
    OledSendCommand(0x14); // 
    OledSendCommand(0xA4); // 
    OledSendCommand(0xA6); //
    OledSendCommand(0xAF); //
}


void OledSetPosition(unsigned char row, unsigned char column)
{
    // 设置起始行
    OledSendCommand(0xb0 + row);
    // 设置起始列
    OledSendCommand((column & 0x0f) | 0x00);
    OledSendCommand(((column & 0xf0) >> 4) | 0x10);
}


void OledSetBlock(unsigned char row, unsigned char column)
{
    OledSetPosition(row*2, column*8);
}

// 在指定位置输出字符，如 'a', '0', '!'
void OledPutChar(unsigned char row, unsigned char column, char character)
{
    unsigned char i;

    OledSetPosition(row*2, column*8);
    for(i=0;i<16;i++)
    {
        OledSendData(CHARACTERS_8X16[character - 32][i]);
        if(i == 7){
            OledSetPosition(row*2+1, column*8);
        }
    }
}


// 在指定位置输出字符串，如 "hello".
// 当前行空间不足时会延伸输出到下一行
void OledPutWord(unsigned char row, unsigned char column, char* msg)
{
    unsigned char i;
    for(i=0; msg[i]!='\0'; i++)
    {
        OledPutChar(row, column,msg[i]);
        column++;
        if(column == 16){
            column = 0;
            row++;
            if(row == 4){
                row = 0;
            }
        }
    }
}

// 在指定位置输出数字,如 233, 2000
// 行为与 OledPutWord() 相同
void OledPutNumber(unsigned char row , unsigned char column, long int number)
{
    char msg[16] = {};
    ltoa(number, msg);
    OledPutWord(row, column, msg);
}

// 清除指定区块
void OledClearBlock(unsigned char row, unsigned char column)
{
    unsigned char i;
    OledSetBlock(row, column);
    for(i=0;i<8;i++)
    {
        OledSendData(0x00);
    }
}


//清除指定位置的字符
void OledClearChar(unsigned char row, unsigned char column)
{
    unsigned char i;
    OledSetPosition(row*2, column*8);
    for(i=0;i<16;i++)
    {
        OledSendData(0x00);
        if(i == 7){
            OledSetPosition(row*2+1, column*8);
        }
    }
}

// 清除指定行的 [column_start, column_end) 区域
void OledClearRowPartial(unsigned char row, unsigned char column_start, unsigned char column_end)
{
    unsigned char i;
    OledSetPosition(row*2, column_start*8);
    for(i=column_start*8;i<column_end*8;i++)
    {
        OledSendData(0x00);
    }
    OledSetPosition(row*2+1, column_start*8);
    for(i=column_start*8;i<column_end*8;i++)
    {
        OledSendData(0x00);
    }
}


// 清除指定行的 column_start 至行尾
void OledClearToRowEnd(unsigned char row, unsigned char column_start)
{
    OledClearRowPartial(row, column_start, 16);
}
// 清除指定行的行首至 column_end 
void OledClearToRowStart(unsigned char row, unsigned char column_end)
{
    OledClearRowPartial(row, 0, column_end+1);
}




// DSP 与 OLED 之间的串行通信函数
void SpiReadWrite(unsigned char data)
{
  unsigned char i;
  for(i=0; i<8; i++)
  {
    //将数据移位后的最高位赋给 SDA
    if((data << i) & 0x80)
    {
      GpioDataRegs.GPBSET.bit.SDA = 1;
    }else{
      GpioDataRegs.GPBCLEAR.bit.SDA = 1;
    }

    // SDA 中的数据将在 SCL 的上升沿时传送出去
    GpioDataRegs.GPBCLEAR.bit.SCL = 1;
    DELAY_US(5);
    GpioDataRegs.GPBSET.bit.SCL = 1;
  }
}

// 向 OLED 发送数据
void OledSendData(unsigned char data)
{
  GpioDataRegs.GPBCLEAR.bit.CS = 1;

  GpioDataRegs.GPBSET.bit.DC = 1;
  SpiReadWrite(data);


  GpioDataRegs.GPBSET.bit.CS = 1;
}

// 向 OLED 发送命令
void OledSendCommand(unsigned char command)
{
    GpioDataRegs.GPBCLEAR.bit.CS = 1;

    GpioDataRegs.GPBCLEAR.bit.DC = 1;
    SpiReadWrite(command);

    GpioDataRegs.GPBSET.bit.CS = 1;
}
#endif
