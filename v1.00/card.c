

#include "common.h"


WORD EnterSaveCnt, pEnterSave;
BYTE ResetState, OverwriteState;
volatile BYTE fAuthCard, fSuccess;
volatile BYTE BuildNo, AreaNo, FloorNo;
volatile WORD uiHotelCode;
volatile BYTE DateBuf[10]; 
volatile BYTE BuildBuf[3];
volatile BYTE PublicBuf[3];
BYTE SaveBuf[32];
volatile BYTE nCardResult;
volatile BYTE nErrorCode;
volatile WORD pEnterSend; 
BYTE OverwriteState;
WORD EnterSaveCnt, pEnterSave;
BYTE ResetState, OverwriteState;
BYTE SaveBuf[32];
BYTE ReaderBuf0[30];
BYTE ReaderBuf1[30];
BYTE ReaderBuf2[30];
BYTE SubBuf0[20], SubBuf1[20], SubBuf2[20];
BYTE ReaderAck0, ReaderAck1, ReaderAck2, ReaderAck3;

volatile WORD uiHotelCode;
volatile BYTE nCardResult;
volatile BYTE nErrorCode;
volatile BYTE MasterArea, MaidArea, MiniBarArea;
//volatile BYTE SpcArea0[5], SpcArea1[5];
//volatile BYTE fReader0, fReader1, fReader2;
//volatile BYTE ContactTime0, ContactTime1, ContactTime2, ContactTime3;
volatile BYTE RoomBuffer0, RoomBuffer1, RoomBuffer2;

void RelayOut(void)
{
	RELAY = 0;
	delay_ms(100);
	RELAY = 1;
}

void RelayOn(void)
{
	//PORTB=PORTB&0xF7;
	if(Office)
	{
		if(RELAY ==0)RELAY = 1;
		else RELAY = 0;
	}
	else RELAY = 0;
}


void RelayOff(void)
{
	//PORTB=PORTB|0x08;
	RELAY = 1;
}

void CardSuccess(void)
{
	BYTE tmp=0;

	CallTime=Read_Byte_Data(eSETSTART + eRelayGreenLED);
	delay_ms(5);
	
	if(CallTime==0xff)
		CallTime=3;
	if(CallTime<1)
		CallTime=3;
	tmp=CallTime;

	
	Timer1_Sec_Stop();
	T1IRQ = 0;
	Timer1_Sec_Start(tmp);		//4초 timer 

	
	Blue_LED_On();

	NEW_CARD = 1;
	
	Buzzer_On();
	delay_ms(50);
	Buzzer_Off();

	//RelayOn();
	
	//delay_ms(100);
}

void CardFailed(void)
{
	BYTE tmp=0;

	RLedTime=Read_Byte_Data(eSETSTART + eRedLed);
	if(RLedTime==0xff)
		RLedTime=2;
	delay_ms(5);
	tmp=RLedTime;
	if(RLedTime==0)
		tmp=2;

	Timer1_Sec_Stop();
	T1IRQ = 0;
    Timer1_Sec_Start(tmp);		// 1초 timer 

	
	Red_LED_On();

	NEW_CARD = 1;
	Buzzer_On();
	delay_ms(50);
	Buzzer_Off();
	//delay_ms(200);
	//All_LED_On();
}

void SaveDataCount(void)
{
   // BYTE i;
    WORD ckSaveAddr, tAddr;
    WDT;
    if(pEnterSave > 400) 
    {
        OverwriteState = OverwriteState+1;
	if(OverwriteState>5)
		OverwriteState=5;
        pEnterSave = 1; 
        
        tAddr = eSETSTART + eOVERWRITE;
        Save_Byte_Data(tAddr, OverwriteState); 
        delay_ms(6);
        tAddr = eSETSTART + eFullWrite;
        Save_Byte_Data(tAddr, 1);
    }

        ckSaveAddr = pEnterSave * 32;
        Save_Page_Data(ckSaveAddr, 30, &SaveBuf[0]);
		pEnterSave++;	
        delay_ms(10);
       
        //pEnterSave = 1;
        //UART_STRING("\r\n Save Data = ");for(i=0;i<30;i++)UART_H2A(SaveBuf[i]);
        
  Save_Word_Data(eSETSTART+eENTERSAVE, pEnterSave);
  
}  

void SaveGuestCardData(void)  // Guest, Precheck, Onetime, Standby
{
    //BYTE temp;
    Rtc_Read_Data();
    WDT;    
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    //temp = rtc_year;
    //if((temp >= 12) && (temp < 28)) CardData[9] = CardData[9] + 12;  // 2012 ~ 2027
    //else if((temp >= 28) && (temp < 44)) CardData[9] = CardData[9] + 28;  // 2028 ~ 2043     
  
    SaveBuf[0] = MyBuf[0];         // Card Class
    SaveBuf[1] = MyBuf[1];         // Hotel Code High Byte
    SaveBuf[2] = MyBuf[2];         // Hotel Code Low Byte    
    SaveBuf[3] = MyBuf[3];         // Room Number High Byte
    SaveBuf[4] = MyBuf[4];         // Room Number Low Byte
    
    SaveBuf[5] = MyBuf[3];         // Staff Number High Byte
    SaveBuf[6] = MyBuf[4];         // Staff Number Low Byte    
    SaveBuf[7] = MyBuf[5];         // Security Number High Byte
    SaveBuf[8] = MyBuf[6];         // Security Number Low Byte
    //SaveBuf[11] = MyBuf[7];         // Invalid Year
    //SaveBuf[12] = MyBuf[8];         // Invalid Month
    //SaveBuf[13] = MyBuf[9];         // Invalid Date
    SaveBuf[9] = MyBuf[7];         // Invalid Year
    SaveBuf[10] = MyBuf[8];        // Invalid Month
    SaveBuf[11] = MyBuf[9];        // Invalid Date    
    SaveBuf[12] = MyBuf[10];       // Index 
    
    SaveBuf[13] = 0;                  // Time Code
    SaveBuf[14] = 0;                  // Staff Area
    SaveBuf[15] = MyBuf[13];       // Speacial Area Byte 1
    SaveBuf[16] = MyBuf[14];       // Speacial Area Byte 2
    SaveBuf[17] = MyBuf[15];       // Speacial Area Byte 3 
    SaveBuf[18] = MyBuf[16];       // Speacial Area Byte 4
    SaveBuf[19] = MyBuf[17];       // Speacial Area Byte 5
    SaveBuf[20] = MyBuf[18];       // Checkout Time

    Rtc_Read_Data();
    SaveBuf[21] = rtc_year;     // RTC Year
    SaveBuf[22] = rtc_month;    // RTC Month 
    SaveBuf[23] = rtc_date;     // RTC Date
    SaveBuf[24] = rtc_hour;     // RTC Hour
    SaveBuf[25] = rtc_min;      // RTC Minute
    SaveBuf[26] = rtc_sec;      // RTC Second 
    
    SaveBuf[27] = nErrorCode;       // Error Code
    SaveBuf[28] = 0;                // Reserved
    SaveBuf[29] = 0;                // Reserved
    
    SaveDataCount();


}

void SaveStaffCardData(void)  // Emergency, Grand Master, Master, Maid, Mini Bar
{
 //   BYTE temp;
    Rtc_Read_Data();  
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    //temp = rtc_year;
    //if((temp >= 12) && (temp < 28)) CardData[9] = CardData[9] + 12;  // 2012 ~ 2027
    //else if((temp >= 28) && (temp < 44)) CardData[9] = CardData[9] + 28;  // 2028 ~ 2043        
    
    SaveBuf[0] = MyBuf[0];         // Card Class
    SaveBuf[1] = MyBuf[1];         // Hotel Code High Byte
    SaveBuf[2] = MyBuf[2];         // Hotel Code Low Byte    
    SaveBuf[3] = 0;                   // Room Number High Byte
    SaveBuf[4] = 0;                   // Room Number Low Byte
    
    SaveBuf[5] = MyBuf[3];         // Staff Number High Byte
    SaveBuf[6] = MyBuf[4];         // Staff Number Low Byte    
    SaveBuf[7] = MyBuf[5];         // Security Number High Byte
    SaveBuf[8] = MyBuf[6];         // Security Number Low Byte
    //SaveBuf[11] = MyBuf[7];         // Invalid Year
    //SaveBuf[12] = MyBuf[8];         // Invalid Month
    //SaveBuf[13] = MyBuf[9];         // Invalid Date
    SaveBuf[9] = MyBuf[7];         // Invalid Year
    SaveBuf[10] = MyBuf[8];        // Invalid Month
    SaveBuf[11] = MyBuf[9];        // Invalid Date    
    SaveBuf[12] = MyBuf[10];       // Index 
    
    SaveBuf[13] = MyBuf[11];                  // Time Code
    SaveBuf[14] = MyBuf[12];                  // Staff Area
    SaveBuf[15] = MyBuf[13];       // Speacial Area Byte 1
    SaveBuf[16] = MyBuf[14];       // Speacial Area Byte 2
    SaveBuf[17] = MyBuf[15];       // Speacial Area Byte 3 
    SaveBuf[18] = MyBuf[16];       // Speacial Area Byte 4
    SaveBuf[19] = MyBuf[17];       // Speacial Area Byte 5
    SaveBuf[20] = 0;                  // Checkout Time

    Rtc_Read_Data();
    SaveBuf[21] = rtc_year;     // RTC Year
    SaveBuf[22] = rtc_month;    // RTC Month 
    SaveBuf[23] = rtc_date;     // RTC Date
    SaveBuf[24] = rtc_hour;     // RTC Hour
    SaveBuf[25] = rtc_min;      // RTC Minute
    SaveBuf[26] = rtc_sec;      // RTC Second 
    
    SaveBuf[27] = nErrorCode;       // Error Code
    SaveBuf[28] = 0;                // Reserved
    SaveBuf[29] = 0;                // Reserved    
           
    SaveDataCount();
}

void SaveSetCardData(void)  // Emergency, Grand Master, Master, Maid, Mini Bar
{
 //   BYTE temp;
    Rtc_Read_Data();
        
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    //temp = rtc_year;
    //if((temp >= 12) && (temp < 28)) CardData[7] = CardData[7] + 12;  // 2012 ~ 2027
    //else if((temp >= 28) && (temp < 44)) CardData[7] = CardData[7] + 28;  // 2028 ~ 2043        
    
    SaveBuf[0] = MyBuf[0];         // Card Class
    SaveBuf[1] = MyBuf[1];         // Hotel Code High Byte
    SaveBuf[2] = MyBuf[2];         // Hotel Code Low Byte    
    SaveBuf[3] = 0;                   // Room Number High Byte
    SaveBuf[4] = 0;                   // Room Number Low Byte
    
    SaveBuf[5] = MyBuf[3];         // Staff Number High Byte
    SaveBuf[6] = MyBuf[4];         // Staff Number Low Byte    
    SaveBuf[7] = MyBuf[5];         // Security Number High Byte
    SaveBuf[8] = MyBuf[6];         // Security Number Low Byte
    //SaveBuf[11] = MyBuf[7];         // Invalid Year
    //SaveBuf[12] = MyBuf[8];         // Invalid Month
    //SaveBuf[13] = MyBuf[9];         // Invalid Date
    SaveBuf[9] = MyBuf[7];         // Invalid Year
    SaveBuf[10] = MyBuf[8];        // Invalid Month
    SaveBuf[11] = MyBuf[9];        // Invalid Date    
    SaveBuf[12] = MyBuf[10];       // Index 
    
    SaveBuf[13] = MyBuf[11];       // Time Code
    SaveBuf[14] = MyBuf[12];       // Staff Area
    SaveBuf[15] = MyBuf[13];       // Speacial Area Byte 1
    SaveBuf[16] = MyBuf[14];       // Speacial Area Byte 2
    SaveBuf[17] = MyBuf[15];       // Speacial Area Byte 3 
    SaveBuf[18] = MyBuf[16];       // Speacial Area Byte 4
    SaveBuf[19] = MyBuf[17];       // Speacial Area Byte 5
    SaveBuf[20] = 0;                  // Checkout Time

    Rtc_Read_Data();
    SaveBuf[21] = rtc_year;     // RTC Year
    SaveBuf[22] = rtc_month;    // RTC Month 
    SaveBuf[23] = rtc_date;     // RTC Date
    SaveBuf[24] = rtc_hour;     // RTC Hour
    SaveBuf[25] = rtc_min;      // RTC Minute
    SaveBuf[26] = rtc_sec;      // RTC Second 
    
    SaveBuf[27] = nErrorCode;       // Error Code
    SaveBuf[28] = 0;                // Reserved
    SaveBuf[29] = 0;                // Reserved   
    
    SaveDataCount();
}

BYTE GetHotelCode(void)
{
    BYTE RcvChk = 0;
    WORD tHotelCode;

    tHotelCode = MyBuf[1];
    tHotelCode = (tHotelCode << 8);
    tHotelCode |= MyBuf[2];    
	uiHotelCode = Read_Word_Data(eSETSTART + eHOTELCODE);
    if(tHotelCode == uiHotelCode) RcvChk = 1;
    else RcvChk = 0;
   // test[41] = (BYTE)(tHotelCode >> 8);   // Hign Byte
    //test[42] = (BYTE)tHotelCode;         // Low Byte
    //test[43] = (BYTE)(uiHotelCode >> 8);   // Hign Byte
    //test[44] = (BYTE)uiHotelCode;         // Low Byte
    /*
    tx_data0[0] = 0x7B;
    tx_data0[1] = (BYTE)(tHotelCode >> 8);   // Hign Byte
    tx_data0[2] = (BYTE)tHotelCode;         // Low Byte
    tx_data0[3] = (BYTE)(uiHotelCode >> 8);   // Hign Byte
    tx_data0[4] = (BYTE)uiHotelCode;         // Low Byte
    tx_data0[5] = RcvChk;
    tx_data0[6] = 0x7D;     
    delay_ms(2);
    UART0_TX_EN;    
    UART0_PUT_STRING(tx_data0, 7); 
    delay_ms(2);
    UART0_RX_EN; 
    */      
    return RcvChk;    
}  

/*
BYTE GetCardChkOutTime(void)
{
    BYTE tTimeCode, tHour, tMinute, ckReturn;
    
    tTimeCode =  RxData1[22];
    tHour = eeprom_read_byte(nCheckHour0 + tTimeCode);
    tMinute = eeprom_read_byte(nCheckMin0 + tTimeCode);
    
    if(tHour > rtc_hour) ckReturn = 1;
    else if(tHour < rtc_hour) ckReturn = 0;
    else
    {
       if(tMinute >= rtc_min) ckReturn = 1;
       else ckReturn = 0;
    }
    
    return ckReturn;
}  
*/
BYTE GetCardDate(void)
{
    signed int RcvChk = 0;
    BYTE tYear, tMonth, tDate, dYear;
    dYear = 0;
    
    Rtc_Read_Data();
    tDate = MyBuf[9];
    tMonth = MyBuf[8];
    tYear = MyBuf[7];
        
    
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    dYear = rtc_year;
    if(tYear > dYear)
    {/*
        if(dYear == 0) // Door는 First Year
        {
		RcvChk = 0; // Card는 Last Year는 차단
        }   
        else  RcvChk = 1;  // Valid Card     */
	if(tYear - dYear >= 90) RcvChk = 0;
	else RcvChk = 1;
    }
    
    // 16년 주기의 마지막해에 유효기간을 다음 주기의 첫해로 발급할 경우
    else if(tYear < dYear)    // Valid Year < RTC Year
    {
		/*
        if(dYear == 99)  // Last Year 문제
        {
            if(tYear == 0)  RcvChk = 1; // Card는 First Year
            else RcvChk = 0;
        }
        else  RcvChk = 0;*/
        if(dYear - tYear >=90) RcvChk = 1;
	else RcvChk = 0;
    }
    else  // Card Year = Door Year
    {
        if(tMonth > rtc_month)  RcvChk = 1;
        else if(tMonth < rtc_month)  RcvChk = 0;
        else  // Card Month = RTC Month
        {
            if(tDate >= rtc_date)
            {
                if(tDate == rtc_date) RcvChk = 2;
                else RcvChk = 1;
            }
            else if(tDate < rtc_date)  RcvChk = 0;
        }  
    }  
    /*
    tx_data0[0] = 0x7B;
    tx_data0[1] = tYear; 
    tx_data0[2] = tMonth;
    tx_data0[3] = tDate;
    tx_data0[4] = dYear; 
    tx_data0[5] = rtc_year;
    tx_data0[6] = rtc_month;    
    tx_data0[7] = RcvChk;
    tx_data0[8] = 0x7D;     
    
    delay_ms(2);
    UART0_TX_EN;    
    UART0_PUT_STRING(tx_data0, 9); 
    delay_ms(6);
    UART0_RX_EN; 
    */


	
    return RcvChk;
}  

BYTE GetSpecialArea(void) 
{
    BYTE i, j, tArea, tMask, tCount;
    BYTE RcvChk = 0;
    SpAreaNo = Read_Byte_Data(eSETSTART + eSpecialArea);
	if(SpAreaNo==0xff)
		SpAreaNo=99;
	SpAreaNo=SpAreaNo+1;
    if(SpAreaNo == 100) RcvChk = 1;
    else
    {  
        tCount = 1;
        for(i=0; i<5; i++)
        {  
            tMask = 0x80;
            tArea = MyBuf[13 + i];
            //tCount = 1;
            for(j=0; j<8; j++)
            {  
                if(tArea & tMask)
                {  
                    if(tCount == SpAreaNo)
                    {
                        RcvChk = 1;
                        i = 10;
                        j = 10;
                        break;
                    } 
                }
                tMask = tMask >> 1;
                tCount++;
            }
   
        }
    }
    //if(AREA_SET == DISABLED) RcvChk = 1;  
   
    return RcvChk;     
}

BYTE GetPrecheckSpecialArea(void) 
{
    BYTE i, j, tArea, tMask, tCount;
    BYTE RcvChk = 0;
    SpAreaNo = Read_Byte_Data(eSETSTART + eSpecialArea);
	if(SpAreaNo==0xff)
		SpAreaNo=99;
	SpAreaNo=SpAreaNo+1;
    if(SpAreaNo == 100) RcvChk = 1;
    else
    {  
        tCount = 1;
        for(i=0; i<5; i++)
        {  
            tMask = 0x80;
            tArea = MyBuf[15 + i];
            //tCount = 1;
            for(j=0; j<8; j++)
            {  
                if(tArea & tMask)
                {  
                    if(tCount == SpAreaNo)
                    {
                        RcvChk = 1;
                        i = 10;
                        j = 10;
                        break;
                    } 
                }
                tMask = tMask >> 1;
                tCount++;
            }
   
        }
    }
    //if(AREA_SET == DISABLED) RcvChk = 1;  
   
    return RcvChk;     
}

BYTE GetStaffArea(void)
{
    BYTE tClass, tArea, RcvChk;
    
    tClass = MyBuf[0];    
    if(tClass == MASTER_CARD) tArea = MasterArea;
    else if(tClass == MAID_CARD) tArea = MaidArea; 
    else if(tClass == MINIBAR_CARD) tArea = MiniBarArea;
    
    if(tClass == tArea) RcvChk = 1;
    else RcvChk = 0;  
    
    return RcvChk;
}  

BYTE GetSystemSecurityNumber(void)
{
    BYTE RcvChk = 0;
    //WORD tSecurity;
	WORD SCMin,SCMax;
	SCMin=uiSecurity;
	SCMax=uiSecurity+9;
	if(SCMin>511) SCMin=SCMin-512;
	if(SCMax>511) SCMax=SCMax-512;
    cSecurity = MyBuf[5];
    cSecurity = (cSecurity << 8);
    cSecurity |= MyBuf[6];

	if(SCMin<SCMax)
		{
			if(SCMin<=cSecurity&&cSecurity<=SCMax)
				RcvChk=1;
			else
				RcvChk=0;
		}
	else
		{
			if(SCMin<=cSecurity||cSecurity<=SCMax)
				RcvChk=1;
			else
				RcvChk=0;
		}
	
    /*
    if(uiSecurity <= cSecurity)     // Greater than reader security
    {
	if((uiSecurity + 9) < cSecurity) RcvChk = 0;
	else
        {
            if(uiSecurity == cSecurity) RcvChk = 1;
            else RcvChk = 2;
        }
    }
    else
    {
        if(uiSecurity == 511)
        {
            if(cSecurity == 0) RcvChk = 2;
            else if(cSecurity >= 1  && cSecurity <= 9) RcvChk = 2;
            else RcvChk = 0;
        }
        else RcvChk = 0;
    }
	*/
	if (uiSecurity == 0xffff) RcvChk = 1;
   	if(cSecurity>511) RcvChk = 0;     
   	if(RcvChk)
   	{
    	Save_Word_Data(eSETSTART + eSystemSecurity, cSecurity);
   	}
	
    return RcvChk;
}

BYTE GetEmergencySecurityNumber(void)
{
    BYTE RcvChk = 0;
    //WORD tSecurity;
	WORD SCMin,SCMax;
	    SCMin=uiSecurity;
	    SCMax=uiSecurity+9;
	    if(SCMin>511)
			SCMin=SCMin-512;
	    if(SCMax>511)
			SCMax=SCMax-512;
    cSecurity = MyBuf[5];
    cSecurity = (cSecurity << 8);
    cSecurity |= MyBuf[6];
	if(SCMin<SCMax)
		{
			if(SCMin<=cSecurity&&cSecurity<=SCMax)
				RcvChk=1;
			else
				RcvChk=0;
		}
	else
		{
			if(SCMin<=cSecurity||cSecurity<=SCMax)
				RcvChk=1;
			else
				RcvChk=0;
		}
/*
    if(uiSecurity <= cSecurity)     // Greater than reader security
    {
	if((uiSecurity + 9) < cSecurity) RcvChk = 0;
	else RcvChk = 1;
    }
    else
    {
        if(uiSecurity == 511)
        {
            if(cSecurity == 0) RcvChk = 1;
            else if(cSecurity >= 1  && cSecurity <= 9) RcvChk = 1;
            else RcvChk = 0;
        }
        else RcvChk = 0;
    }
    */
    if (uiSecurity == 0xffff) RcvChk = 1;
    if(cSecurity>511)RcvChk = 0;
  
    return RcvChk;
}

BYTE GetCardChkOutTime(void)
{
    BYTE tTimeCode, tHour, tMinute, ckReturn;

    if(MyBuf[0] == PRECHECK_CARD) tTimeCode = MyBuf[20];
    else tTimeCode =  MyBuf[18];
    //tHour = eeprom_read_byte(nCheckHour0 + tTimeCode);
    tHour = Read_Byte_Data(eSETSTART + eCheckOutTime1 + 2*tTimeCode);
    //tMinute = eeprom_read_byte(nCheckMin0 + tTimeCode);
    tMinute = Read_Byte_Data(eSETSTART + eCheckOutTime1 + 2*tTimeCode + 1);
    
    if(tHour > rtc_hour) ckReturn = 1;
    else if(tHour < rtc_hour) ckReturn = 0;
    else
    {
       if(tMinute > rtc_min) ckReturn = 1;
       else ckReturn = 0;
    }

    if(tHour == 0xff | tMinute == 0xff) ckReturn = 0;
    return ckReturn;
} 


void GetGuestCard(void)
{
    BYTE i=0;
    BYTE ckResult = 0, ckHotelCode, ckArea;
    //BYTE i;
    BYTE ckDate = 0, ckTime =0;
    
	ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {     
		nCardResult = TRUE;
        ckResult = ACK; 
        ckDate = GetCardDate();

	if(ckDate == 1)
	{
	    nCardResult = TRUE;
	    ckResult = ACK;      
	    ckArea = GetSpecialArea();

	    if(ckArea == 1) 
	    {
	        nCardResult = TRUE;
	        ckResult = ACK;
	    }
	    else
	    {
	        nCardResult = FALSE;
	        nErrorCode = nEr_specialArea;
	        ckResult = NAK;
	    }  
	}
	else if(ckDate == 2)
	{
		nCardResult = TRUE;
		ckResult = ACK;
		ckTime = GetCardChkOutTime();
		if(ckTime == 1)
		{
			nCardResult = TRUE;
			ckResult = ACK; 
			ckArea = GetSpecialArea();

			if(ckArea == 1) 
			{
				nCardResult = TRUE;
				ckResult = ACK;
			}
			else
			{
				nCardResult = FALSE;
				nErrorCode = nEr_specialArea;
				ckResult = NAK;
			}  
		}
		else
		{
		    nCardResult = FALSE;
		    nErrorCode = nEr_ChkOutTime;
		    ckResult = NAK;                      
		}                  
	}	
        else
        {
            nCardResult = FALSE;
            nErrorCode = nEr_ValidDate;
            ckResult = NAK;
        }
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }    
    
    SaveGuestCardData();
    
    
    if(ckResult)
    {
	
    	//Timer1_Sec_Start(4);
	//for(i=0;i<40;i++) test[i]=0x29;
	CardSuccess();
	RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
    	
		for(i=0;i<40;i++) test[i]=nErrorCode;
	
	CardFailed();	
    }	
	//nErrorCode = 0;
}

void GetPrecheckCard(void)
{
    BYTE i=0;
    BYTE ckResult = 0, ckHotelCode, ckArea;
    //BYTE i;
    BYTE ckDate = 0, ckTime =0;
    
	ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {     
		nCardResult = TRUE;
        ckResult = ACK; 
        ckDate = GetCardDate();

	if(ckDate == 1)
	{
	    nCardResult = TRUE;
	    ckResult = ACK;      
	    ckArea = GetSpecialArea();

	    if(ckArea == 1) 
	    {
	        nCardResult = TRUE;
	        ckResult = ACK;
	    }
	    else
	    {
	        nCardResult = FALSE;
	        nErrorCode = nEr_specialArea;
	        ckResult = NAK;
	    }  
	}
	else if(ckDate == 2)
	{
		nCardResult = TRUE;
		ckResult = ACK;
		ckTime = GetCardChkOutTime();
		if(ckTime == 1)
		{
			nCardResult = TRUE;
			ckResult = ACK; 
			ckArea = GetSpecialArea();

			if(ckArea == 1) 
			{
				nCardResult = TRUE;
				ckResult = ACK;
			}
			else
			{
				nCardResult = FALSE;
				nErrorCode = nEr_specialArea;
				ckResult = NAK;
			}  
		}
		else
		{
		    nCardResult = FALSE;
		    nErrorCode = nEr_ChkOutTime;
		    ckResult = NAK;                      
		}                  
	}	
        else
        {
            nCardResult = FALSE;
            nErrorCode = nEr_ValidDate;
            ckResult = NAK;
        }
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }    
    
    SaveGuestCardData();
    
    
    if(ckResult)
    {
	
    	//Timer1_Sec_Start(4);
	//for(i=0;i<40;i++) test[i]=0x29;
	CardSuccess();
	RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
    	
		for(i=0;i<40;i++) test[i]=nErrorCode;
	
	CardFailed();	
    }	
	//nErrorCode = 0;
}


void GetStandbyCard(void)
{
    BYTE i=0;
    BYTE ckResult = 0, ckHotelCode, ckArea;
    //BYTE i;
    //BYTE ckDate = 0, ckTime =0;
    
	ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {     
		nCardResult = TRUE;
        ckResult = ACK; 

   
	    ckArea = GetSpecialArea();

	    if(ckArea == 1) 
	    {
	        nCardResult = TRUE;
	        ckResult = ACK;
	    }
	    else
	    {
	        nCardResult = FALSE;
	        nErrorCode = nEr_specialArea;
	        ckResult = NAK;
	    }  

    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }    
    
    SaveGuestCardData();
    
    
    if(ckResult)
    {
	
    	//Timer1_Sec_Start(4);
	//for(i=0;i<40;i++) test[i]=0x29;
	CardSuccess();
	RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
    	
		for(i=0;i<40;i++) test[i]=nErrorCode;
	
	CardFailed();	
    }	
	//nErrorCode = 0;
}

void GetStaffCard(void)
{
    BYTE ckResult = 0, ckHotelCode;
    BYTE ckDate = 0, ckArea;
    
    ckHotelCode = GetHotelCode();  
    
    if(ckHotelCode)
    {         
        ckDate = GetCardDate();
        
        if(ckDate == 1 || ckDate == 2)
        {
            nCardResult = TRUE;
            ckResult = ACK; 
            /*
            ckArea = GetStaffArea();
            if(ckArea == 1) 
            {
                nCardResult = TRUE;
                ckResult = ACK;
            }
            else
            {
                nCardResult = FALSE;
                nErrorCode = nEr_StaffArea;
                ckResult = NAK;
            }
            */
	     ckArea = GetSpecialArea();
            if(ckArea == 1) 
            {
                nCardResult = TRUE;
                ckResult = ACK;
            }
            else
            {
                nCardResult = FALSE;
                nErrorCode = nEr_specialArea;
                ckResult = NAK;
            }  
        }
        else
        {
            nCardResult = FALSE;
            nErrorCode = nEr_ValidDate;
            ckResult = NAK;
        }
        
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
	
    SaveStaffCardData();
   // nErrorCode = 0;
    
    if(ckResult)
    {

	CardSuccess();
	RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
	CardFailed();	
    }	    
}

void GetEmergencyCard(void)
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity = 0;
    
    ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {  
        nCardResult = TRUE;
        ckResult = ACK; 

	uiSecurity = Read_Word_Data(eSETSTART + eEmgSecurity);
        ckSecurity = GetEmergencySecurityNumber(); 
        if(ckSecurity)
        {         
            nCardResult = TRUE;
            ckResult = ACK;         
        }
        else
        {
            nCardResult = FALSE;
            nErrorCode = nEr_Security;
            ckResult = NAK;
        }   

    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
    SaveStaffCardData();
    //nErrorCode = 0; 

    if(ckResult)
    {
	if(ckSecurity) Save_Word_Data(eSETSTART + eEmgSecurity, cSecurity);
	CardSuccess();
	RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
	CardFailed();	
    }	        
}  
  
void GetCashCard(void)
{
    /*
    //TransmitReaderAck(READER_CARD_REQ, NAK);
    if(SendAddr == 0) ReaderAck0 = NAK;
    else if(SendAddr == 1) ReaderAck1 = NAK;
    else if(SendAddr == 2) ReaderAck2 = NAK;
    else if(SendAddr == 3) ReaderAck3 = NAK;
    */
    SaveStaffCardData();
    //memset(RxData1, 0x00, sizeof(RxData1));
    //delay_ms(3);
    //TransmitReaderAck(SendAddr, READER_CARD_REQ, NAK);  
}  

void GetFunctionCard(void)
{
    SaveStaffCardData();
    //memset(RxData1, 0x00, sizeof(RxData1));
    //delay_ms(3);
    //TransmitReaderAck(SendAddr, READER_CARD_REQ, NAK);  
}  

void GetResetCard(void)
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity;
    WORD tAddr;
    
    ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {  
		nCardResult = TRUE;
		ckResult = ACK;  
		uiSecurity = Read_Word_Data(eSETSTART + eSystemSecurity);
		ckSecurity = GetSystemSecurityNumber();
        if(ckSecurity >= 1)
        {  			 
			nCardResult = TRUE;
			ckResult = ACK;

			MasterArea = 1;
			tAddr = eSETSTART + eMasterArea;
			Save_Byte_Data(tAddr, 0xff);
			delay_ms(6);

			MaidArea = 1;  
			tAddr = eSETSTART + eMaidArea;
			Save_Byte_Data(tAddr, 0xff);
			delay_ms(6);

			MiniBarArea = 1;
			tAddr = eSETSTART + eMiniArea;
			Save_Byte_Data(tAddr, 0xff);
			delay_ms(6);            

			SpAreaNo = 0;
			tAddr = eSETSTART + eSpecialArea;
			Save_Byte_Data(tAddr, 0xff);
			delay_ms(6); 
			
			tAddr = eSETSTART + eGuestSecurity;
			Save_Page_Init(tAddr, 26, 0xff);
			delay_ms(7);

			ResetState = 0;

			tAddr = eSETSTART + eSystemSecurity;
            		Save_Word_Data(tAddr, cSecurity);
    	}    
		else
		{
		    nCardResult = FALSE;
		    nErrorCode = nEr_Security;
		    ckResult = NAK;
		}
	}
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
    
    SaveStaffCardData();
    //nErrorCode = 0; 
    //delay_ms(3);

    if(ckResult)
    {
	Timer1_Sec_Stop();
	T1IRQ = 0;
	Timer1_Sec_Start(2);		//4초 timer 

	CardSuccess();
	//RelayOn();
	//NEW_CARD = 1;
	ResetState |= 0x02;
    }
    else
    {
	CardFailed();	
    }	        
}

void GetTimeCard(void)
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity;
    WORD tAddr;
	
    ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {  
		ckResult = ACK;
		nCardResult = TRUE;
		uiSecurity = Read_Word_Data(eSETSTART + eSystemSecurity);
		ckSecurity = GetSystemSecurityNumber();
        if(ckSecurity >= 1)
		{ 

			rtc_year  = MyBuf[7];
			rtc_month = MyBuf[8];  
			rtc_date  = MyBuf[9];

			//rtc_week  = CardData[8];
			rtc_hour  = MyBuf[10];
			rtc_min   = MyBuf[11];
			rtc_sec   = MyBuf[12];

			rtc_sec   = 0;
			Rtc_Write_Data();
			//ResetState |= 0x02;

		}    
		else
		{
			nCardResult = FALSE;
			nErrorCode = nEr_Security;
			ckResult = NAK;
		}       
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
	
    
    SaveSetCardData();
   // nErrorCode = 0; 
    //delay_ms(3);

    if(ckResult)
    {
  

	
	Timer1_Sec_Stop();
	T1IRQ = 0;
    	Timer1_Sec_Start(2);		//2초 timer 

	CardSuccess();
	//RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
	CardFailed();	
    }	    
}

void GetInitCard(void)
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity;
    WORD tAddr, tBuf;
    
    ckHotelCode = GetHotelCode();

    if(ckHotelCode)
    {  
		nCardResult = TRUE;
		ckResult = ACK;
		uiSecurity = Read_Word_Data(eSETSTART + eSystemSecurity);
		ckSecurity = GetSystemSecurityNumber();
		if(ckSecurity >= 1)
		{      
			nCardResult = TRUE;
			ckResult = ACK;

			tBuf = MyBuf[3];
			tBuf = (tBuf << 8);
			tBuf |= MyBuf[4];
			tAddr = eSETSTART + eRoomNumber;
			Save_Word_Data(tAddr, tBuf);
			delay_ms(6);

			ReaderType = MyBuf[7];
			tAddr = eSETSTART + eReaderType;
			Save_Byte_Data(tAddr, ReaderType);
			delay_ms(6);

			MasterArea = MyBuf[9];
			tAddr = eSETSTART + eMasterArea;
			Save_Byte_Data(tAddr, MasterArea);
			delay_ms(6);

			MaidArea = MyBuf[10];
			tAddr = eSETSTART + eMaidArea;
			Save_Byte_Data(tAddr, MaidArea);
			delay_ms(6);

			MiniBarArea = MyBuf[11];
			tAddr = eSETSTART + eMiniArea;
			Save_Byte_Data(tAddr, MiniBarArea);
			delay_ms(6);   

			BusAddress = MyBuf[12];
			tAddr = eSETSTART + eBusAddress;
			Save_Byte_Data(tAddr, BusAddress);
			delay_ms(6);  

			SpAreaNo = MyBuf[13];
			tAddr = eSETSTART + eSpecialArea;
			Save_Byte_Data(tAddr, SpAreaNo);
			delay_ms(6);  
			 
		        }
		else
		{
			nCardResult = FALSE;
			nErrorCode = nEr_Security;
			ckResult = NAK;
		}    
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    } 

    SaveSetCardData();
   // nErrorCode = 0; 
            
    if(ckResult)
    {
	Timer1_Sec_Stop();
	T1IRQ = 0;
    	Timer1_Sec_Start(2);		//4초 timer 

	CardSuccess();
	//RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
	CardFailed();	
    }	
}

void GetHotelCodeCard(void)
{
    WORD tHotelCode, tAddr;
    
    tHotelCode = MyBuf[5];
    tHotelCode = (tHotelCode << 8);
    tHotelCode |= MyBuf[6];    
    uiHotelCode = tHotelCode;
    
    tAddr = eSETSTART + eHOTELCODE;
    //eeprom_write_word(nHOTELCODE,uiHotelCode);
    //Save_Word_Data(tAddr, uiHotelCode);
    Save_Word_Data(tAddr, uiHotelCode);
    delay_ms(6);
    
    Timer1_Sec_Stop();
    T1IRQ = 1;
    Timer1_Sec_Start(1);		//4초 timer 
        
    CardSuccess();
	//RelayOn();
    //NEW_CARD = 1;  
}

void GetContactCard(void)
{
    BYTE ckResult = 0;
    BYTE BufCall, BufAuto;
    WORD tAddr;
     
    nCardResult = TRUE;
    ckResult = ACK;   
    
    BufAuto = MyBuf[5];
    BufCall = MyBuf[6];
    // Auto Time
    if(!(BufAuto == 0))
    {
        FloorTime = BufAuto/10;
	if(FloorTime==0)
		FloorTime=1;
        //eeprom_write_byte(nFLOORTIME,BufFloor);
        tAddr = eSETSTART + eAUTOTIME;
        //Save_Byte_Data(tAddr, FloorTime);
        Save_Byte_Data(tAddr, FloorTime);
        delay_ms(6);
    } 
    // Call Time
    if(!(BufCall == 0))
    {
        CallTime = BufCall/10;
	if(CallTime==0)
		CallTime=1;
        tAddr = eSETSTART + eRelayGreenLED;
        Save_Byte_Data(tAddr, CallTime);
        delay_ms(6);
    }             
    
    if(ckResult)
    {
	Timer1_Sec_Stop();
	T1IRQ = 1;
    	Timer1_Sec_Start(1);		//4초 timer 
        
	CardSuccess();
	//RelayOn();
	//NEW_CARD = 1;
    }
    else
    {
	CardFailed();	
    }	  
}

void GetTimeSetCard(void)
{
    rtc_week  = MyBuf[5];
    rtc_year  = MyBuf[6];
    rtc_month = MyBuf[7];
    rtc_date  = MyBuf[8];  
   
    rtc_hour  = MyBuf[9];
    rtc_min   = MyBuf[10];
    rtc_sec   = MyBuf[11];
    
    Rtc_Write_Data();
    
    Timer1_Sec_Stop();
    T1IRQ = 1;
    Timer1_Sec_Start(1);		//4초 timer 
        
    CardSuccess();
	//RelayOn();
    //NEW_CARD = 1;  
}

void GetAreaSetCard(void)
{
    WORD BufArea, tAddr;
    
    BufArea = MyBuf[5];
    SpAreaNo = BufArea;
    tAddr = eSETSTART + eSpecialArea;
 
    Save_Byte_Data(tAddr, BufArea);
    delay_ms(6);
    
    Timer1_Sec_Stop();
    T1IRQ = 1;
    Timer1_Sec_Start(1);		//4초 timer 
        
    CardSuccess();
    //RelayOn();
   // NEW_CARD = 1;  
}
void GetParameterCard(void)
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity, tCard;
    WORD tAddr;  
    float fTemp;
    
    ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {
 
		ckResult = ACK;
		nCardResult = TRUE;
		uiSecurity = Read_Word_Data(eSETSTART + eSystemSecurity);
		ckSecurity = GetSystemSecurityNumber();
		if(ckSecurity >= 1)
		{  	
			// Green LED Time
			fTemp = MyBuf[7];
			CallTime= (BYTE)(fTemp);
			CallTime=CallTime/4;
			//GLedTime=CardData[7];

			tAddr = eSETSTART + eRelayGreenLED;
			Save_Byte_Data(tAddr, CallTime);
			delay_ms(6);    
			// Red LED Time
			fTemp = MyBuf[8];
			RLedTime = (BYTE)(fTemp);
			RLedTime=RLedTime/4;
			//RLedTime=CardData[8];

			tAddr = eSETSTART + eRedLed;
			Save_Byte_Data(tAddr, RLedTime);
			delay_ms(6);

			tCard = MyBuf[9];
			tCard = (tCard & 0x08) >> 3;
			Office=tCard;
			tAddr = eSETSTART + officeset;
			Save_Byte_Data(tAddr,tCard);
			delay_ms(6);
		}    
		else
		{
			nCardResult = FALSE;
			nErrorCode = nEr_Security;
			ckResult = NAK;
		} 
              
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
    
    SaveSetCardData();
    //nErrorCode = 0; 
    //delay_ms(3);

    if(ckResult)
    {        
	 if(ckSecurity) //eeprom_write_word(nTimeSecurity,cSecurity);  
        {
            tAddr = eSETSTART + eSystemSecurity;
            Save_Word_Data(tAddr, cSecurity);
        }  
	CardSuccess();
	//RelayOn();
	
    }
    else
    {
	CardFailed();	
    }	  
}

void GetHandheldCard(void) //2020.11.19 김태성 사원 수정
{
	BYTE ckResult = 0, ckHotelCode;

	ckHotelCode = GetHotelCode();
	if(ckHotelCode)
	{
		ckResult = ACK;
		nCardResult = TRUE;    
		HHTime = MyBuf[5];		
	}
	else
	{
		nCardResult = FALSE;
		nErrorCode = nEr_HotelCode;
		ckResult = NAK;
	}  
	SaveSetCardData();
	// nErrorCode = 0; 
	//delay_ms(3);

	if(ckResult)
	{
		All_LED_On();
		fHandheld = 1;
		NEW_CARD = 0; //주석 처리시 정보조회기 기능을 하지 않음.		
		//Timer0_mSec_Stop();
		Timer1_Sec_Stop();
		T1IRQ = 0;
		Timer1_Sec_Start(HHTime);		//2초 timer 
		Buzzer_On();
		delay_ms(100);
		Buzzer_Off();
	}
	else
	{
		CardFailed();	
	}	 
}	
void Hotelcode_Card_Set(void) //2020.11.18 김태성 사원 수정
{
	WORD tHotelCode, tAddr;
	BYTE ucCommand, ucSub;
	 uiHotelCode = Read_Word_Data(eSETSTART + eHOTELCODE);
	if(uiHotelCode==0xffff||uiHotelCode==1888)
		{
			tHotelCode = MyBuf[1];
			tHotelCode = (tHotelCode << 8);
			tHotelCode |= MyBuf[2];    
			uiHotelCode = tHotelCode;

			tAddr = eSETSTART + eHOTELCODE;
			Save_Word_Data(tAddr, uiHotelCode);
			delay_ms(5);
			Save_Byte_Data(eSETSTART + eHotelCodeSet, 1);

			delay_ms(5);
			BLE_AdPk_Set();

			LED_BLUE=1;
			BuzzerOn();
			delay_ms(150);
			BuzzerOff();
			LED_BLUE=0;
		}
	else
		{
			LED_RED=1;
			delay_ms(200);
			LED_RED=0;
		}
	
}

void GetAddressCard(void)   // 21.02.23 hy 추가  
{
    BYTE ckResult = 0, ckHotelCode, ckSecurity;
    WORD tAddr;
    
    ckHotelCode = GetHotelCode();
    if(ckHotelCode)
    {
        //uiSecurity = eeprom_read_word(nTimeSecurity); 
        uiSecurity = Read_Word_Data(eSETSTART + eSystemSecurity);
        //if(uiSecurity == 0xffff) uiSecurity = 0;
        ckSecurity = GetSystemSecurityNumber(); 
        if(ckSecurity)
        {
		BusAddress = MyBuf[7];
         	Save_Byte_Data(eSETSTART + eBusAddress,BusAddress);
		delay_ms(6);
		ckResult=ACK;
        }
        else
        {
            nCardResult = FALSE;
            nErrorCode = nEr_Security;
            ckResult = NAK;
        }                 
    }
    else
    {
        nCardResult = FALSE;
        nErrorCode = nEr_HotelCode;
        ckResult = NAK;
    }  
   
    //nErrorCode = 0; 

    if(ckResult)
    {
        if(ckSecurity) //eeprom_write_word(nTimeSecurity,cSecurity);  
        {
            tAddr = eSETSTART + eSystemSecurity;
            Save_Word_Data(tAddr, cSecurity);
        }           
	
	CardSuccess();

    }
    else
    {
	CardFailed();	
    }	    
	 SaveSetCardData();
}


void CheckCardClass(void)
{
    BYTE cCardClass = 0;
	BYTE i=0;
	ACU_DATA_CARD= 0; //2020.11.17 김태성 사원 수정, 우이동 ACU 연동 변수 
    cCardClass = MyBuf[0];
    nErrorCode=0;
    pEnterSave=Read_Word_Data(eSETSTART+eENTERSAVE);
    WDT; 
	
	switch(cCardClass)
    {
    	
        case GUEST_CARD:
            GetGuestCard();
            break;
        case PRECHECK_CARD:
            GetPrecheckCard();
            break; 
        case STANDBY_CARD:
            GetStandbyCard();
            break;
        case ONETIME_CARD:
            GetGuestCard();
            break; 
        case EMERGENCY_CARD:
            GetEmergencyCard();
            break;            
        case GRANDMASTER_CARD:
            GetStaffCard();
            break;
        case MASTER_CARD:
            GetStaffCard();
            break;
        case MAID_CARD:
            GetStaffCard();
            break; 
        case MINIBAR_CARD:
            GetStaffCard();
            break;
	/*
        case CASH_CARD:
            GetCashCard();
            break;
        case FUNCTION_CARD:
            GetFunctionCard();
            break;
        */                    
        case RESET_CARD:
            GetResetCard();
            break;
	 case PARAMETER_CARD:
            GetParameterCard();
            break;
        case TIME_CARD:
            GetTimeCard();
            break;
        case INIT_CARD:
            GetInitCard();
            break;             
        case HOTELCODE_CARD:
            GetHotelCodeCard();
            break;           
	/*
        case CONTACT_CARD:
            GetContactCard();
            break;
        case TIME_SET_CARD:
            GetTimeSetCard();
            break;            
        case AREA_SET_CARD:
            GetAreaSetCard();
            break;
      */
	case HANDHELD_CARD:
		GetHandheldCard();
		break; 
	case Hotelcode_CARD :
		Hotelcode_Card_Set();
		break;
	case SETADDRESS_CARD:		// 21.02.23 hy 추가 
		GetAddressCard();
		break;
        default:
		CardFailed();
		Timer1_Sec_Stop();
		T1IRQ = 0;
		Timer1_Sec_Start(1);		//4초 timer 
            break;
    }    
	for(i=0;i<32;i++) 
	{
		MyBuf[i] = 0;
		//test[i]=TotalData[i];
	}
}



