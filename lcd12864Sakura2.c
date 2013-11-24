#include "lcd12864.h"
#include "comm.h"
//#include "serial.h"
#include "keyBoard.h"
#include "sakura2.h"
#include <intrins.h>
#define LCD12864_DELAYTIME 20
#define KEY_ENTER 	KEY_1
#define KEY_CHANGE	KEY_2
#define KEY_CANCEL	KEY_3
sbit key1 = P3^4;
sbit key2 = P3^5;

void Lcd12864_WriteCommand(uchar com){
	Lcd_RS=0;
	P0=com;
	Lcd_EN=1;
	delayus(LCD12864_DELAYTIME);
	Lcd_EN=0;
	delayus(LCD12864_DELAYTIME);
}
void Lcd12864_WriteData(uchar dat){
	Lcd_RS=1;
	P0=dat;
	Lcd_EN=1;
	delayus(LCD12864_DELAYTIME);
	Lcd_EN=0;
	delayus(LCD12864_DELAYTIME);
}
void Lcd12864_WriteData2(uint dat){
	Lcd_RS=1;
	P0=dat>>8;
	Lcd_EN=1;
	delayus(LCD12864_DELAYTIME);
	Lcd_EN=0;
	P0=dat;
	delayus(LCD12864_DELAYTIME);
	Lcd_EN=1;
	delayus(LCD12864_DELAYTIME);
	Lcd_EN=0;
	delayus(LCD12864_DELAYTIME);
}
uchar Lcd12864_ReadData(){
	uchar dat;
	Lcd_EN=0;
	Lcd_RS=1;
	Lcd_RW=1; 
	P0=0xff;
	//改变到输入时必须先置高电平，不然呵呵了。
	Lcd_EN=1;	
	delayus(LCD12864_DELAYTIME);
	dat=P0;
	Lcd_EN=0;
	Lcd_RW=0; 
	return dat;
}
void Lcd_Init(){
	Lcd_EN=0;
	Lcd_RW=0;
	Lcd12864_WriteCommand(0x38);
	Lcd12864_WriteCommand(0x0c);
	Lcd12864_WriteCommand(0x06);
	Lcd12864_WriteCommand(0x01);
	delay(20);
}
void Lcd_Clear(){
	Lcd12864_WriteCommand(0x01);
	delay(20);
}
void Lcd_Locate(uchar x,uchar y){
	uchar temp_data;
	Lcd12864_WriteCommand(0x80+0x08*(y/2)+0x10*(y%2)+x/2);
	if(x%2!=0){
		//Lcd12864_ReadData();
		//Lcd12864_ReadData();
		temp_data=Lcd12864_ReadData();	
		Lcd12864_WriteCommand(0x80+0x08*(y/2)+0x10*(y%2)+x/2);
		Lcd12864_WriteData(temp_data);
		//Lcd12864_WriteCommand(0x80+0x10*y+x/2);
		//Lcd12864_WriteData(temp_data);
				
	}
	
}
void Lcd_Input_str(uchar *str){
	while(*str!='\0'){
		Lcd12864_WriteData(*str++);
	}
	*str=0;
}
void Lcd_Display_str(uchar x,uchar y,uchar *str){
	Lcd_Locate(x,y);
	while(*str!='\0'){
		Lcd12864_WriteData(*str++);
	}
	*str=0;
}
void Lcd_Input_long(long num) 	//发送整数
{
	
 	ulong temp;
	uint i=0,j; 
 	if(num<0){Lcd12864_WriteData('-');num=-num;} //处理成正数
	temp=num;
 	do{
 		temp=temp/10;
		i++;
 	}while(temp>0);
 	while(i>0){
 		j=i;
		temp=1;
		while(j>1){j--;temp*=10;}
		Lcd12864_WriteData(num/temp+'0');
		num-=(num/temp)*temp;
 		i--;
 	}
}
void Lcd_Input_float(float num,uchar precise)  	
//发送浮点数，precise标识精度
{
	long temp;
 	if(num<0){Lcd12864_WriteData('-');num=-num;} //处理成正数	
	if(num>=1){
		temp=(long)num;
		Lcd_Input_long(temp);
		num=num-temp;
		precise--;
	}else{
		Lcd12864_WriteData('0');
	}
	Lcd12864_WriteData('.');
	num=num*10;
	while(precise>0){
		precise--;
		temp=(int)num;
		Lcd12864_WriteData(temp+'0');
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
#define Menu_Item 8
uchar Lcd_Menu(uchar item_number,uchar items[10][10]){
	uchar item_select=0;
	uchar i=0;
	uchar x,y;
	bit reDraw=1;
	Lcd_Clear();
	KEY_Last=KEY_ENTER;
//	send_str(items[0]);
	do{
		if(KEY_Last==KEY_ENTER){
			item_select++;
			if(item_select>item_number)item_select=1;
			if(item_select%Menu_Item==1)reDraw=1;
		}
		else{item_select--;
			if(item_select<1){item_select=item_number;reDraw=1;}
			if(item_select%Menu_Item==0)reDraw=1;
		}
		
		for(i=((item_select-1)/Menu_Item)*Menu_Item;i<((item_select+(Menu_Item-1))/Menu_Item)*Menu_Item;i++){
		   x=(i%Menu_Item)%2*8;
		   y=(i%Menu_Item)/2;
		   if(reDraw){
				Lcd_Display_str(x+1,y,"      ");
				if(i<item_number){
					Lcd_Display_str(x+1,y,items[i]);
				}
			}
			if(i+1==item_select){
				Lcd_Display_str(x,y,">");
				Lcd_Display_str(x+7,y,"<");
			}else{
				Lcd_Display_str(x,y," ");
				Lcd_Display_str(x+7,y," ");
			}	
		}
		reDraw=0;
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
	for(idx=0;str[idx]!=0;idx++){
		Lcd_Locate(idx*2,1);		
		Lcd12864_WriteData(str[idx]);
	}			
	idx=0;
	Lcd12864_WriteCommand(0x0f);
	//开启光标闪烁
	while(str[idx]!=0){
		Lcd_Locate(idx*2,1);
		while(waitForKey(KEY_CANCEL | KEY_CHANGE | KEY_ENTER,0)==KEY_CHANGE){
			str[idx]=str[idx]+1;
			if(str[idx]>upperChar)str[idx]=lowerChar;	
//			send_str(str);
			Lcd12864_WriteData(str[idx]);
			Lcd_Locate(idx*2,1);
		}
		if(KEY_Last==KEY_CANCEL && idx>0)idx--;
		if(KEY_Last==KEY_ENTER )idx++;
	}
	
	Lcd12864_WriteCommand(0x0c);
	//隐藏光标
	Lcd_Clear();
	delay(50);	
}	
void Lcd_DrawMode(){
	Lcd_Clear();
	Lcd12864_WriteCommand(0x34);//开扩充指令
	Lcd12864_WriteCommand(0x36);//开绘图指令

}
void Lcd_DrawClear(){
	uchar mx,my,t;
	uchar * Pdat;
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"              ");
							
	for(t=0;t<53;t++){
		if(t==1){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"隐藏着黑暗力量的");			
		}
		if(t==8){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"    钥匙啊！    ");
		}
		if(t==13){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"              ");
		}
		if(t==15){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"在我面前显示出  ");
		}
		if(t==22){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"你真实的力量吧  ");
		}
		if(t==29){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"    根据契约    ");
		}
		if(t==35){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"  小樱命令你！  ");
		}

		if(t==45){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"              ");
		}
		if(t==47){
				Lcd12864_WriteCommand(0x32);//换到基本指令
				Lcd_Display_str(0,3,"   Release!!  ");
		}
		Lcd12864_WriteCommand(0x36);//开绘图指令
	//	Lcd12864_WriteCommand(0x34);//关绘图指令
		//delayus(10);
		//delayus(300);
		mx=0;
		my=0;
		while(my<32){

			Lcd_RS=0;
			P0=0x80+my;
			delayus(10);
			Lcd_EN=1;
			delayus(10);
			Lcd_EN=0;

			P0=0x80+mx;
			delayus(10);
			Lcd_EN=1;
			delayus(10);
			Lcd_EN=0;

			Lcd_RS=1;
			Pdat=picture[t][my];
			delayus(10);
			while(mx<16){
					if(mx==8){Pdat=picture[t][my+32];}				
					P0=*(Pdat++);
					delayus(2);
					Lcd_EN=1;
					delayus(1);
					_nop_();
					_nop_();
					_nop_();
					_nop_();
					Lcd_EN=0;
					P0=*(Pdat++);
					delayus(2);					
					Lcd_EN=1;
					delayus(1);
					_nop_();
					_nop_();
					_nop_();
					_nop_();
					Lcd_EN=0;
					mx++;
			}
			mx=0;
			my++;
		}
		delay(85);
	}
	delay(1000);
	Lcd12864_WriteCommand(0x32);//换到基本指令
	Lcd12864_WriteCommand(0x30);//关画面
}
void Lcd_WritePoint(uchar x,uchar y,bit dat){
	uchar mx,my,dx;
	Lcd12864_WriteCommand(0x34);//关绘图指令
	mx=x/16+y/32*8;
	my=y%32;
	dx=x%16;
	Lcd12864_WriteCommand(0x80+my);
	Lcd12864_WriteCommand(0x80+mx);
	Lcd12864_WriteData(0xf0);
	Lcd12864_WriteData(0xf0);
	Lcd12864_WriteCommand(0x36);
	delay(10);
		
}