/* File: lcd.c
 * Description: Hardware abstraction layer for LCD control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include "lcd.h"
 
 /* static void _delay_ms(int time)
{
  int cnt = time*1000;
  while(cnt --);
} */

/*****Function to Reset LCD*****/
static void lcdSet4Bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

	 
/*****Function to Write Command on LCD*****/
static void lcdWriteCommand(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

/*****Function to Write Data on LCD*****/
static void lcdWriteData(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

/*****Function to Initialize LCD*****/
static void lcdInit()
{
	_delay_ms(1);

	lcdWriteCommand(0x28);			//LCD 4-bit mode and 2 lines.
	lcdWriteCommand(0x01);
	lcdWriteCommand(0x06);
	lcdWriteCommand(0x0C);
	lcdWriteCommand(0x80);
		
}

STATUS initLcd(void) {
	/* LCD pin config */
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
	
	/* Set 4 bit mode and clear LCD screen */
	lcdSet4Bit();
	lcdInit();
	
	return STATUS_OK;
 }
 
void lcdHome()
{
	INT_LOCK();
	lcdWriteCommand(0x80);
	INT_UNLOCK();
}

void lcdClear()
{
	INT_LOCK();
	lcdWriteCommand(0x01);
	INT_UNLOCK();
}

/*****Function to Print String on LCD*****/
void lcdString(char *str)
{
	INT_LOCK();
	while(*str != '\0')
	{
		lcdWriteData(*str);
		str++;
	}
	INT_UNLOCK();
}

/*** Position the LCD cursor at "row", "column". ***/

void lcdCursor (int row, int column)
{
	INT_LOCK();
	switch (row) {
		case 1: lcdWriteCommand (0x80 + column - 1); break;
		case 2: lcdWriteCommand (0xc0 + column - 1); break;
		case 3: lcdWriteCommand (0x94 + column - 1); break;
		case 4: lcdWriteCommand (0xd4 + column - 1); break;
		default: break;
	}
	INT_UNLOCK();
}

/***** Function To Print Any input value upto the desired digit on LCD *****/
/* static void lcdPrint (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcdHome();
	}
	else
	{
		lcdCursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcdWriteData(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcdWriteData(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcdWriteData(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcdWriteData(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcdWriteData(unit);
	}
	if(digits>5)
	{
		lcdWriteData('E');
	}
	
} */
