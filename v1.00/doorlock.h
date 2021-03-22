/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : doorlock.h
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/12
* Description : 
********************************************************************************
*/
#ifndef _DOORLOCK_H
#define _DOORLOCK_H

#ifdef _SET_GLOBAL_
		#define _GLOBAL_
#else
		#define _GLOBAL_   extern
#endif

//extern unsigned char secu_sw_flag;
//extern unsigned char door_sw_flag;
//extern unsigned char SwitchMode;
//extern volatile unsigned char SuccessBuzzerOnFg;
//extern volatile unsigned char FailedBuzzerOnFg;
//extern volatile unsigned char BuzzzerCnt;
//_GLOBAL_ volatile BYTE TOV1Count, TOV0Count;
//------------------------------------------------------------------------------
//  Periperal I/O Port Control Functions
//------------------------------------------------------------------------------
void Mode_Set(void);
//void EmergencyCheck(void);
//void Emergency_Function(void);
//void DoorLock_Function(void);
//void DoorOpenCheck(void);
//void Doorlock_Set(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void BuzzerOn(void);
void BuzzerOff(void);
void Card_Sucess(void);
void Card_Failed(void);
unsigned char UART0_Rcv_Cmd(void);
void Doorlock_Open(void);
void Doorlock_Close(void);
void I2C_Write_Byte(unsigned char waddr);
void FailedBuzzerOnProcess(void);
void ReaderRunFunction(void);

#endif
