#include "comm.h"
#include"lcd12864.h"
#include "SD.h"
#include "serial.h"

//�����෽�Ľ��������˶�����������
//��ͼƬ��128*64 �ڰ�bmp��ʽ˳��洢��һ���ļ����ڣ�����SD����Ŀ¼�¡�
//ʹ��1.hex��ʼ��sd����
//Ȼ��ʹ�ñ�����
//Ұ����Ļ������
#include "keyBoard.h"
//#include "ClowCards.h"
//ע����ʹ�ò˵�ʱһ��Ҫ����ά����ĳ�����롣
//����Ǻ�
uchar xdata SDPic[1024];
//uchar xdata SDData[512];
//uchar xdata SDData2[512];

uchar xdata name[5]="Hehe";
uchar xdata age[5]="100";
uchar xdata clow_card=5;

void ReadBMP(ulong SD_Sector){
	uint x=0;
	uint y=0;

	//SD_ReadBlock(SD_Sector,SDPic,0x003e,0x01c2);
	if(Lcd_SubLineEn){
		for(x=0;x<256;x++){
			SDPic[x]=0;
		}
	//	SD_ReadBlock(SD_Sector,SDPic,0x003e,0x01c2);
		SD_ReadBlock(SD_Sector,SDPic+0x0100,0x013e,0x00c2);
	}else{
		SD_ReadBlock(SD_Sector,SDPic,0x003e,0x01c2);
	}	
/*	
	//��һ����
	for(i=0x0000;i<0x0200-0x003e;i++){
		//_send_value(i);	
		
		x=(i)%16;
		y=(i)/16;
		SDPic[y][x]=~SDData[i+0x003e];
		send_uint(SDPic[y][x]);
		send_tab();send_uint(i+0x003e);send_tab();
		_send_value((SDData[i+0x003e]));
		
	}
*/
	SD_Sector+=1;
	SD_ReadBlock(SD_Sector,SDPic+0x01c2,0x0000,0x0200);
/*	
	//�ڶ�����
	for(i=0x0200-0x003e;i<0x0400-0x003e;i++){	
		//_send_value(i);
		x=(i)%16;
		y=(i)/16;
		SDPic[y][x]=~SDData[i-(0x0200-0x003e)];
		send_uint(SDPic[y][x]);
		send_tab();send_uint(i-(0x0200-0x003e));send_tab();
		_send_value((SDData[i-(0x0200-0x003e)]));
		
	}
	*/
	SD_Sector+=1;

	SD_ReadBlock(SD_Sector,SDPic+0x03c2,0x0000,0x003e);
	//��������
/*
	for(i=0x0400-0x003e;i<0x0400;i++){	
		//_send_value(i);
		x=(i)%16;
		y=(i)/16;
		SDPic[y][x]=~SDData[i-(0x0400-0x003e)];
		send_uint(SDPic[y][x]);
		send_tab();send_uint(i-(0x0400-0x003e));send_tab();
		_send_value((SDData[i-(0x0400-0x003e)]));
		
	}
*/
}
void main(){
	uint i;
	uchar *p1;
	ulong SD_Sector;
	Lcd_Init();

	delay(100);	
	Serial_Init();
//	_send_value(clow_cards[0]);
	//	send_uint(0);
	p1="Hehe Designed";
	Lcd_MessageBox("A:�ٱ�Сӣ","B:�����ľ���");
	if(!Lcd_Dialog("ѡ�񶯻�Ƭ��","  A  ","  B  ")){
		SD_Sector=336320;
	}else{
		SD_Sector=576;
	}
	i=SD_Init();
	_send_value(i);
	Lcd_Display_str(0,0,p1);
	delay(1000);
	Lcd_DrawMode();
	for(i=0;i<6000;i++){
		ReadBMP(SD_Sector);
		Lcd_DrawPicture(SDPic,i);
		SD_Sector+=64;
		//delay(50);
	}	
	delay(10);
//	Lcd_Menu(55,clow_cards);
	//Lcd_Locate(0,1);
	Lcd_changeString("What'sYourName?",name,'A','z');
	Lcd_MessageBox(name,"Oh!A good name!");
	if(Lcd_Dialog("Are you a girl?"," Yes"," No")){
		
			if(!Lcd_Dialog("Let's be Friend?"," OK!"," No")){
				Lcd_MessageBox("Your are a good"," guy,Thank you!");
			}else{
				Lcd_MessageBox("Don't be shy~","");
			}
		Lcd_changeString("What'sYourAge?",age,'0','9');
	}else{
		Lcd_MessageBox("Would you ....."," ");
		Lcd_MessageBox("Would you .....","marry me?");
		if(!Lcd_Dialog("marry me?"," Yes"," No")){
			Lcd_MessageBox("I love You!","");	
		}else{
			Lcd_MessageBox("No!!!!!!!!","");	
		}
			
	}
	Lcd_Dialog("What is Hehe?"," Hehe"," ???");

}