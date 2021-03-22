/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : system.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/11
* Description : 
********************************************************************************
*/
#include "common.h"


void Read_Hotelcode_eeprom(void)
{
    BYTE temp;
  
    temp = Read_Byte_Data(eSETSTART + eHotelCodeSet);
    //Save_Word_Data(eSETSTART + eHOTELCODE, 1888);
    delay_ms(5);    
    if(temp !=1)
    {
        Save_Word_Data(eSETSTART + eHOTELCODE, HOTEL_CODE_SET);
        delay_ms(5);
        Save_Byte_Data(eSETSTART + eHotelCodeSet, 1);
        delay_ms(3);         
    }    
    
    uiHotelCode = Read_Word_Data(eSETSTART + eHOTELCODE);
   if(uiHotelCode==0xffff)
      uiHotelCode=1888;
}

void Read_AreaNumber_eeprom(void)
{
    BYTE temp; 
    
    temp = Read_Byte_Data(eSETSTART + eSpecialArea);
    if(temp == 0xff) Save_Byte_Data(eSETSTART + eSpecialArea, 99);
    else SpAreaNo = temp;
    
    temp = Read_Byte_Data(eSETSTART + eMasterArea);
    if(temp == 0xff) MasterArea = 1;   
    else MasterArea = temp;

    temp = Read_Byte_Data(eSETSTART + eMaidArea);
    if(temp == 0xff) MaidArea = 1;   
    else MaidArea = temp;   
    
    temp = Read_Byte_Data(eSETSTART + eMiniArea);
    if(temp == 0xff) MiniBarArea = 1;   
    else MiniBarArea = temp;     
}

void Read_OverwriteState_eeprom(void)
{
   BYTE tTemp; 
   WORD wTemp;

   tTemp = Read_Byte_Data(eSETSTART + eOVERWRITE);
   if(tTemp > 10)
      Save_Byte_Data(eSETSTART + eOVERWRITE,0);

   wTemp=Read_Word_Data(eSETSTART+eENTERSAVE);
   if(wTemp>500)
      Save_Word_Data(eSETSTART+eENTERSAVE,0);
   
   wTemp=Read_Word_Data(eSETSTART+eENTERSEND);
   if(wTemp>500)
      Save_Word_Data(eSETSTART+eENTERSEND,0);
   
   tTemp=Read_Byte_Data(eSETSTART + eFullWrite);
   if(tTemp>10)
      Save_Byte_Data(eSETSTART + eFullWrite,0);
      
}

void Read_LedTime(void)
{
   BYTE temp;
   float fTemp;

   temp = Read_Byte_Data(eSETSTART + eRelayGreenLED);
   if(temp == 0xff) CallTime = 3;
   else
   {
       CallTime = (BYTE)(fTemp); 
   }

   temp = Read_Byte_Data(eSETSTART + eRedLed);
   if(temp == 0xff) RLedTime = 1;
   else
   {
       RLedTime = (BYTE)(fTemp); 
   }   

   temp = Read_Byte_Data(eSETSTART + eAUTOTIME);
   if(temp == 0xff) FloorTime = 1;
   else
   {
       FloorTime = (BYTE)(fTemp); 
   }   
    
}

void Read_ReaderType(void)
{
    BYTE temp;

    temp = Read_Byte_Data(eSETSTART + eReaderType);
    if(temp == 0xff) 
      Save_Byte_Data(eSETSTART + eReaderType,1); //0:GuestRoom 1:SR
}


void Read_BusAddress(void)
{
   BYTE temp;
   
   temp = Read_Byte_Data(eSETSTART + eBusAddress);
   if(temp==0xff || temp==0x00) BusAddress=1;
   else BusAddress = temp;
}

void Read_BuildingNumber(void)
{
   BYTE temp;
   
   temp = Read_Byte_Data(eSETSTART + eBuildingNumber);
   if(temp==0xff || temp==0x00) BuildNo=1;
   else BuildNo = temp;
}

void Read_CheckoutIime(void)
{
    BYTE temp, i;
    WORD tAddr;
    
    for(i=0; i<8; i++)
    {
        temp = Read_Byte_Data(eSETSTART + eCheckOutTime1 + 2*i);
        if(temp > 24)
        {
            tAddr = eSETSTART + eCheckOutTime1 + 2*i;
            Save_Byte_Data(tAddr, 12 + i);  // hour
            delay_ms(3);
        }
        temp = Read_Byte_Data(eSETSTART + eCheckOutTime1 + 1 + 2*i);
        if(temp > 59)
        {
            tAddr = eSETSTART + eCheckOutTime1 + 2*i + 1;
            Save_Byte_Data(tAddr, 0);       // Minutes
            delay_ms(3);
        }
    }
}

void Init_Set_Data(void)
{
   BYTE tNumber;
   BYTE tTemp; 
    WORD wTemp;
   
       Read_Hotelcode_eeprom();
   Read_OverwriteState_eeprom();
   

       Read_AreaNumber_eeprom();
   
   Read_LedTime();
   Read_ReaderType();
   Read_BusAddress();
   Read_BuildingNumber();
   Read_CheckoutIime();
   
   tNumber = e2prom_read_byte(nFloorNo);
   delay_ms(5);
   if(tNumber == 0xff) FloorNo = 1;
   else FloorNo = tNumber;



   Office=Read_Byte_Data( eSETSTART + officeset); //2020.11.20 김태성 사원 수정
   if(Office==0xFF)
      Office=0;   
   
}


void Init_Reader_Address(void)
{
    BYTE temp;
    temp = PINC;
    temp = (temp & 0xc0) / 64;    // ex) 1100 0000을  0000 0011로 만들기 위해.결과적으로 right로 6칸 shift 시킨 결과 
    ReaderAddr = temp^0x03;   
}



/* //2020.11.20 김태성 사원 수정, 사용 안하는걸로 추정 
void Init_Device_Mode(void)
{
   BYTE temp_dev = 0;

   //temp_dev = DEV_TYPE;
   //if(!temp_dev) DeviceMode = 1; 
   //else DeviceMode = 0;

   
   temp_dev = eeprom_read_byte(DEV_MODE_SET);
   if(temp_dev == 0xff) DeviceMode = 0;

   else DeviceMode = temp_dev;
}
*/


void System_Init(void)
{
   MCU_Init();       // MCU Initiation

   delay_ms(50);
   
   //TRH_INIT();
   Uart0_Init();
   Uart1_Init();
   Timer0_Init();
   Timer1_Init();
   Timer2_Init();
   Rtc_Init();
   Read_Hotelcode_eeprom();
   Read_CheckoutIime();
   delay_ms(10);
   sei();           // Interrupt Enabled
}