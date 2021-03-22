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
* Filename : trh_type.h
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2015/08/21
* Description :
********************************************************************************
*/
#ifndef _TRH_TYPE_H
#define _TRH_TYPE_H


//==============================================================================
//TRH Variable define
//==============================================================================
_GLOBAL_  u8 TRH_LEN;
_GLOBAL_  u8 TRH_ERR;
_GLOBAL_  u8 TRH_INT;
  
_GLOBAL_  u8 TRH_TxLen;
_GLOBAL_  u8 TRH_TxBuf[128];
_GLOBAL_  u8 TRH_RxLen;
_GLOBAL_  u8 TRH_RxBuf[128];

//==============================================================================
//RFID Variable define
//==============================================================================
_GLOBAL_  u8 ATQA[2];
_GLOBAL_  u8 SAK;
_GLOBAL_  iu8 UID[20];
_GLOBAL_  u8 ISO14443_PCB;

//==============================================================================
//Mifare Variable define
//==============================================================================
_GLOBAL_  u8 SECTORKEY[6];
_GLOBAL_  u8 Set_Sectorkey[6];
_GLOBAL_  u8 SECTORKEYBUFF[6];
_GLOBAL_  u8 MIFARE_ADDR;
_GLOBAL_  u8 Mifare_Set_Addr;
_GLOBAL_  u8 MIFARE_KEYSEL;
  
_GLOBAL_  u8 MIFARE_ACK;
_GLOBAL_  u8 MIFARE_RxBuf[48];
_GLOBAL_  u8 MIFARE_TxBuf[48];


_GLOBAL_  iu8 TagRan[4];
//##############################################################################
_GLOBAL_  u8 RandomA[4];//Reader Random number
_GLOBAL_  u8 RandomB[4];//Tag Random number Gen
_GLOBAL_  u8 TransByte[20];
_GLOBAL_  u8 TransParity[20];
_GLOBAL_  u8 keystream[22];

//==============================================================================
//NFC Variable define
//==============================================================================
_GLOBAL_	u8  TRH_NFCTxEn;
_GLOBAL_	u8  TRH_NFCRxEn;

_GLOBAL_	u16 TRH_NFCRxPtr;
_GLOBAL_	u16 TRH_NFCRxLen;
_GLOBAL_	u8  TRH_NFCRxBuf[128];

_GLOBAL_	u16 TRH_NFCTxPtr;
_GLOBAL_	u16 TRH_NFCTxLen;
_GLOBAL_	u8  TRH_NFCTxBuf[128];

#endif
