/*
 * ADX345_Atmega32.c
 *
 * Created: 8/14/2015 8:22:04 PM
 *  Author: Er. Saroj Shakya
 */ 


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define LCD_DATA_PORT PORTA
#define LCD_DATA_DDR DDRA
#define LCD_RS 2
#define LCD_EN 3

unsigned char xyz_values[6];

void LCD_cmnd(unsigned char cmnd)
{
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd & 0xF0); //send upper 4 bit
	LCD_DATA_PORT &= ~(1<<LCD_RS); //0b11111011 //RS = 0
	LCD_DATA_PORT |= 1<<LCD_EN; //0b00001000 //EN = 1
	_delay_us(50);
	LCD_DATA_PORT &= ~(1<<LCD_EN); //0b11110111 //EN = 0
	_delay_us(200);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd << 4); //send lower 4 bit
	LCD_DATA_PORT |= 1<<LCD_EN; //0b00001000 //EN = 1
	_delay_us(50);
	LCD_DATA_PORT &= ~(1<<LCD_EN); //0b11110111 //EN = 0
}
void LCD_data(unsigned char data)
{
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
	LCD_DATA_PORT |= 1<<LCD_RS; //0b00000100 //RS = 1
	LCD_DATA_PORT |= 1<<LCD_EN; //0b00001000
	_delay_us(50);
	LCD_DATA_PORT &= ~(1<<LCD_EN); //0b11110111
	_delay_us(2000);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data << 4);
	LCD_DATA_PORT |= 1<<LCD_EN; //0b00001000
	_delay_us(2000);
	LCD_DATA_PORT &= ~(1<<LCD_EN); //0b11110111
}
void LCD_initialize(void)
{
	LCD_DATA_DDR = 0xFC;
	LCD_DATA_PORT &= ~(1<<LCD_EN); //0b11110111;
	_delay_ms(200);
	LCD_cmnd(0x33);
	_delay_ms(20);
	LCD_cmnd(0x32);
	_delay_ms(20);
	LCD_cmnd(0x28);
	_delay_ms(20);
	LCD_cmnd(0x0E);
	_delay_ms(20);
	LCD_cmnd(0x01);
	_delay_ms(20);
}
void LCD_clear(void)
{
	LCD_cmnd(0x01);
	_delay_ms(2);
}
void LCD_print(char * str)
{
	unsigned char i=0;
	while(str[i] != 0)
	{
		LCD_data(str[i]);
		i++;
		_delay_ms(5);
	}
}
void LCD_set_curser(unsigned char y, unsigned char x)
{
	if(y==1)
	LCD_cmnd(0x7F+x);
	else if(y==2)
	LCD_cmnd(0xBF+x);
}
void LCD_num(unsigned char num1)
{
	LCD_data(num1/10 + 0x30);
	LCD_data(num1%10 + 0x30);
}

void i2c_init()
{
	TWSR = 0x00;
	TWBR = 0x47;
	TWCR = 0x04;
}

void i2c_start()
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

void i2c_stop()
{
	TWCR = (1<< TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

unsigned char i2c_read(unsigned char isLast)
{
	if (isLast == 0)
	{
		TWCR = (1<<TWINT) | (1<<TWEN) | (TWEA);
	}
	else
	{
		TWCR = (1<<TWINT) | (1<<TWEN);
	}
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

int main(void)
{
	LCD_initialize();
	LCD_print("ADXL345 Test");
	_delay_ms(2000);
	LCD_clear();
	i2c_init();
	i2c_start();
	i2c_write(0x53);
	//LCD_print("1");
	i2c_stop();
	i2c_start();
	i2c_write(0x2D);
	LCD_print("2");	
	i2c_write(0x00);
	i2c_stop();
	i2c_start();
	i2c_write(0x53);
	i2c_write(0x2D);
	i2c_write(0x10);
	i2c_stop();
	i2c_start();
	i2c_write(0x53);
	i2c_write(0x2D);
	i2c_write(0x08);
	i2c_stop();
	LCD_print("here");
    while(1)
    {
       int xyz_register = 0x32;
	   //int x,y,z;
	   
	   i2c_start();
	   i2c_write(0x53);
	   i2c_write(xyz_register);
	   i2c_stop();
	   
	   i2c_start();
	   i2c_write(0x53);
	   xyz_values[0]=i2c_read(1);
	   i2c_stop();
	   LCD_num(xyz_values[0]);
	   LCD_print("here");
	   _delay_ms(2000);
    }
}