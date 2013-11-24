#include "lcd12864.h"
#include "comm.h"
#include "serial.h"
#include "keyBoard.h"
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
	//�ı䵽����ʱ�������øߵ�ƽ����Ȼ�Ǻ��ˡ�
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
void Lcd_Input_long(long num) 	//��������
{
	
 	ulong temp;
	uint i=0,j; 
 	if(num<0){Lcd12864_WriteData('-');num=-num;} //���������
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
//���͸�������precise��ʶ����
{
	long temp;
 	if(num<0){Lcd12864_WriteData('-');num=-num;} //���������	
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
	//����Key_enter�϶�����keyL��R
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
//��lowerChar��UpperChar֮���ĳ��������һ���ִ���
	uchar idx=0;
	Lcd_Clear();
	Lcd_Display_str(0,0,text);
	for(idx=0;str[idx]!=0;idx++){
		Lcd_Locate(idx*2,1);		
		Lcd12864_WriteData(str[idx]);
	}			
	idx=0;
	Lcd12864_WriteCommand(0x0f);
	//���������˸
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
	//���ع��
	Lcd_Clear();
	delay(50);	
}	
void Lcd_DrawMode(){
	Lcd_Clear();
	Lcd12864_WriteCommand(0x34);//������ָ��
	Lcd12864_WriteCommand(0x36);//����ͼָ��

}
uchar Lcd_SubLineEn=0;
void Lcd_DrawSubLine(uchar * str){
	Lcd12864_WriteCommand(0x32);//��������ָ��
	//delay(1);
	Lcd_Display_str(0,3,"                ");
	Lcd_Display_str(0,3,str);
	Lcd12864_WriteCommand(0x36);//������ָ��
	Lcd_SubLineEn=18;	
}
void Lcd_ClearSubLine(){
	Lcd12864_WriteCommand(0x32);//��������ָ��
	Lcd_Display_str(0,3,"                ");
	Lcd12864_WriteCommand(0x36);//������ָ��
}

void Lcd_DrawPicture(uchar picture[64][16],uint i){
	char mx,my;
	uchar * Pdat;
	mx=0;
	my=0;
	if(Lcd_SubLineEn>0)Lcd_SubLineEn--;
	if(Lcd_SubLineEn==1)Lcd_ClearSubLine();
//	_send_value(Lcd_SubLineEn);
	if(0){
//	if(!(i%2)){
		switch(i/2){
			case(0):Lcd_DrawSubLine("�Ǻ���Ļ��");break;
			case(56):Lcd_DrawSubLine("�̲غڰ�������");break;
			case(63):Lcd_DrawSubLine("Կ�װ�");break;
			case(79):Lcd_DrawSubLine("������ǰ��ʾ");break;
			case(85):Lcd_DrawSubLine("����ʵ��������");break;
			case(94):Lcd_DrawSubLine("������Լ");break;
			case(102):Lcd_DrawSubLine("Сӣ������");break;
			case(113):Lcd_DrawSubLine("Release!!");break;
			case(206):Lcd_DrawSubLine("�磡");break;
			case(303):Lcd_DrawSubLine("������");break;
			case(308):Lcd_DrawSubLine("��������Ү");break;
			case(328):Lcd_DrawSubLine("û�뵽�ܶ���");break;
			case(334):Lcd_DrawSubLine("��ô�صĴ���");break;
			case(345):Lcd_DrawSubLine("������ʲô����");break;
			case(358):Lcd_DrawSubLine("������֪����");break;
			case(370):Lcd_DrawSubLine("ߴ�ﹾ��ߴ�ﹾ��");break;
			case(390):Lcd_DrawSubLine("����.....");break;
			case(399):Lcd_DrawSubLine("���ȴ���ɣ�");break;
			case(456):Lcd_DrawSubLine("��һ�£�");break;
			case(472):Lcd_DrawSubLine("�������ƶ԰�");break;
			case(504):Lcd_DrawSubLine("Ҫ��Ҫ����");break;
			case(510):Lcd_DrawSubLine("��һ��������");break;
			case(524):Lcd_DrawSubLine("ΪʲôҪ��������");break;
			case(536):Lcd_DrawSubLine("���ƺܽ���");break;
			case(542):Lcd_DrawSubLine("�����Լ�������");break;
			case(560):Lcd_DrawSubLine("֤�ݾ�������");break;
			case(598):Lcd_DrawSubLine("������");break;
			case(607):Lcd_DrawSubLine("����");break;
			case(625):Lcd_DrawSubLine("�������ˣ�");break;
			case(679):Lcd_DrawSubLine("�㿴��");break;
			case(683):Lcd_DrawSubLine("��������������");break;
			case(704):Lcd_DrawSubLine("ʲô���ò�����");break;
			case(712):Lcd_DrawSubLine("�İ���");break;
			case(715):Lcd_DrawSubLine("�㻹�������˵");break;
		    case(723):Lcd_DrawSubLine("��ʲô��������");break;
			case(737):Lcd_DrawSubLine("Ҫ��ô�����Ȱ���");break;
			case(757):Lcd_DrawSubLine("��һ￴����");break;
			case(763):Lcd_DrawSubLine("Ӧ�úܺý����");break;
			case(774):Lcd_DrawSubLine("���������ˣ���");break;
			case(795):Lcd_DrawSubLine("����ô����");break;
			case(800):Lcd_DrawSubLine("Ӯ����");break;
			case(810):Lcd_DrawSubLine("���ٲ�˵������");break;
			case(817):Lcd_DrawSubLine("һֱ׷�����ǰ�");break;
			case(836):Lcd_DrawSubLine("��....�κӣ�");break;
			case(956):Lcd_DrawSubLine("����ԭ��");break;
			case(960):Lcd_DrawSubLine("��ô�ɰ���");break;
			case(973):Lcd_DrawSubLine("���С����");break;
			case(979):Lcd_DrawSubLine("�����Դ����");break;
			case(1050):Lcd_DrawSubLine("С��");break;
			case(1059):Lcd_DrawSubLine("�е�Σ��ʱ");break;
			case(1064):Lcd_DrawSubLine("��ͷ��ְ�");break;
			case(1124):Lcd_DrawSubLine("С�ɣ�");break;
			case(1130):Lcd_DrawSubLine("Сӣ������ѽ��");break;
			case(1154):Lcd_DrawSubLine("��ѽ���÷���");break;
			case(1160):Lcd_DrawSubLine("�Ը��Ǽһ");break;
			case(1174):Lcd_DrawSubLine("�ҵ���û�취����");break;
			case(1253):Lcd_DrawSubLine("����");break;
			case(1272):Lcd_DrawSubLine("���ͣ�");break;
			case(1417):Lcd_DrawSubLine("ʱ��");break;
			case(1671):Lcd_DrawSubLine("�ò�����Ӯ�˰ɣ�");break;
			case(1683):Lcd_DrawSubLine("������Ү");break;
			case(1692):Lcd_DrawSubLine("��Ӯ������Ӯ����");break;
			case(1714):Lcd_DrawSubLine("лл��");break;
			case(1719):Lcd_DrawSubLine("������Ϊ��İ�æ");break;
			case(1754):Lcd_DrawSubLine("Сӣ��");break;
			case(1757):Lcd_DrawSubLine("����ӡ����");break;
			case(1798):Lcd_DrawSubLine("�ָ���ԭ��������");break;
			
			case(1814):Lcd_DrawSubLine("�����ƣ�");break;
			case(1942):Lcd_DrawSubLine("������");break;
			case(1946):Lcd_DrawSubLine("�����鷳����");break;
			case(1960):Lcd_DrawSubLine("��ɵ�̫����");break;
			case(1970):Lcd_DrawSubLine("�һ���Ϊ��������");break;
			case(1987):Lcd_DrawSubLine("�ղ���Σ����");break;
			case(2014):Lcd_DrawSubLine("��֪������");break;
			case(2020):Lcd_DrawSubLine("����ɾ���û");break;
			case(2038):Lcd_DrawSubLine("С�ɣ������ƣ�");break;
			case(2060):Lcd_DrawSubLine("��Ȼ��ʹ�ú�");break;
			case(2066):Lcd_DrawSubLine("�ͻ���������");break;
			case(2090):Lcd_DrawSubLine("Сӣ");break;
			case(2094):Lcd_DrawSubLine("�������������");break;
			case(2112):Lcd_DrawSubLine("�ף�");break;
			case(2215):Lcd_DrawSubLine("֪����");break;
			case(2235):Lcd_DrawSubLine("���ڿ�ʲô��");break;
			case(2248):Lcd_DrawSubLine("Сӣ");break;
			case(2258):Lcd_DrawSubLine("������Ƭ��....");break;
			case(2270):Lcd_DrawSubLine("лл���һ�����");break;
			case(2277):Lcd_DrawSubLine("��Ϊ�����");break;
			case(2285):Lcd_DrawSubLine("�㲻���ø�");break;
			case(2291):Lcd_DrawSubLine("���˿���");break;
			case(2305):Lcd_DrawSubLine("�ã�");break;
			case(2320):Lcd_DrawSubLine("�磡");break;
			case(2332):Lcd_DrawSubLine("�簲��");break;
			case(2363):Lcd_DrawSubLine("��˵���췢��");break;
			case(2370):Lcd_DrawSubLine("�ܶ�����");break;
			case(2378):Lcd_DrawSubLine("���ǲ�Ҫ����");break;
			case(2387):Lcd_DrawSubLine("�ǣ�");break;
			case(2393):Lcd_DrawSubLine("�����������Ƭ��");break;
			case(2410):Lcd_DrawSubLine("��");break;
			case(2440):Lcd_DrawSubLine("��......");break;
			case(2453):Lcd_DrawSubLine("����......");break;
			case(2463):Lcd_DrawSubLine("��ô��......");break;
			case(2471):Lcd_DrawSubLine("�ҵ�ħ��...");break;
			case(2477):Lcd_DrawSubLine("˵�����ᴩ��....");break;
			case(2488):Lcd_DrawSubLine("Сӣ��û�뵽");break;
			case(2494):Lcd_DrawSubLine("���������ô��");break;
			//case(2510):Lcd_DrawSubLine("�ǺǺǺ�");break;
			case(2523):Lcd_DrawSubLine("��..���Ǻϳ���Ƭ");break;
			case(2538):Lcd_DrawSubLine("����Ҳ����");break;
			case(2553):Lcd_DrawSubLine("û��ħ���Ļ�");break;
			case(2561):Lcd_DrawSubLine("�����¸���");break;
			case(2567):Lcd_DrawSubLine("�첻����");break;
		}
	}

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
		Pdat=picture[63-my];
		delayus(10);
		while(mx<16){
			if(mx==8){
				Pdat=picture[31-my];
			}
			P0=*(Pdat++);
			//P0=picture[my+mx/8*32][mx%8*2];
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
		//	_send_value((mx%8*2+1));
			//delay(10);

		}
		mx=0;
		
		my++;
		//_send_value((my+mx/8*32));
		//delay(100);

	}
	delay(5);
//	Lcd12864_WriteCommand(0x32);//��������ָ��
//	Lcd12864_WriteCommand(0x30);//�ػ���
}