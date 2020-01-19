#ifndef __AT24C02_H_
#define __AT24C02_H_

#define AT24C02_ADDR 0xA0


void i2c_init(void);
unsigned char i2c_read_byte(unsigned char);
void i2c_write_byte(unsigned char, unsigned char);
void i2c_burst_write(unsigned char, unsigned char *, unsigned char );
void i2c_burst_read(unsigned char, unsigned char, unsigned char *);

#endif