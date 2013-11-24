#include "lcd1602.h"
#include "comm.h"
#include "serial.h"
#include "keyBoard.h"
#define LCD1602_DELAYTIME 10
#define KEY_ENTER 	KEY_1
#define KEY_CHANGE	KEY_2
#define KEY_CANCEL	KEY_3
sbit key1 = P3^4;
sbit key2 = P3^5;
void Lcd1602_WriteCommand(uchar com){
	Lcd_RS=0;
	P0=com;
	delayus(LCD1602_DELAYTIME);
	Lcd_EN=1;
	delayus(LCD1602_DELAYTIME);
	Lcd_EN=0;
}

void Lcd1602_WriteData(uchar dat){
	Lcd_RS=1;
	P0=dat;
	delayus(LCD1602_DELAYTIME);
	Lcd_EN=1;
	delayus(LCD1602_DELAYTIME);
	Lcd_EN=0;
}
uchar Lcd1602_ReadData(){
	uchar dat;
	Lcd_EN=0;
	Lcd_RS=1;
	Lcd_RW=1; 
	P0=0xff;
	//改变到输入时必须先置高电平，不然呵呵了。
	Lcd_EN=1;	
	delayus(LCD1602_DELAYTIME);
	dat=P0;
	Lcd_EN=0;
	return dat;
}
void Lcd_Init(){
	Lcd_EN=0;
	Lcd_RW=0;
	Lcd1602_WriteCommand(0x38);
	Lcd1602_WriteCommand(0x0c);
	Lcd1602_WriteCommand(0x06);
	Lcd1602_WriteCommand(0x01);
}
void Lcd_Clear(){
	Lcd1602_WriteCommand(0x01);
	delay(10);
}
void Lcd_Locate(uchar x,uchar y){
	Lcd1602_WriteCommand(0x80+0x40*y+x);
}
void Lcd_Input_str(uchar *str){
	while(*str!='\0'){
		Lcd1602_WriteData(*str++);
	}
	*str=0;
}
void Lcd_Display_str(uchar x,uchar y,uchar *str){
	Lcd_Locate(x,y);
	while(*str!='\0'){
		Lcd1602_WriteData(*str++);
	}
	*str=0;
}
void Lcd_Input_long(long num) 	//发送整数
{
	
 	ulong temp;
	uint i=0,j; 
 	if(num<0){Lcd1602_WriteData('-');num=-num;} //处理成正数
	temp=num;
 	do{
 		temp=temp/10;
		i++;
 	}while(temp>0);
 	while(i>0){
 		j=i;
		temp=1;
		while(j>1){j--;temp*=10;}
		Lcd1602_WriteData(num/temp+'0');
		num-=(num/temp)*temp;
 		i--;
 	}
}
void Lcd_Input_float(float num,uchar precise)  	
//发送浮点数，precise标识精度
{
	long temp;
 	if(num<0){Lcd1602_WriteData('-');num=-num;} //处理成正数	
	if(num>=1){
		temp=(long)num;
		Lcd_Input_long(temp);
		num=num-temp;
		precise--;
	}else{
		Lcd1602_WriteData('0');
	}
	Lcd1602_WriteData('.');
	num=num*10;
	while(precise>0){
		precise--;
		temp=(int)num;
		Lcd1602_WriteData(temp+'0');
		num=(num-temp)*10;	
	}
}
void Lcd_MessageBox(uchar * text,uchar * text2){
	Lcd_Clear();
	Lcd_Display_str(0,0,text);
	Lcd_Display_str(0,1,text2);
	waitForKey(KEY_ENTER,0);
	Lcd_Clear();
	delay(50);	
}
bit Lcd_Dialog(uchar * text,uchar * button1_text,uchar * button2_text){
	bit Answer=0;
	Lcd_Clear();
	Lcd_Display_str(0,0,"                ");
	Lcd_Display_str(0,0,text);
	Lcd_Display_str(0,1,"                ");	
	Lcd_Display_str(1,1,button1_text);
	Lcd_Display_str(9,1,button2_text);
	Lcd_Display_str(0,1,">");
	Lcd_Display_str(7,1,"<");
	Lcd_Display_str(8,1," ");
	Lcd_Display_str(15,1," ");	
	while(waitForKey(KEY_CHANGE | KEY_ENTER,0)!=KEY_ENTER){
	//不是Key_enter肯定就是keyL或R
		Answer=~Answer;
		if(Answer==0){
			Lcd_Display_str(0,1,">");
			Lcd_Display_str(7,1,"<");
			Lcd_Display_str(8,1," ");
			Lcd_Display_str(15,1," ");
		}else{
			Lcd_Display_str(0,1," ");
			Lcd_Display_str(7,1," ");
			Lcd_Display_str(8,1,">");
			Lcd_Display_str(15,1,"<");			
		}	
	}	
	Lcd_Clear();
	delay(50);
	return(Answer);
	
}
uchar Lcd_Menu(uchar item_number,uchar items[10][10]){
	uchar item_select=0;
	uchar i=0;
	uchar x,y;
	Lcd_Clear();
	KEY_Last=KEY_ENTER;
	send_str(items[0]);
	do{
		if(KEY_Last==KEY_ENTER){
			item_select++;
			if(item_select>item_number)item_select=1;
		}
		else{item_select--;
			if(item_select<1)item_select=item_number;
		}
		
		for(i=((item_select-1)/4)*4;i<((item_select+3)/4)*4;i++){
		   x=(i%4)%2*8;
		   y=(i%4)/2;
			_send_value(i);
			Lcd_Display_str(x+1,y,"      ");
			if(i<item_number){
				Lcd_Display_str(x+1,y,items[i]);
			}
			if(i+1==item_select){
				Lcd_Display_str(x,y,">");
				Lcd_Display_str(x+7,y,"<");
			}else{
				Lcd_Display_str(x,y," ");
				Lcd_Display_str(x+7,y," ");
			}	
		}
	}while(waitForKey(KEY_CANCEL | KEY_CHANGE | KEY_ENTER,0)!=KEY_CHANGE);
	Lcd_Clear();
	delay(50);
	return 0;
}

void Lcd_changeString(uchar * text,uchar * str,uchar lowerChar,uchar upperChar){
//用lowerChar至UpperChar之间的某个数调整一个字串。
	uchar idx=0;
	Lcd_Clear();
	Lcd_Display_str(0,0,text);
	Lcd_Display_str(0,1,str);			
	Lcd1602_WriteCommand(0x0f);
	//开启光标闪烁
	while(str[idx]!=0){
		Lcd_Locate(idx,1);
		while(waitForKey(KEY_CANCEL | KEY_CHANGE | KEY_ENTER,0)==KEY_CHANGE){
			str[idx]=str[idx]+1;
			if(str[idx]>upperChar)str[idx]=lowerChar;	
			send_str(str);
			Lcd1602_WriteData(str[idx]);
			Lcd_Locate(idx,1);
		}
		if(KEY_Last==KEY_CANCEL && idx>0)idx--;
		if(KEY_Last==KEY_ENTER )idx++;
	}
	
	Lcd1602_WriteCommand(0x0c);
	//隐藏光标
	Lcd_Clear();
	delay(50);	
}	
