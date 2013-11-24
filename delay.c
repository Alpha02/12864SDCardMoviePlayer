#include "comm.h"
#include<delay.h>
#define uint unsigned int 
#define uchar unsigned char
void delay(uint z){
	uint x,y,i;
	for(i=0;i<8;i++){
		for(x=z;x>0;x--){
			for(y=110;y>0;y--);
		}
	}
}
void delay_us(uint z){
	uint i,j;
	for(i=0;i<8;i++){
		 for(j=0;j<z;j++){
			   _nop_();
		 }
	}
}