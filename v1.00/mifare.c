/*
********************************************************************************
* 3ALogics Inc.
* 7th Floor, Hyundai-office Bldg., 9-4, Sunae-dong. Bundang-gu, Seongnam-si,
* Gyeonggi-do, 463-783, Korea
*
* (c) Copyright 2015, 3ALogics, Korea
*
* Hardware : RACOS Sytem RFID Reader
* Complier : IAR AVR
* Filename : mifare.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2015/08/21
* Description :
********************************************************************************
*/
#include "common.h"

u8 MIFARE_Authentication(u8 keksel, u8 addr)
{
  u8 i;
  u8 check;
  //WDT;
  TRH_WRITE(0x22, 0x07);
  TRH_WRITE(0x09, 0x01);
  if(keksel == 0) TRH_WRITE(0x02, 0x60);
  else TRH_WRITE(0x02, 0x61);
  TRH_WRITE(0x02, addr);
  
  TRH_WRITE(0x01, 0x14);
  
  delay_ms(5);
  
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);

  if(TRH_LEN == 4)
  {
  	
    for(i=0;i<4;i++) TagRan[i] = TRH_READ(0x02);
    for(i=0;i<6;i++) SECTORKEYBUFF[i] = SECTORKEY[i];
    
    //Crypto Initial
    for(i=0;i<4;i++) MifareCrypto1Byte((UID[i] ^ TagRan[i]), 1);
    //Random number Gen
    for(i=0;i<4;i++) RandomA[i] = 0x0A+TagRan[i];//Reader random number
    //Tagrandom
    for(i=0;i<4;i++) RandomB[i] = TagRan[i];
    MifarePRNG32(RandomB, 64);
    //##########################################################################
    for(i=0;i<4;i++) TransByte[i] = RandomA[i];
    for(i=0;i<4;i++) TransByte[i+4] = RandomB[i];
    for(i=0;i<8;i++) TransParity[i] = MifareParity(TransByte[i]);
    //##########################################################################
    for(i=0;i<4;i++) keystream[i] = MifareCrypto1Byte(RandomA[i], 1);
    for(i=0;i<4;i++) keystream[i+4] = MifareCrypto1Byte(0, 1);
    keystream[8] = MifareCrypto1Parity();
    //##########################################################################
    //Encryption 
    for(i=0;i<8;i++) TransByte[i] ^= keystream[i];
    for(i=0;i<8;i++) TransParity[i] ^= (keystream[i+1]&0x01);
    
    TRH_WRITE(0x22, 0x03);
    TRH_WRITE(0x09, 0x01);
    TRH_WRITE(0x3A, 0x10);
    for(i=0;i<8;i++)
    {
    TRH_WRITE(0x27, TransParity[i]);
    TRH_WRITE(0x02, TransByte[i]);
    }
    
    TRH_WRITE(0x01, 0x14);
    
    delay_ms(4);
    for(i=0;i<4;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  
  
    TRH_WRITE(0x3A, 0x00);
    TRH_LEN = TRH_READ(0x04);
    TRH_ERR = TRH_READ(0x0A);
    TRH_WRITE(0x01, 0x00);
    TRH_WRITE(0x2E, 0x00);	

	
    if(TRH_LEN == 4)
    {
      for(i=0;i<4;i++) RandomA[i] = TRH_READ(0x02);
      for(i=0;i<4;i++) RandomB[i] = TagRan[i];
  
      for(i=0;i<4;i++) RandomA[i] ^= keystream[i];
      MifarePRNG32(RandomB, 96);
  
      check = 0;
      for(i=0;i<4;i++)
      {
        if(RandomA[i] == RandomB[i]) check += 0;
        else check += 1;
      }
      if(check == 0) return 1;
      else return 0;
    }
    else return 0;
  }
  else return 0;
}


u8 MIFARE_READ(u8 addr)
{
  u8 i;
  u16 crc;
	WDT;
  TransByte[0] = 0x30;
  TransByte[1] = addr;
  crc = MifareCrc(2, TransByte);
  TransByte[2] = (u8)crc;
  TransByte[3] = (u8)(crc>>8);
  
  for(i=0;i<4;i++) TransParity[i] = MifareParity(TransByte[i]);
  
  for(i=0;i<4;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  keystream[4] = MifareCrypto1Parity();
  
  for(i=0;i<4;i++) TransByte[i] ^= keystream[i];
  for(i=0;i<4;i++) TransParity[i] ^= (keystream[i+1]&0x01);
  
  TRH_WRITE(0x22, 0x03);
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x3A, 0x10);
  for(i=0;i<4;i++)
  {
    TRH_WRITE(0x27, TransParity[i]);
    TRH_WRITE(0x02, TransByte[i]);
  }
  
  TRH_WRITE(0x01, 0x14);
  delay_ms(20);
  for(i=0;i<18;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  TRH_WRITE(0x3A, 0x00);
  
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);	

  if(TRH_LEN == 18)
  {
    for(i=0;i<18;i++) TransByte[i] = TRH_READ(0x02);
    for(i=0;i<18;i++) TransByte[i] ^= keystream[i];
    crc = MifareCrc(18, TransByte);
    if(crc == 0)
    {
      for(i=0;i<16;i++) MIFARE_RxBuf[i] = TransByte[i]; 
      return 1;
    }
    else return 0;
  }
  else return 0;
}


u8 MIFARE_WRITE(u8 addr, u8 *data)
{
  u8 i;
  u16 crc;
  u8 keyack;
  
  TransByte[0] = 0xA0;
  TransByte[1] = addr;
  crc = MifareCrc(2, TransByte);
  TransByte[2] = (u8)crc;
  TransByte[3] = (u8)(crc>>8);
  
  for(i=0;i<4;i++) TransParity[i] = MifareParity(TransByte[i]);
  
  for(i=0;i<4;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  keystream[4] = MifareCrypto1Nibble(0, 1);
  keyack = keystream[4];
  
  for(i=0;i<4;i++) TransByte[i] ^= keystream[i];
  for(i=0;i<4;i++) TransParity[i] ^= (keystream[i+1]&0x01);
  
  TRH_WRITE(0x22, 0x03);
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x3A, 0x10);
  for(i=0;i<4;i++)
  {
    TRH_WRITE(0x27, TransParity[i]);
    TRH_WRITE(0x02, TransByte[i]);
  }
  
  TRH_WRITE(0x0F,0x40);
  TRH_WRITE(0x01, 0x14);
  
  delay_ms(2);
  
  TRH_WRITE(0x0F,0x00);
  TRH_WRITE(0x3A, 0x00);
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);	
  
  
  MIFARE_ACK = TRH_READ(0x02);
  MIFARE_ACK >>= 4;
  MIFARE_ACK ^= keyack;
  
  if(MIFARE_ACK != 0x0A) return 0;
  
  //########################################################
  for(i=0;i<18;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  keystream[18] = MifareCrypto1Nibble(0,1);
  keyack = keystream[18];
  //########################################################
  for(i=0;i<16;i++) TransByte[i] = data[i];
  
  crc = MifareCrc(16, TransByte);
  TransByte[16] = (u8)crc;
  TransByte[17] = (u8)(crc>>8);
  for(i=0;i<18;i++) TransParity[i] = MifareParity(TransByte[i]);
  
  
  for(i=0;i<18;i++) TransByte[i] ^= keystream[i];
  for(i=0;i<18;i++) TransParity[i] ^= (keystream[i+1]&0x01);
  
  TRH_WRITE(0x22, 0x03);
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x3A, 0x10);
  for(i=0;i<18;i++)
  {
    TRH_WRITE(0x27, TransParity[i]);
    TRH_WRITE(0x02, TransByte[i]);
  }
  
  TRH_WRITE(0x01, 0x14);
  
  //########################################################
  for(i=0;i<22;i++) keystream[i] = MifareCrypto1Byte(0, 1);
  //########################################################
  TRH_WRITE(0x3A, 0x00);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);	
  //########################################################
  
  TransByte[0] = 0x30;
  TransByte[1] = addr;
  crc = MifareCrc(2, TransByte);
  TransByte[2] = (u8)crc;
  TransByte[3] = (u8)(crc>>8);
  
  for(i=0;i<4;i++) TransParity[i] = MifareParity(TransByte[i]);
  
  for(i=0;i<4;i++) TransByte[i] ^= keystream[i];
  for(i=0;i<4;i++) TransParity[i] ^= (keystream[i+1]&0x01);
  
  TRH_WRITE(0x22, 0x03);
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x3A, 0x10);
  for(i=0;i<4;i++)
  {
    TRH_WRITE(0x27, TransParity[i]);
    TRH_WRITE(0x02, TransByte[i]);
  }
  
  TRH_WRITE(0x01, 0x14);
  delay_ms(3);
  TRH_WRITE(0x3A, 0x00);
  
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);		
  if(TRH_LEN == 18)
  {
    for(i=0;i<18;i++) TransByte[i] = TRH_READ(0x02);
    for(i=0;i<18;i++) TransByte[i] ^= keystream[i+4];
    crc = MifareCrc(18, TransByte);
    if(crc == 0)
    {
      for(i=0;i<16;i++) MIFARE_RxBuf[i] = TransByte[i]; 
      return 1;
    }
    else return 10;
  }
  else return 10;
}


u8 CheckPreCode(void)
{
	u8 i;
	u8 tmp;
	BYTE PreCodingData[16] = {0x02, 0x22 , 0xD4, 0x88, 0x56, 0x78, 0xB5, 0x9F, 
						      0x18, 0xEE,  0x79, 0xA8, 0x03, 0x25 ,0x79, 0xF2};
	
	tmp = MIFARE_READ(MIFARE_ADDR+2);
	
	if(tmp == 0)
	{
		
		return 0;
	}
	for(i=0;i<16;i++)
	{
		if(MIFARE_RxBuf[i] !=PreCodingData[i]) return 0;
	}
	return 1;
}

u8 MIFARE_READ_RunFunction(void)
{
  u8 i;
  u8 tmp;
  
  //#############################################
  tmp = MIFARE_Authentication((u8)MIFARE_KEYSEL, (u8)MIFARE_ADDR);

  if(tmp == 0)
  {
  	
    
    return 0;
  }
  	//#############################################
  	/*
	tmp = CheckPreCode();

	if(tmp == 0)
	{
		#if mifare_debug
		UART_STRING("\r\nMifare Read Fail!!");
		#endif
		return 0;
	}
	*/
  //#############################################
 
  tmp = MIFARE_READ(MIFARE_ADDR);
  if(tmp == 0)
  {
    
    return 0;
  }
  for(i=0;i<16;i++) CARD_DATA[i] = MIFARE_RxBuf[i];
  
   //#############################################
 
  tmp = MIFARE_READ(MIFARE_ADDR+1);
  if(tmp == 0)
  {
   
    return 0;
  }
  for(i=0;i<16;i++) CARD_DATA[16+i] = MIFARE_RxBuf[i];
  
   //#############################################
  
  tmp = MIFARE_READ(MIFARE_ADDR+2);
  if(tmp == 0)
  {
    
    return 0;
  }
  for(i=0;i<16;i++) CARD_DATA[32+i] = MIFARE_RxBuf[i];
  

  TRH_WRITE(0x11, 0x58);
  
  //#############################################
  
  //#############################################
  return 1;
}


u8 MIFARE_WRITE_RunFunction(void)
{
  //u8 i;
  u8 tmp;
  
  //#############################################
   tmp = MIFARE_Authentication(MIFARE_KEYSEL, MIFARE_ADDR);
  if(tmp == 0)
  {
    
    return 0;
  }
  	//#############################################
	tmp = CheckPreCode();

	if(tmp == 0)
	{
		
		return 0;
	}
  //#############################################
  tmp = MIFARE_WRITE(MIFARE_ADDR, (unsigned char *)MIFARE_TxBuf);
  if(tmp == 0)
  {
    
    return 0;
  }
  else if(tmp == 1)
  {
    //#############################################
    
    //#############################################
    return 1;
  }
  else
  {
    
    return 10;
  }
}
