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
* Filename : iso14443a.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2011/06/20
* Description : 
********************************************************************************
*/
#include "common.h"

//==============================================================================
// ISO14443A Functions
//==============================================================================

//volatile BYTE DETECT_14443A;
//volatile BYTE DETECT_14443A_MEM;
//BYTE CARD_DATA[20];

void ISO14443A_REG(void)
{
	//WDT;
	TRH_WRITE(0x11, 0x5B);
	TRH_WRITE(0x12, 0x3F); //수정
	TRH_WRITE(0x19, 0x00); //수정
	TRH_WRITE(0x1C, 0xC5);
	
	TRH_WRITE(0x14, 0x01);
	TRH_WRITE(0x1E, 0x01);
	TRH_WRITE(0x21, 0x06);
	TRH_WRITE(0x26, 0x02);
	delay_ms(4);
}

u8 ISO14443A_REQA(void)
{
	WDT;
	TRH_WRITE(0x0F, 0x07);
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x02, 0x26);
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(1);
	//delay_us(380);
	delay_ms(1);
	
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	delay_ms(1);
	//UART_STRING("ISO14443A ATQA Fail!!\n");
	if((LEN == 2)&&!(ERROR&0x02))
	{
		ATQA[0] = TRH_READ(0x02);
		ATQA[1] = TRH_READ(0x02);		
		//UART_STRING("ISO14443A ATQA = ");UART_H2A(ATQA[0]);UART_H2A(ATQA[1]);UART_TX(0x0A);
		return 1;
	}
	else
	{
		//UART_STRING("ISO14443A ATQA Fail!!\n");
		TRH_WRITE(0x11, 0x58);
		delay_ms(5);
		return 0;
	}
}

u8 ISO14443A_ANTISELECT(u8 sel)
{
	
  u8 i;
  u8 tmp;
  
  //WDT;
	TRH_WRITE(0x0F, 0x00);
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x02, sel);
	TRH_WRITE(0x02, 0x20);
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(2);
	//delay_us(650);
	delay_ms(2);
  
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN==5)&&!(TRH_ERR&0x02))
  {
    for(i=0;i<5;i++) UID[i] = TRH_READ(0x02);
  }
  else return 0;
  

  //###################################################################
  TRH_WRITE(0x22, 0x0F);
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x02, sel);
  TRH_WRITE(0x02, 0x70);
  for(i=0;i<5;i++) TRH_WRITE(0x02, UID[i]);
  //###################################################################
  TRH_WRITE(0x01, 0x14);
  //###################################################################
  delay_ms(3);
  //###################################################################
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN==1)&&!(TRH_ERR&0x08)&&!(TRH_ERR&0x02))
  {
    SAK = TRH_READ(0x02);
    
    
    
    tmp = SAK;
    tmp &= 0x64;
    if(tmp == 0x00) return 1;      //not ISO14443A part 4
    else if(tmp == 0x04) return 2; //UID not complete
    else if(tmp == 0x24) return 2; //UID not complete
    else if(tmp == 0x20) return 3; //ISO14443A part 4
    else if(tmp == 0x40) return 5; //Smart Phone
    else if(tmp == 0x60) return 5; //Smart Phone
    else return 0;
  }
  else return 0;
}

u8 Mifare_Authentication(u8 keysel, u8 addr)
{
	u8 i;
	u8 temp;
	u8 CardBuf[4];
	u8 TempDataBuf[8];
	u8 TempBitBuf[8];

	//Get Challenge
	TRH_WRITE(0x22, 0x07);
	TRH_WRITE(0x09, 0x01);
	if(keysel == 0) TRH_WRITE(0x02, 0x60);
	else TRH_WRITE(0x02, 0x61);
	TRH_WRITE(0x02, addr);
	
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(2);
	delay_us(800);
	
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	
	if((LEN == 4)&&!(ERROR&0x02))
	{
		for(i=0;i<4;i++) CardBuf[i] = TRH_READ(0x02);
		for(i=0;i<4;i++) TempDataBuf[i] = ACID[i];
		for(i=0;i<4;i++) TempDataBuf[i+4] = CardBuf[i];
		
		temp = MifareCrypto1Function(8, TempDataBuf, TempBitBuf, 0);
		
		TRH_WRITE(0x22, 0x03);
		TRH_WRITE(0x09, 0x01);
		TRH_WRITE(0x3A, 0x10);
		for(i=0;i<8;i++)
		{
			TRH_WRITE(0x27, TempBitBuf[i]);
		    TRH_WRITE(0x02, TempDataBuf[i]);
		}
		
		TRH_WRITE(0x01, 0x14);
	
		_delay_ms(2);
		//_delay_ms(1);
		//delay_us(200);
		
		TRH_WRITE(0x3A, 0x00);
		
		LEN = TRH_READ(0x04);
		ERROR = TRH_READ(0x0A);
		TRH_WRITE(0x01, 0x00);
		TRH_WRITE(0x2E, 0x00);
		
		if(LEN == 4)
		{
			for(i=0;i<4;i++) TempDataBuf[i] = TRH_READ(0x02);
			for(i=0;i<4;i++) TempDataBuf[i+4] = CardBuf[i];
			temp = MifareCrypto1Function(8, TempDataBuf, TempBitBuf, 1);
			
			if((temp == 1)&&(TempDataBuf[0] == 0xAA))
			{
				return 1;
			}
			else return 0;
		}
		else return 0;
	}
	else return 0;
}

u8 Mifare_Read(u8 addr)
{
	u8 i;
	u8 temp;
	u8 TempDataBuf[20];
	u8 TempBitBuf[20];

	
	TempDataBuf[0] = 0x30;
	TempDataBuf[1] = addr;
	
	temp = MifareCrypto1Function(2, TempDataBuf, TempBitBuf, 2);
	
	if(temp == 0) return 0;
	
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x3A, 0x10);
	for(i=0;i<temp;i++){
		TRH_WRITE(0x27, TempBitBuf[i]);
	    TRH_WRITE(0x02, TempDataBuf[i]);
	}
	
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(10);
	delay_ms(3);
	delay_us(100);
	
	TRH_WRITE(0x3A, 0x00);
		
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	if(LEN == 18){
		for(i=0;i<18;i++) 
		{
			TempDataBuf[i] = TRH_READ(0x02);
			//CARD_DATA[i] = TempDataBuf[i];
		}
		temp = MifareCrypto1Function(18, TempDataBuf, TempBitBuf, 4);

		for(i=0;i<temp;i++) CARD_DATA[i] = TempDataBuf[i];
		if(temp != 0)
		{
			DETECT_14443A_MEM = 1;
			/*
			UART0_TX_EN;	
			UART_STRING("Card Memory Data = ");
			for(i=0;i<temp;i++) UART_H2A(TempDataBuf[i]);
			UART_TX(0x0A);
			for(i=0;i<temp;i++) UART_H2A(CARD_DATA[i]);
			delay_us(800);
			UART0_RX_EN;
			*/
		}
		else return 0;
	}
	
	return 0;
}
/*
void MIFARE_WRITE(u8 addr, char *txdata)
{
	u8 i;
	u8 temp;
	u8 check;
	u8 TempDataBuf[20];
	u8 TempBitBuf[20];

	TempDataBuf[0] = 0xA0;
	TempDataBuf[1] = addr;
	
	temp = MifareCrypto1Function(2, TempDataBuf, TempBitBuf, 3);
	
	if(temp == 0) return;
	
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x3A, 0x10);
	for(i=0;i<temp;i++){
		TRH_WRITE(0x27, TempBitBuf[i]);
	    TRH_WRITE(0x02, TempDataBuf[i]);
	}
	
	TRH_WRITE(0x01, 0x14);
	
	_delay_ms(1);
	
	TRH_WRITE(0x3A, 0x00);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	for(i=0;i<16;i++) TempDataBuf[i] = txdata[i];
	temp = MifareCrypto1Function(16, TempDataBuf, TempBitBuf, 3);
	
	if(temp == 0) return;
	
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x3A, 0x10);
	for(i=0;i<temp;i++){
		TRH_WRITE(0x27, TempBitBuf[i]);
	    TRH_WRITE(0x02, TempDataBuf[i]);
	}
	
	TRH_WRITE(0x01, 0x14);
	
	_delay_ms(4);
	
	TRH_WRITE(0x3A, 0x00);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	//Write Data Check
	TempDataBuf[0] = 0x30;
	TempDataBuf[1] = addr;
	
	temp = MifareCrypto1Function(2, TempDataBuf, TempBitBuf, 2);
	
	if(temp == 0) return;
	
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x3A, 0x10);
	for(i=0;i<temp;i++){
		TRH_WRITE(0x27, TempBitBuf[i]);
	    TRH_WRITE(0x02, TempDataBuf[i]);
	}
	
	TRH_WRITE(0x01, 0x14);
	
	_delay_ms(4);
	
	TRH_WRITE(0x3A, 0x00);
		
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	if(LEN == 18)
	{
		for(i=0;i<18;i++) TempDataBuf[i] = TRH_READ(0x02);
		temp = MifareCrypto1Function(18, TempDataBuf, TempBitBuf, 4);
		
		if(temp != 0)
		{
			check = 0;
			for(i=0;i<16;i++){
				if(txdata[i] == TempDataBuf[i]) check += 0;
				else check += 1;
			}
		}
	}
}
*/
/*
void ISO14443A_UID(void)
{
	u8 temp;	
		
	ISO14443A_REG();
	temp = ISO14443A_REQA();
	if(temp == 0) return;
	temp = ISO14443A_ANTISELECT(0x93);
	if(temp == 0) return;
}
*/
void ISO14443A_Memory(void)
{
	u8 temp;
	//write test
	//u8 xxx[16];
	//for(temp = 0;temp<16;temp++) xxx[temp] = temp+10;
	//
	ISO14443A_REG();
	temp = ISO14443A_REQA();
	if(temp == 0) return;

	temp = ISO14443A_ANTISELECT(0x93);
	if(temp == 0) return;

	//temp = Mifare_Authentication(0, 0);
	temp = Mifare_Authentication(1, Block_Address);
	if(temp == 1)
	{
		//UART_STRING("Card Authentication OK!!\n");
		//Mifare_Read(0);
		//DETECT_14443A = 1;
		Mifare_Read(Block_Address);
		//write test
		//MIFARE_WRITE(8, xxx);
		//Mifare_Read(8);
	}
}

unsigned char ISO14443A_RunFunction(void)
{
  u8 i;
  u8 tmp;
  u8 racos_key[6] = {0x64,0x1A,0x73,0xDE,0x6F,0x7C};

  
  ISO14443A_REG();
  tmp = ISO14443A_REQA();

  if(tmp == 0) return 0;
 
  tmp = ISO14443A_ANTISELECT(0x93);
 
  if(tmp == 0) return 0;
  test[2]=0x02;
  if(tmp == 1) //Mifare Memory Read
  {
     
    for(i=0;i<6;i++) SECTORKEY[i] = racos_key[5-i];//0xFF;
    MIFARE_KEYSEL = 0;
    MIFARE_ADDR = 20;
    
    tmp = MIFARE_READ_RunFunction();
	
	DETECT_14443A_MEM=tmp;
    if(DETECT_14443A_MEM)             // Detect ISO14443A CARD
    {
       
        DETECT_14443A_MEM = 0;
		NEW_CARD=1;
		
		 for(i=0;i<48;i++) MyBuf[i] = CARD_DATA[i];
		 //UART0_TX;
		 //UART0_PUT_STRING(MyBuf, 48);
		 //UART0_RX;
		CheckCardData();
		
    } 
      
    return tmp;
  }
  else if(tmp == 3&&fHandheld==1) //ISO14443 Part -4
  {
  //Timer1_Sec_Start(2);
    tmp = ISO14443A_RATS();
    if(tmp == 0) return 0;
    tmp = ISO14443_RunFunction();
    
    TRH_WRITE(0x11, 0x5B);
    delay_ms(30);
   
	NEW_CARD=0;
	/*
    if(TRH_NFCRxEn)
		{
			
			TRH_NFCRxEn=0;
			NewData0=1;
			for(i=0;i<TRH_NFCRxLen;i++) 
				{
					RxData0[i] = TRH_NFCRxBuf[i];
					TRH_NFCRxBuf[i]=0;
				}
			//for(i=0;i<TRH_NFCRxLen;i++) test[i] = TRH_NFCRxBuf[i];
			TRH_NFCTxLen = 0;
			NEW_CARD=1;
			//test_act=1;
			
		
		}
	*/
    return tmp;
  }
 
  else return 0;
}



