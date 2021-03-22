/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : rfproto.h
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/11
* Description : 
********************************************************************************
*/

#ifndef _RFPROTO_H
#define _RFPROTO_H


//------------------------------------------------------------------------------
//  ISO14443A UART Command Define
//------------------------------------------------------------------------------
#define COM_ISO14443A_REG					0xA1
#define COM_ISO14443A_REQA					0x26
#define COM_ISO14443A_WUPA					0x52
#define COM_ISO14443A_AntiCollison_LVEL1	0x93
#define COM_ISO14443A_AntiCollison_LVEL2	0x95
#define COM_ISO14443A_AntiCollison_LVEL3	0x97
#define COM_ISO14443A_SELECT_LVEL1			0x94
#define COM_ISO14443A_SELECT_LVEL2			0x96
#define COM_ISO14443A_SELECT_LVEL3			0x98
#define COM_ISO14443A_HLTA					0x50
#define COM_ISO14443A						0xF0
#define COM_ISO14443A_LOOP					0xF1


#endif
