#ifndef _LCD1602_H
#define _LCD1602_H
#include "comm.h"
#define uchar unsigned char
#define uint  unsigned int 
sbit Lcd_EN=P2^6;
sbit Lcd_RS=P2^4;
sbit Lcd_RW=P2^5;
extern void  Lcd1602_WriteCommand(uchar com);
extern void  Lcd1602_WriteData(uchar dat);
extern uchar Lcd1602_ReadData();
extern void  Lcd_Init();
extern void  Lcd_Clear();
extern void Lcd_Locate(uchar x,uchar y);
extern void  Lcd_Input_str(uchar *str);
extern void  Lcd_Display_str(uchar x,uchar y,uchar *str);
extern void Lcd_Input_long(long num);
extern void Lcd_Input_float(float num,uchar precise);
extern bit Lcd_Dialog(uchar * text,uchar * button1_text,uchar * button2_text);
extern void Lcd_changeString(uchar * text,uchar * str,uchar lowerChar,uchar upperChar);
extern void Lcd_MessageBox(uchar * text,uchar * text2);
uchar Lcd_Menu(uchar item_number,uchar items[10][10]);
#endif