/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : trh031m.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/11
* Description : 
********************************************************************************
*/
#include "common.h"


void TRH_INIT(void)
{
	TRH_RSTH;
	TRH_NCSL;
	TRH_NWRH;
	TRH_NRDH;
	TRH_ALEL;
	
	//_delay_ms(1);
	TRH_RSTL;
	//_delay_ms(1);
	delay_us(1);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);

}


void TRH_WRITE(u8 addr, u8 data)
{
	DDRA = 0xFF;
	PORTA = addr;
	TRH_ALEH;
	TRH_ALEL;
	PORTA = data;
	TRH_NWRL;
	TRH_NWRH;
}

u8 TRH_READ(u8 addr)
{
	u8 temp;
	DDRA = 0xFF;
	PORTA = addr;
	TRH_ALEH;
	TRH_ALEL;
	DDRA = 0x00;
//	DDRA = 0xFF;
	TRH_NRDL;
	_delay_us(1);	// -> 20110726 카드 read로 추가 
	temp = PINA;
	TRH_NRDH;
	return temp;
}
