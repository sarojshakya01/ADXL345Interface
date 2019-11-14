#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "twi.h"
#include "twi.c"

#define LCD_DATA_PORT PORTA
#define LCD_DATA_DDR DDRA
#define LCD_RS 2
#define LCD_EN 3

volatile int16_t xData, yData, zData;

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

void LCD_num(int16_t num1)
{
	LCD_data(num1/100+0x30);
	num1 = num1%100;
	LCD_data(num1/10 + 0x30);
	LCD_data(num1%10 + 0x30);
}

uint8_t ADXL345_Init(const uint8_t Address)
{
	TWI_InitStandard();

	TWI_BeginTransmission(0x53);
	TWI_Write(0x00);
	TWI_EndTransmission();
	
	TWI_BeginTransmission(0x53);
	TWI_Write(0x2D);
	TWI_Write(0x00);
	TWI_EndTransmission();
	
	TWI_BeginTransmission(0x53);
	TWI_Write(0x2D);
	TWI_Write(0x10);
	TWI_EndTransmission();
	
	TWI_BeginTransmission(0x53);
	TWI_Write(0x2D);
	TWI_Write(0x08);
	TWI_EndTransmission();
	
	return 1;
}

void ADXL345_read_xyz()
{
	TWI_BeginTransmission(0x53);
	TWI_Write(0x32);
	TWI_EndTransmission();
	
	uint8_t data[6];
	uint8_t* dataPointer = data;
	TWI_RequestFrom(0x53, dataPointer, 6);
	TWI_EndTransmission();
	
	xData = (data[1] << 8) + data[0];
	yData = (data[3] << 8) + data[2];
	zData = (data[5] << 8) + data[4];
}

void print_xyz()
{
	LCD_print("  x    y    z");
	LCD_set_curser(2,1);
	if (xData<0)
	{
		LCD_data('-');
		xData = (-1) * xData;
	}
	LCD_set_curser(2,2);
	LCD_num(xData);
	
	LCD_data(' ');
	
	if (yData<0)
	{
		LCD_data('-');
		yData = (-1) * yData;
	}
	LCD_set_curser(2,7);
	LCD_num(yData);
	
	LCD_data(' ');
	
	if (zData<0)
	{
		LCD_set_curser(2,11);
		LCD_data('-');
		zData = (-1) * zData;
	}
	LCD_set_curser(2,12);
	LCD_num(zData);
}

int main()
{
	LCD_initialize();
	LCD_cmnd(0x0C);
	LCD_print("ADXL345 Test");
	_delay_ms(2000);
	LCD_clear();
	if(ADXL345_Init(0x52)==1);
	{
	while(1)
	{
		ADXL345_read_xyz();
		print_xyz();
		_delay_ms(1000);
		LCD_clear();
	}
	}
}