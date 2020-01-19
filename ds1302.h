#ifndef __DS1302_H_
#define __DS1302_H_
#include <intrins.h>
#include <reg52.h>

void ds1302_init(void);
void ds1302_write(unsigned char addr, unsigned char dat);
void ds1302_burst_write();
void ds1302_burst_read();
unsigned char ds1302_read(unsigned char addr);

#endif