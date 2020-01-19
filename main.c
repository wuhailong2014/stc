#include <reg52.h>
#include <stdio.h>
#include "ds1302.h"
#include "at24c02.h"

extern unsigned char time_array[7];
unsigned char temp_for_time_sec;
extern unsigned char for_test[8];
unsigned char count = 0;
sbit k_switch = P1^3;

void init()
{
	P2 = 0;
	//PCON = 0x80; //设置加倍
	EA = 1;        //总中断
	ES = 1;        //串口中断
	ET0 = 1;       //定时器中断
	SM0 = 0;       //SM0与SM1 配置串口模式，两位4种模式
	SM1 = 1;
	REN = 1;       //允许串口接收数据
	TMOD = 0x21;    //给定时器设置工作方式，两个一起设置。
	TH0 = 0;        //定时器初始值
	TL0 = 0;
	TH1 = 0xfd;  //9600波特率 （通过计算得出波特率）
	TL1 = 0xfd;
	TI = 1;
	TR1 = 1;     //开启定时器
	TR0 = 1;     //开启定时器
}


int main()
{
	unsigned char i = 0;
	k_switch = 0;
	init(); 
	ds1302_init();
	i2c_init();
	i2c_burst_read(0x00,8,for_test);
	while(1)
	{
		if((time_array[2] == for_test[0]) &&
			(time_array[1] >= for_test[1]) &&
			(time_array[1] < for_test[1]+for_test[2]))
			k_switch = 1;
		else
			k_switch = 0;
		
		if(time_array[0] != temp_for_time_sec)
		{
			temp_for_time_sec = time_array[0];
			printf("当前时间是%d时%d分%d秒",time_array[2],time_array[1],time_array[0]);
		}			
	}
}


void time0() interrupt 1
{
	TH0 = 0;
	TL0 = 0;
	ds1302_write(0x8e,0x00);//关掉保护
	ds1302_burst_read();
	ds1302_write(0x8e,0x80);//开启保护
	
	if(time_array[0] != temp_for_time_sec)
	{
		temp_for_time_sec = time_array[0];
//		if(!TI)
//		{
//			SBUF = time_array[count++];
//			count %= 7;
//		}
	}
}
		

//void serial() interrupt 4
//{
//	if(RI)
//		RI = 0;
//	if(TI)
//		TI = 0;
//}	









