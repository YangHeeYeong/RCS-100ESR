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
* Filename : main.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2011/06/20
* Description : 
********************************************************************************
*/
#define _SET_GLOBAL_
//#define nfc_debug 1
#include "common.h"
volatile unsigned char test[100];
volatile unsigned char DeviceMode = 0;
volatile BYTE NormalOpenMode = 0;
volatile BYTE DoorClose_B_Fg = 0;
volatile BYTE CLOSE_RESET = 0;
volatile unsigned int delay_nomal = 0;
volatile unsigned int delay_close = 0;
volatile unsigned int delay_openmode = 0;
unsigned char test_act_test_code;
volatile BYTE fElStart, fElEnd;

int main(void)
{
	BYTE i;

	System_Init();
	Timer0_mSec_Stop();
	Timer0_mSec_Start(10);
	
	//PORTA = 0xff;
	//WDT=1;
	TRH_INIT();
	TRH_WRITE(0x09, 0x01);
	for(i=0;i<6;i++) SECTORKEY[i] = 0xFF;

	//for(i=0;i<6;i++) SECTORKEY[i] = Sector_Key[i];

	//Init_Reader_Address();
	Init_Set_Data();
	delay_ms(100);
	 
	
	All_LED_Off();
	//BUZZER = 0;
	RelayOff();
	//RELAY = 0;

	UART0_RX_EN;
	
	test_act=1;
	test_code=0x01;
	//PORTB=0x02;
	//BuzzerOn();
	White_LED_On();
	
	Buzzer_On();
	RelayOn();
	delay_ms(100);
	RelayOff();
	Buzzer_Off();
	
	while(1)
	{	
		WDT;
		
		if(!NEW_CARD) ISO14443A_RunFunction(); //card 인식
	
		HCE_RxRunFunction();	// 정보조회기 (NFC)
		TRH_WRITE(0x11, 0x58);
		BLE_Rcv_Command();
		//Check_Main_Data(); 
		Check_Online_Data();
		
        	if(T1IRQ)
        	{  
            		T1IRQ = 0;
            		Timer1_Sec_Stop();

			if(NEW_CARD)
			{
				NEW_CARD = 0;
				DETECT_CARD = 0;
				fHandheld = 0;
				
				if(!Office)RelayOff();
				TRH_INIT();
				White_LED_On();
			}
			if(nErrorCode)
			{
				nErrorCode = 0;
			}
			if(fHandheld)
			{
				fHandheld = 0;
				White_LED_On();
			}
		}
		delay_ms(5);
		
	}

	
}


