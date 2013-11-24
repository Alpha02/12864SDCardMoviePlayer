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
uchar Lcd_SubLineEn=0;
void Lcd_DrawSubLine(uchar * str){
	Lcd12864_WriteCommand(0x32);//换到基本指令
	//delay(1);
	Lcd_Display_str(0,3,"                ");
	Lcd_Display_str(0,3,str);
	Lcd12864_WriteCommand(0x36);//开扩充指令
	Lcd_SubLineEn=18;	
}
void Lcd_ClearSubLine(){
	Lcd12864_WriteCommand(0x32);//换到基本指令
	Lcd_Display_str(0,3,"                ");
	Lcd12864_WriteCommand(0x36);//开扩充指令
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
			case(0):Lcd_DrawSubLine("呵呵字幕组");break;
			case(56):Lcd_DrawSubLine("蕴藏黑暗力量的");break;
			case(63):Lcd_DrawSubLine("钥匙啊");break;
			case(79):Lcd_DrawSubLine("在我面前显示");break;
			case(85):Lcd_DrawSubLine("你真实的力量吧");break;
			case(94):Lcd_DrawSubLine("根据契约");break;
			case(102):Lcd_DrawSubLine("小樱命令你");break;
			case(113):Lcd_DrawSubLine("Release!!");break;
			case(206):Lcd_DrawSubLine("风！");break;
			case(303):Lcd_DrawSubLine("大象差点");break;
			case(308):Lcd_DrawSubLine("就受伤了耶");break;
			case(328):Lcd_DrawSubLine("没想到能丢起");break;
			case(334):Lcd_DrawSubLine("这么重的大象");break;
			case(345):Lcd_DrawSubLine("到底是什么牌呢");break;
			case(358):Lcd_DrawSubLine("啊！我知道了");break;
			case(370):Lcd_DrawSubLine("叽里咕噜叽里咕噜");break;
			case(390):Lcd_DrawSubLine("但是.....");break;
			case(399):Lcd_DrawSubLine("快点救大象吧！");break;
			case(456):Lcd_DrawSubLine("等一下！");break;
			case(472):Lcd_DrawSubLine("你是力牌对吧");break;
			case(504):Lcd_DrawSubLine("要不要跟我");break;
			case(510):Lcd_DrawSubLine("比一比力气？");break;
			case(524):Lcd_DrawSubLine("为什么要比力气？");break;
			case(536):Lcd_DrawSubLine("力牌很骄傲");break;
			case(542):Lcd_DrawSubLine("对于自己的力量");break;
			case(560):Lcd_DrawSubLine("证据就在那里");break;
			case(598):Lcd_DrawSubLine("啊！！");break;
			case(607):Lcd_DrawSubLine("跳！");break;
			case(625):Lcd_DrawSubLine("它过来了！");break;
			case(679):Lcd_DrawSubLine("你看！");break;
			case(683):Lcd_DrawSubLine("还好你用了跳牌");break;
			case(704):Lcd_DrawSubLine("什么还好不还好");break;
			case(712):Lcd_DrawSubLine("的啊！");break;
			case(715):Lcd_DrawSubLine("你还不快跟它说");break;
		    case(723):Lcd_DrawSubLine("用什么比力气？");break;
			case(737):Lcd_DrawSubLine("要怎么跟它比啊？");break;
			case(757):Lcd_DrawSubLine("这家伙看起来");break;
			case(763):Lcd_DrawSubLine("应该很好解决吧");break;
			case(774):Lcd_DrawSubLine("啊！又来了！！");break;
			case(795):Lcd_DrawSubLine("我怎么可能");break;
			case(800):Lcd_DrawSubLine("赢它嘛");break;
			case(810):Lcd_DrawSubLine("你再不说，它会");break;
			case(817):Lcd_DrawSubLine("一直追着我们啊");break;
			case(836):Lcd_DrawSubLine("拔....拔河！");break;
			case(956):Lcd_DrawSubLine("力牌原来");break;
			case(960):Lcd_DrawSubLine("这么可爱啊");break;
			case(973):Lcd_DrawSubLine("你别小看它");break;
			case(979):Lcd_DrawSubLine("否则会吃大亏噢");break;
			case(1050):Lcd_DrawSubLine("小可");break;
			case(1059):Lcd_DrawSubLine("感到危险时");break;
			case(1064):Lcd_DrawSubLine("你就放手吧");break;
			case(1124):Lcd_DrawSubLine("小可！");break;
			case(1130):Lcd_DrawSubLine("小樱！用牌呀！");break;
			case(1154):Lcd_DrawSubLine("快呀！用风牌");break;
			case(1160):Lcd_DrawSubLine("对付那家伙！");break;
			case(1174):Lcd_DrawSubLine("我的手没办法动啊");break;
			case(1253):Lcd_DrawSubLine("大象！");break;
			case(1272):Lcd_DrawSubLine("加油！");break;
			case(1417):Lcd_DrawSubLine("时！");break;
			case(1671):Lcd_DrawSubLine("该不会是赢了吧？");break;
			case(1683):Lcd_DrawSubLine("好像是耶");break;
			case(1692):Lcd_DrawSubLine("我赢啦！我赢啦！");break;
			case(1714):Lcd_DrawSubLine("谢谢！");break;
			case(1719):Lcd_DrawSubLine("都是因为你的帮忙");break;
			case(1754):Lcd_DrawSubLine("小樱！");break;
			case(1757):Lcd_DrawSubLine("快点封印它啊");break;
			case(1798):Lcd_DrawSubLine("恢复你原来的样子");break;
			
			case(1814):Lcd_DrawSubLine("库洛牌！");break;
			case(1942):Lcd_DrawSubLine("真是张");break;
			case(1946):Lcd_DrawSubLine("爱惹麻烦的牌");break;
			case(1960):Lcd_DrawSubLine("你干得太好啦");break;
			case(1970):Lcd_DrawSubLine("我还以为我输了呢");break;
			case(1987):Lcd_DrawSubLine("刚才真危险呢");break;
			case(2014):Lcd_DrawSubLine("不知道树懒");break;
			case(2020):Lcd_DrawSubLine("提起干劲了没");break;
			case(2038):Lcd_DrawSubLine("小可，这张牌？");break;
			case(2060):Lcd_DrawSubLine("当然是使用后");break;
			case(2066):Lcd_DrawSubLine("就会力大无穷");break;
			case(2090):Lcd_DrawSubLine("小樱");break;
			case(2094):Lcd_DrawSubLine("我有事想拜托你");break;
			case(2112):Lcd_DrawSubLine("咦？");break;
			case(2215):Lcd_DrawSubLine("知世！");break;
			case(2235):Lcd_DrawSubLine("你在看什么？");break;
			case(2248):Lcd_DrawSubLine("小樱");break;
			case(2258):Lcd_DrawSubLine("这张照片是....");break;
			case(2270):Lcd_DrawSubLine("谢谢！我会留下");break;
			case(2277):Lcd_DrawSubLine("作为纪念的");break;
			case(2285):Lcd_DrawSubLine("你不能拿给");break;
			case(2291):Lcd_DrawSubLine("别人看噢");break;
			case(2305):Lcd_DrawSubLine("好！");break;
			case(2320):Lcd_DrawSubLine("早！");break;
			case(2332):Lcd_DrawSubLine("早安！");break;
			case(2363):Lcd_DrawSubLine("听说昨天发生");break;
			case(2370):Lcd_DrawSubLine("很多事噢");break;
			case(2378):Lcd_DrawSubLine("你们不要紧吧");break;
			case(2387):Lcd_DrawSubLine("是！");break;
			case(2393):Lcd_DrawSubLine("那是昨天的照片吗？");break;
			case(2410):Lcd_DrawSubLine("嗯");break;
			case(2440):Lcd_DrawSubLine("啊......");break;
			case(2453):Lcd_DrawSubLine("这是......");break;
			case(2463):Lcd_DrawSubLine("怎么办......");break;
			case(2471):Lcd_DrawSubLine("我的魔法...");break;
			case(2477):Lcd_DrawSubLine("说不定会穿帮....");break;
			case(2488):Lcd_DrawSubLine("小樱，没想到");break;
			case(2494):Lcd_DrawSubLine("你的力气这么大啊");break;
			//case(2510):Lcd_DrawSubLine("呵呵呵呵");break;
			case(2523):Lcd_DrawSubLine("那..那是合成照片");break;
			case(2538):Lcd_DrawSubLine("我想也是呢");break;
			case(2553):Lcd_DrawSubLine("没有魔力的话");break;
			case(2561):Lcd_DrawSubLine("这种事根本");break;
			case(2567):Lcd_DrawSubLine("办不到吧");break;
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
//	Lcd12864_WriteCommand(0x32);//换到基本指令
//	Lcd12864_WriteCommand(0x30);//关画面
}