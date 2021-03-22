/*
********************************************************************************
* 3ALogics Inc.
* 7th Floor, Hyundai-office Bldg., 9-4, Sunae-dong. Bundang-gu, Seongnam-si,
* Gyeonggi-do, 463-783, Korea
*
* (c) Copyright 2011, 3ALogics, Korea
*
* Hardware : xxxx
* Complier : WinAVR + AVR Studio
* Filename : common.h
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2011/06/20
* Description : 
********************************************************************************
*/
#ifndef _COMMON_H
#define _COMMON_H
#ifdef _SET_GLOBAL_
		#define _GLOBAL_
#else
		#define _GLOBAL_   extern
#endif
#define u8			unsigned char
#define u16			unsigned int
#define u32			unsigned long
#define u64			unsigned long long


#define iu8			volatile unsigned char
#define iu16			volatile unsigned int
#define iu32			volatile unsigned long
#define iu64			volatile unsigned long long


#define BYTE  unsigned char
#define WORD  unsigned int
#define DWORD unsigned long

//==============================================================================
//WinAVR Include header file
//==============================================================================
//#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "mifarelib.h"
#include "trh031m.h"
#include "rfproto.h"
#include "mcu.h"
#include "doorlock.h"

#include "e2prom.h"
#include "rtc.h"
#include "hotelcode.h"

//#include "des.h"
#include "24lc256.h"
#include "trh_type.h"
#include "reader.h"
#include "card.h"
#include "online.h"
//==============================================================================
// Variable redefine
//==============================================================================



volatile u8 LEN, ERROR;
//volatile u8 ATQA[2];
volatile u8 ACID[10];
volatile BYTE Sector_Key[6];
volatile BYTE Block_Address;

volatile u8 RFIDBUF[30];
volatile u8 CRYPTOBUF[20];
volatile u8 cid_toggle;
volatile unsigned char test[100];
unsigned char test_act,test_code;
//==============================================================================
//Functions Include header file
//==============================================================================
void UartInit(void);
void UART_TX(u8 tx);
void UART_H2A(u8 conv);
void UART_STRING(char *str);

void TRH_INIT(void);
void TRH_WRITE(u8 addr, u8 data);
u8 TRH_READ(u8 addr);

void ISO14443A_REG(void);
u8 ISO14443A_REQA(void);
u8 ISO14443A_ANTISELECT(u8 sel);


void ISO14443A_Memory(void);
void Init_Set_Data(void);


#endif
