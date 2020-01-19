//i2c_ 开头的函数才是对外部的接口  其他全是static 函数 
#include "at24c02.h"
#include <intrins.h>
#include <reg52.h>

sbit AT24C02_SCLK = P0^1;
sbit AT24C02_IO = P0^0;


/*4个数据为一组， 第一个是从哪个小时开始，第二个是从几分钟开始，
第三个是持续多久时间（其他数据代表持续多少分钟(最多255分钟)）
第四个备用（等待开发模式）*/
unsigned char for_debug[8] = {0x00,0x05,10};
extern unsigned char for_test[8] = {0};

static void Delay200ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 2;
	j = 103;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

static void delay_5us(void)
{
	_nop_();
	_nop_();
}

static void start(void) //OK
{
	AT24C02_SCLK = 1;
	AT24C02_IO = 1;
	delay_5us();
	AT24C02_IO = 0;
	delay_5us();
	AT24C02_SCLK = 0;
	AT24C02_IO = 1;
}

static void stop(void)//OK
{
	AT24C02_SCLK = 0;
	_nop_();
	AT24C02_IO = 0;
	_nop_();
	AT24C02_SCLK = 1;
	delay_5us();
	AT24C02_IO = 1;
	delay_5us();
}

//传0是不应答
static void ack(bit dat) //OK
{
	AT24C02_IO = !dat;
	_nop_();
	AT24C02_SCLK = 1;
	delay_5us();
	AT24C02_SCLK = 0;
	_nop_();
	AT24C02_IO = 1;
}

static bit read_ack(void) //OK
{
	bit dat;
	AT24C02_SCLK = 1;//拉高是为了读从机的应答信号
	delay_5us();
	if(AT24C02_IO)
		dat = 0;
	else
		dat = 1;
	AT24C02_SCLK = 0;
	AT24C02_IO = 1;  //没事拉高
	return dat;
}

static void write_byte(unsigned char dat) //OK
{
	unsigned char i = 0;
	for(i = 0; i < 8; i++)
	{
		AT24C02_SCLK = 0;//时钟线拉低 IO 才能变；
		_nop_();
		if(dat & 0x80)  //发数据从高位发起
			AT24C02_IO = 1;
		else
			AT24C02_IO = 0;
		_nop_();
		AT24C02_SCLK = 1;//时钟线拉高 写入数据；
		_nop_();
		dat <<= 1;
		_nop_();
	}
	
	AT24C02_SCLK = 0;  //没事把时钟拉低；
	_nop_();
	AT24C02_IO = 1;    //没事把IO口拉高，为读ACK做准备；
}


static unsigned char read_byte(void)//OK
{
	unsigned char dat;
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		AT24C02_SCLK = 1;//拉搞时钟才能读取IO口的数据 
		dat <<= 1;//先移再写数据，如果先写在移。则会丢失最高一位。
		_nop_();
		if(AT24C02_IO)  //从高位开始发
			dat |= 0x01;
		else
			dat &= 0xfe;
		_nop_();
		AT24C02_SCLK = 0;//时钟线拉低 IO 才能变；
		_nop_();
	}
	//最后sclk = 0；
	AT24C02_IO = 1;  //没事拉高；
	return dat;
}

unsigned char i2c_read_byte(unsigned char addr)
{
	unsigned char dat;
	start();
	write_byte(0xa0);
	read_ack();
	write_byte(addr);
	read_ack();
	
	start();
	write_byte(0xa1);
	read_ack();

	dat = read_byte();
	ack(0);
	stop();
	return dat;
}
void i2c_write_byte(unsigned char byte_addr,unsigned char dat)
{
	start();
	write_byte(0xa0);
	read_ack();
	write_byte(byte_addr);
	read_ack();
	write_byte(dat);
	read_ack();
	stop();
	Delay200ms();
}

void i2c_burst_write(unsigned char byte_addr, unsigned char *dat_addr, unsigned char count)
{
	unsigned char i;
	start();
	write_byte(0xa0);
	read_ack();
	write_byte(byte_addr);
	read_ack();
	for(i = 0; i < count; i++)
	{
		write_byte(dat_addr[i]);
		read_ack();
	}
	stop();
	Delay200ms();
}

void i2c_burst_read(unsigned char begin_addr, unsigned char data_size, unsigned char * buf)
{
	unsigned char i;
	start();
	write_byte(0xa0);
	read_ack();
	write_byte(begin_addr);
	read_ack();
	
	start();
	write_byte(0xa1);
	read_ack();

	for(i = 0; i < data_size; i++)
	{
		*buf++ = read_byte();
		if(i+1 == data_size)
			ack(0);
		else
			ack(1);
	}
	stop();
	Delay200ms();
}

void i2c_init(void)
{
	i2c_burst_write(0x00,for_debug,8);
}












