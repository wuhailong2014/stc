#include "ds1302.h"
sbit DS1302_SCLK = P1^2;
sbit DS1302_IO = P1^1;
sbit DS1302_CE = P1^0;
	
extern unsigned char time_array[7] = {0x00,0x55,0x20,0x16,0x01,0x04,0x20};

static void write(unsigned char dat)  //静态函数  ds1302_write 调用
{
	unsigned char count = 0;
	for(count = 0; count < 8; count++)
	{
		DS1302_SCLK = 0;
		DS1302_IO = (bit)(dat & 0x01);
		dat >>= 1;
		DS1302_SCLK = 1;
	}
}

static unsigned char read()
{
	unsigned char dat = 0;
	unsigned char count = 0;
	for(count = 0; count < 8; count++)
	{
		DS1302_SCLK = 0;
		if(DS1302_IO)
			dat |= 1 << count;
		DS1302_SCLK = 1;
	}
	return dat;
}

void ds1302_write(unsigned char addr, unsigned char dat)
{
	DS1302_CE = 0;
	DS1302_SCLK = 0;
	DS1302_CE = 1;
	write(addr);
	write(dat);
	DS1302_SCLK = 0;
	DS1302_CE = 0;
}
unsigned char ds1302_read(unsigned char addr)
{
	unsigned char dat;
	DS1302_CE = 0;
	DS1302_SCLK = 0;
	DS1302_CE = 1;
	write(addr);
	dat = read();
	DS1302_SCLK = 0;
	DS1302_CE = 0;
	return dat;
}

void ds1302_burst_write()
{
	unsigned char count = 0;
	DS1302_CE = 1;
	write(0xbe);
	for(count = 0; count < 7; count++)
	{
		write(time_array[count]);
	}
	DS1302_CE = 0;
}
	
void ds1302_burst_read()
{
	unsigned char count = 0;
	DS1302_CE = 1;
	write(0xbf);
	for(count = 0; count < 7; count ++)
	{
		time_array[count] = read();
	}
	DS1302_CE = 0;
}

void ds1302_init(void)
{
	if((ds1302_read(0x81) & 0x80) == 0x80)
	{
		ds1302_write(0x8e,0x00);//关掉保护
		ds1302_burst_write();
		ds1302_write(0x8e,0x80);//开启保护
	}
	
}



