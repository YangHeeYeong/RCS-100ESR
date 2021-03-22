
#include "common.h"

/*
void TransmitServerTest2()
{
	unsigned char length=0,i;
	WORD temp;
	tx_data[0] = 0x7B;
    tx_data[1] = RxData[1];
    
    tx_data[2] = RxData[2];
    tx_data[3] = RxData[3];
    tx_data[4] = RxData[4];
    tx_data[5] = RxData[5];
    tx_data[6] = RxData[6]; 
    tx_data[7] = RxData[7];
    tx_data[8] = 0x7D;     
	tx_data[length++] = 0x7B;

	for(i=0;i<64;i++)
	{
		temp = Read_Word_Data(eSETSTART + eRoomGroup1+(i*2));

		tx_data[length++] = (BYTE)(temp >> 8);
		tx_data[length++] = (BYTE)(temp & 0x00FF);
		WDT ^= 1;
	}
	
	tx_data[length++] = 0x7D; 
	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(tx_data, length); 
	delay_ms(10);
	UART0_RX_EN;    
}
    */

void TransmitServerAck_On(BYTE ucCommand, BYTE ucSub, BYTE ucAcknak)
{
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    BYTE i;

    TxData0[0] = STX_ACU;
    TxData0[1] = RxData0[1];
    TxData0[2] = BusAddress;	
    TxData0[3] = 9;
    TxData0[4] = ucCommand;  
    TxData0[5] = ucSub;
    TxData0[6] = ucAcknak;
    TxData0[7] = GetChecksum(&TxData0[0],7);
    TxData0[8] = ETX_ACU; 
	
    
    delay_ms(2);
    UART0_TX_EN;    
    UART0_PUT_STRING(TxData0, 9); 
    delay_ms(2);
    UART0_RX_EN;   
    
   // for(i=0;i<10;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
    
    //UART_STRING("\r\n TX Data = ");for(i=0;i<TRH_NFCTxLen;i++) UART_H2A(tx_data0[i]);
}
/*
BYTE UART_RcvCrcChk(void)
{
    BYTE RcvChk = 0;
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;  
    
    uiCRC = CrcCompute(&RxData[0], RxData[3]-3);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
        
    if( CRC_Low == RxData[RxData[3]-2] && CRC_High == RxData[RxData[3]-3] ) RcvChk = 1;
    else RcvChk = 0; 
        
    return RcvChk;
}
*/
BYTE UART_RcvDataChkSum(void)
{
    BYTE tData;//, tLength;
    BYTE RcvChk = 0;
    BYTE tLength = 1;
    BYTE test_temp;

    tLength = RxData0[3];
	
	if(RxData0[2] == BusAddress)
	{
	  	RcvChk = 1;
	}
	else 
	{
		RcvChk = 0;
	}
	
	if(RcvChk) 
	{
		tData = GetChecksum(&RxData0[0], tLength-2);
		if(tData == RxData0[tLength-2])
		{
		 	RcvChk = 1;
		}
		else
		{
			RcvChk = 0;
		}
	}  
	      
    return RcvChk;
}

void RequestReaderStatus_On(void)
{
	BYTE ucCommand, ucSub;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	NEW_CARD = RxData0[6];
	
	TransmitNServerAck(ucCommand, ucSub, ACK);

}

void SetParameter_On(void)
{
    BYTE i, tTemp, tByteBuf;
    float fTemp;
    BYTE ucCommand, ucSub;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    
    tAddr = eSETSTART + eParameter0;
    for(i=0; i<6; i++)
    {
        Save_Byte_Data(tAddr + i, RxData0[6+i]); 
        delay_ms(6);
    }
    
    fTemp = RxData0[12];
    CallTime= (BYTE)(fTemp);
    CallTime=CallTime/4;
    Save_Byte_Data(eSETSTART + eRelayGreenLED, CallTime);      // Green LED Time
    delay_ms(6);    

    fTemp = RxData0[13];
    RLedTime= (BYTE)(fTemp);
    RLedTime=RLedTime/4;
    Save_Byte_Data(eSETSTART + eRedLed, RLedTime);        // Red LED Time
    delay_ms(6); 
    
    tByteBuf = RxData0[14];   
    Save_Byte_Data(eSETSTART + eBlinkLed, tByteBuf);      // LED Blink Time
    delay_ms(6);  
    /*
    //Staff Entry at DND
    tTemp = RxData0[10];
    tTemp = (tTemp & 0x08) >> 3;       //0000 1000 (1 bit)
    tAddr = eSETSTART + eStaffDnd; 
    Save_Byte_Data(tAddr, tTemp);
    delay_ms(6);    

    //Guest Entry at DND
    tTemp = RxData0[10];
    tTemp = (tTemp & 0x10) >> 4;
    tAddr = eSETSTART + eGuestDnd;
    Save_Byte_Data(tAddr,tTemp);
    delay_ms(6);
    */
      //offic type
    tTemp = RxData0[10];
    tTemp = (tTemp & 0x08) >> 3;
    Office=tTemp;
    tAddr = eSETSTART + officeset;
    Save_Byte_Data(tAddr,tTemp);
    delay_ms(6);
    
	Timer1_Sec_Start(1);
     TransmitServerAck_On(ucCommand, ucSub, ACK);
}

void SetCurrentTime_On(void)
{
    BYTE ucCommand, ucSub;
    //BYTE tYear, temp;

    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    //temp = RxData0[7];
    //if((temp >= 12) && (temp > 28)) tYear = temp - 12;  // 2012 ~ 2027
    //else if((temp >= 28) && (temp > 44)) tYear = temp - 28;  // 2028 ~ 2043
    
    rtc_week  = 0x00; //2020.11.12 김태성 사원 수정
    rtc_year  = RxData0[7];
    rtc_month = RxData0[8];
    rtc_date  = RxData0[9];  
   
    rtc_hour  = RxData0[10];
    rtc_min   = RxData0[11];
    rtc_sec   = 0x00; //2020.11.12 김태성 사원 수정
    
    Rtc_Write_Data();
    ResetState |= 0x02;        

    TransmitServerAck_On(ucCommand, ucSub, ACK);
}  

void RequestCurrentTime_On()
{
   // BYTE i;
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    
    Rtc_Read_Data();
    
    TxData0[0] = STX_ACU;
    TxData0[1] = RxData0[1];
    TxData0[2] = BusAddress;
    TxData0[3] = 15;     // Length
    TxData0[4] = RxData0[4];
    TxData0[5] = RxData0[5];
    
    TxData0[6] = 0x00; //2020.11.12 김태성 사원 수정
    TxData0[7] = rtc_year;
    TxData0[8] = rtc_month;
    TxData0[9] = rtc_date;
    TxData0[10] = rtc_hour;
    TxData0[11] = rtc_min;
    TxData0[12] = 0x00; //2020.11.12 김태성 사원 수정

    TxData0[13] = GetChecksum(&TxData0[0],13);
    TxData0[14] = ETX_ACU; 
	
    delay_ms(2);
    UART0_TX_EN;    
    UART0_PUT_STRING(TxData0, 15); 
    delay_ms(20);
    UART0_RX_EN; 
}

void RequestReaderParam_On(void)
{
	BYTE  i,temp,temp1,temp2;
	BYTE length = 0;
    	WORD tAddr;

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = BusAddress;
	length++;
	TxData0[length++] = REQ_PARAMETER_ON;
	TxData0[length++] = RxData0[5];

	
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter0);     // Duration Door Open
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter1);     // Opening Mode
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter2);     // Daylight Saving Time
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter3);     // Daylight Saving Time
	temp1=Read_Byte_Data(eSETSTART+officeset);
	temp2=0;
	if(temp1)
	temp2=temp2+8;
	TxData0[length++] = temp2;//Read_Byte_Data(eSETSTART + eParameter4);     // Status1
	TxData0[length++] = 0x00;									// Status2
	
	TxData0[length++] =Read_Byte_Data(eSETSTART+eRelayGreenLED);    // Green LED Time
	
	TxData0[length++] =Read_Byte_Data(eSETSTART+eRedLed);       // Red LED Time
	TxData0[length++] = Read_Byte_Data(eSETSTART + eBlinkLed);
	TxData0[3] = length+2;
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU;	
	
	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, length); 
	delay_ms(20);
	UART0_RX_EN; 
}


void SetGuestWorkTime_On(void) //2020.11.12 김태성 사원 수정
{
    BYTE i;
    BYTE ucCommand, ucSub;
    BYTE temp;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    for(i=0; i<4; i++)
    {
        tAddr = eSETSTART + eTimeSlotPermanent + i;
	temp = RxData0[6+i];
        Save_Byte_Data(tAddr, temp); 
        delay_ms(6);
        WDT;
    }
    //UART_STRING("\r\n Timeslot = ");for(i=0;i<4;i++) UART_H2A(RxData0[6+i]);
    //for(i=0; i<4; i++) temp[i]=  Read_Byte_Data(eSETSTART + eTimeSlotPermanent + i);
    //UART_STRING("\r\n Timeslot = ");for(i=0;i<4;i++) UART_H2A(temp[i]);
    
    for(i=0; i<4; i++)
    {
        tAddr = eSETSTART + eTimeSlotGuest + i;
	temp = RxData0[10+i];
        Save_Byte_Data(tAddr, temp); 
        delay_ms(6);
        WDT;
    }    
      
    TransmitServerAck_On(ucCommand, ucSub, ACK);
}

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void SetStaffGroup_On(void)
{
	BYTE ucCommand, ucSub, tByteBuf,i;
	WORD tAddr;
	
	ucCommand = RxData0[4];
    	ucSub = RxData0[5];

	for(i=0;i<8;i++)
	{
              tAddr = eSETSTART + eStaffGroup+ i;
              Save_Byte_Data(tAddr, RxData0[6+i]); 
              delay_ms(6);
              WDT;
	}
        
	TransmitServerAck_On(ucCommand, ucSub, ACK);		
}
*/

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void SetGroupSecurity_On(void)		// Set Room Chnage Command    HY 수정 
{

	BYTE ucCommand, ucSub, i,seccnt, temp[4];
	WORD tByteBuf,tAddr;
	
	ucCommand = RxData0[4];
    	ucSub = RxData0[5];
	
	tAddr = eSETSTART + eRoomChangeFlage;
       Save_Byte_Data(tAddr, 0x01); 
       delay_ms(6);
	
	tAddr = eSETSTART + eRoomChangeNo;
	tByteBuf = RxData0[6];
	tByteBuf = (tByteBuf<<8);
	tByteBuf |= RxData0[7];
       Save_Word_Data(tAddr, tByteBuf); // CardRoomNo
       delay_ms(6);

	tAddr = eSETSTART + eRoomChangeSecurity;
	tByteBuf = RxData0[8];
	tByteBuf = (tByteBuf<<8);
	tByteBuf |= RxData0[9];
       Save_Word_Data(tAddr, tByteBuf); // CardSecurity
       delay_ms(6);

 	//-------------------주석 처리 ---------------------
	tByteBuf = Read_Word_Data(eSETSTART + eRoomChangeNo);
	temp[0] = tByteBuf;
	temp[1] = (tByteBuf>>8);

	tByteBuf = Read_Word_Data(eSETSTART + eRoomChangeNo);
	
	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(*tByteBuf, 2); 
	delay_ms(20);
	UART0_RX_EN;   
	//------------------------------------------------

	TransmitServerAck_On(ucCommand, ucSub, ACK);	
	
	//-------------------주석 처리 ---------------------
       // Group Room Security Set
	BYTE ucCommand, ucSub, i,seccnt;
	WORD tByteBuf,tAddr;
	
	ucCommand = RxData[4];
    	ucSub = RxData[5];
	seccnt = RxData[6];
	
	for(i=0; i<3; i++)		// Room CheckOutDate 
	{
		tAddr = eSETSTART+eGRCkoutDate_year+i;
		tByteBuf =  RxData[7+i];
		Save_Byte_Data(tAddr,tByteBuf);
	}
	
	for(i=0;i<seccnt;i++)
	{
		tAddr = eSETSTART + eRoomSecurityStart + (i*2);
		tByteBuf = RxData[10+(i*2)];
		Save_Byte_Data(tAddr,tByteBuf);
		delay_ms(6);
		WDT^=1;
		tByteBuf = RxData[11+(i*2)];
		Save_Byte_Data(tAddr+1,tByteBuf);
		delay_ms(6);
		WDT^=1;
		
		// security No. hy 추가 
		tAddr = eSETSTART + eRoomGroupStart + (i*2);
		tByteBuf = RxData[12+(i*2)];
		Save_Byte_Data(tAddr,tByteBuf);
		delay_ms(6);
		WDT^=1;
		tByteBuf = RxData[13+(i*2)];
		Save_Byte_Data(tAddr+1,tByteBuf);
		delay_ms(6);
		WDT^=1;
	}
	
	for(i=seccnt;i<41;i++)
	{
		tAddr = eSETSTART + eRoomSecurityStart + (i*2);
		Save_Byte_Data(tAddr,0xFF);
		Save_Byte_Data(tAddr+1,0xFF);
		
		// security No. hy 추가 
		tAddr = eSETSTART + eRoomGroupStart + (i*2);
		Save_Byte_Data(tAddr,0xFF);
		Save_Byte_Data(tAddr+1,0xFF);
	}	
	TransmitServerAck_On(ucCommand, ucSub, ACK);	
	//------------------------------------------------

}
*/

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void SetGroupNumber_On(void)	// Set Group Room    hs수정 
{
	BYTE ucCommand, ucSub, tByteBuf,i,RoomCnt;
	WORD tAddr;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	
	RoomCnt = RxData0[6];		// total Group Room cnt
	
	tByteBuf = RxData0[7];		// Check Out Date (Year)
	Save_Byte_Data(tAddr,tByteBuf);
	
	for(i=0;i<RoomCnt;i++)
	{
		tAddr = eSETSTART + eRoomGroupStart + (i*2);
		tByteBuf = RxData0[7+(i*2)];
		Save_Byte_Data(tAddr,tByteBuf);
		delay_ms(6);
		WDT;
		tByteBuf = RxData0[8+(i*2)];
		Save_Byte_Data(tAddr+1,tByteBuf);
		delay_ms(6);
		WDT;
	}
	
	for(i=RoomCnt;i<41;i++)
	{
		tAddr = eSETSTART + eRoomGroupStart + (i*2);
		Save_Byte_Data(tAddr,0xFF);
		Save_Byte_Data(tAddr+1,0xFF);
	}
	
	TransmitServerAck_On(ucCommand, ucSub, ACK);		
}
*/

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void RequestStaffGroup_On(void)
{
	//WORD uiCRC;
	BYTE  i;
	BYTE length;
	//BYTE CRC_Low, CRC_High;

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = BusAddress;
	length++;
	TxData0[length++] = RxData0[4];
	TxData0[length++] = 0x01;

	for(i=0;i<8;i++) TxData0[length++] = Read_Byte_Data(eSETSTART + eStaffGroup + i);
	WDT;
		
	TxData0[3] = length+2;
	
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU; 

	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, length); 
	delay_ms(20);
	UART0_RX_EN;   
}
*/

void SetGuestCKOutTime_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr;
	BYTE tByteBuf, i;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	
	for(i=0; i<64; i++)
	{
		tAddr = eSETSTART + eCheckOutTime1+ i;
		Save_Byte_Data(tAddr, RxData0[6+i]); 
		delay_ms(6);
		WDT;
	}


	TransmitServerAck_On(ucCommand, ucSub, ACK);
}

void RequestGuestWorkTime_On(void)
{
	//WORD uiCRC;
	//WORD  i;//, length;
	BYTE length = 0;
	BYTE tAddr,i;

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = BusAddress;
	TxData0[length++] = 0x00;
	length++;
	//tx_data0[3] = 196;     // Length
	TxData0[length++] = REQ_GUEST_WORKOUTTIME_ON;
	TxData0[length++] = 0x01;


	 for(i=0; i<4; i++) TxData0[length++] = Read_Byte_Data(eSETSTART + eTimeSlotPermanent + i); 
	WDT;
	    
	for(i=0; i<4; i++) TxData0[length++] = Read_Byte_Data(eSETSTART + eTimeSlotGuest + i); 
	WDT;

	//for(i=0;i<64;i++) tx_data[length+i] = Read_Byte_Data(eSETSTART + eCheckOutTime1+ i);
	//WDT ^= 1;
	//length+=64;

	TxData0[4] = length+2;
	//uiCRC = CrcCompute(&tx_data[0], length);
	//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
	//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
	//tx_data[length++] = CRC_High;
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU; 

	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, length); 
	delay_ms(20);
	UART0_RX_EN;   

}

void SetStaffWorkTime_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr;
	BYTE tByteBuf, i;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];

	for(i=0; i<128; i++)
	{
		tAddr = eSETSTART + eStaffTimeSlot + i;
		Save_Byte_Data(tAddr, RxData0[6+i]); 
		delay_ms(6);
		WDT;
	}

	TransmitServerAck_On(ucCommand, ucSub, ACK);
}

void RequestStaffTimeSlot_On(void)
{
	//WORD uiCRC;
	BYTE  i;//, length;
	BYTE length;
	//BYTE CRC_Low, CRC_High;

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = BusAddress;
	length++;
	//tx_data0[3] = 196;     // Length
	TxData0[length++] = RxData0[4];
	TxData0[length++] = 0x01;

	for(i=0;i<128;i++) TxData0[length++] = Read_Byte_Data(eSETSTART + eStaffTimeSlot+ i);
	WDT;
	
	TxData0[4] = length+2;
	//uiCRC = CrcCompute(&tx_data[0], length);
	//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
	//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
	//tx_data[length++] = CRC_High;
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU; 

	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, length); 
	delay_ms(20);
	UART0_RX_EN;   
}


void SetSecurityNumber_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr, tBuf, number;
	BYTE tByteBuf, i;

	ucCommand = RxData0[4];
	ucSub = 0;

	switch(RxData0[6])
	{
		/*
		case GUEST_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eGuestSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case PRECHECK_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + ePreCheckSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break; 
			
		case STANDBY_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eStandbySecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case ONETIME_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eOnetimeSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break; 
		*/
		case EMERGENCY_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eEmgSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;   
		/*	
		case GRANDMASTER_CARD:
			number = RxData0[7];
			number = (number << 8);
			number |= RxData0[8]; 
			
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			
			tAddr = eSETSTART + eGMasterSecurity + number;
			Save_Byte_Data(tAddr,(BYTE)tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case MASTER_CARD:
			number = RxData0[7];
			number = (number << 8);
			number |= RxData0[8]; 
			
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			
			tAddr = eSETSTART + eMasterSecurity + number;
			Save_Byte_Data(tAddr,(BYTE)tBuf);  // Onetime security
			
			delay_ms(6);	
			break;
			
		case MAID_CARD:
			number = RxData0[7];
			number = (number << 8);
			number |= RxData0[8]; 

			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			
			tAddr = eSETSTART + eMaidSecurity+ number;
			Save_Byte_Data(tAddr,(BYTE)tBuf);  // Onetime security
			delay_ms(6);	
			break; 
			
		case MINIBAR_CARD:
			number = RxData0[7];
			number = (number << 8);
			number |= RxData0[8]; 

			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			
			tAddr = eSETSTART + eMinibarSecurity+ number;
			Save_Byte_Data(tAddr,(BYTE)tBuf);  // Onetime security
			delay_ms(6);	
			break;
		*/
		case RESET_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eSystemSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case TIME_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eSystemSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case INIT_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eSystemSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case PARAMETER_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eSystemSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;
			
		case SETADDRESS_CARD:
			tBuf = RxData0[9];
			tBuf = (tBuf << 8);
			tBuf |= RxData0[10];  
			tAddr = eSETSTART + eSystemSecurity;
			Save_Word_Data(tAddr,tBuf);  // Onetime security
			delay_ms(6);	
			break;

		default:
			break;
	} 
	WDT;
	
	 TransmitServerAck_On(ucCommand, ucSub, ACK);
		
}


/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void SetAllStaffSecurity_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr, tBuf, number;
	BYTE tByteBuf, i;
	WORD data_length;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	//data_length = RxData[3] - 10;
	data_length = RxData0[6];
	//data_length = (data_length<<8) | RxData[7];
	
	for(i=0;i<data_length;i++)
	{
		number = RxData0[7+(i*3)];
		//number = (number << 8);
		//number |= RxData[9+(i*4)]; 

		number = (number*2) - 2;

		tBuf = RxData0[8+(i*3)];
		tBuf = (tBuf << 8);
		tBuf |= RxData0[9+(i*3)];  
		
		tAddr = eSETSTART + eStaffSecurity + number;
		Save_Word_Data(tAddr,tBuf);  // Onetime security
		delay_ms(6);	
		WDT;
	}

	TransmitServerAck_On(ucCommand, ucSub, ACK);
}
*/

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void SetAllGuestSecurity_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr, tBuf, number;
	BYTE tByteBuf, i;

	ucCommand = RxData0[4];
	ucSub = 0x01;

	tBuf = RxData0[6];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[7];  
	tAddr = eSETSTART + eGuestSecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);
	WDT;

	tBuf = RxData0[8];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[9];  
	tAddr = eSETSTART + ePreCheckSecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);
	WDT;

	tBuf = RxData0[10];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[11];  
	tAddr = eSETSTART + eOnetimeSecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);
	WDT;

	tBuf = RxData0[12];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[13];  
	tAddr = eSETSTART + eStandbySecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);
	WDT;

	
	TransmitServerAck_On(ucCommand, ucSub, ACK);
}
*/


void SetSpecialAreaNumber_On(void)
{
	BYTE ucCommand, ucSub;
	WORD tAddr;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];

	tAddr = eSETSTART + eSpecialArea;
	SpAreaNo = RxData0[6];
	Save_Byte_Data(tAddr, SpAreaNo); 
	delay_ms(6);

	TransmitServerAck_On(ucCommand, ucSub, ACK);
}
/*//2020.11.12 김태성 사원 수정, 현재 사용 안함 추후 사용에 주석 처리 
void RequestSpecialAreaNumber_On(void) 
{
    BYTE i;
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0; 
    TxData0[3] = 9;            // Packet Length		
    TxData0[4] = REQ_NSPEACIAL_AREA;
    TxData0[5] = 0x01; 
    
    TxData0[6] = Read_Byte_Data(eSETSTART + eSpecialArea);
    
    TxData0[7] = GetChecksum(&TxData0[0],7);
    TxData0[8] = ETX_ACU;

    delay_ms(2);
    UART0_TX_EN;    
    UART0_PUT_STRING(TxData0, 9); 
    delay_ms(20);
    UART0_RX_EN; 
}
*/
//special area / maid / suite / master / minibar / guest / standby / onetime / pre-checkin / emergency / reader type / hour min ...
void SetInitialize_On(void)
{
	WORD tAddr, tBuf;
	BYTE tByteBuf, i,ucCommand,ucSub;
	float fTemp;
	ucCommand = RxData0[4];
	ucSub = RxData0[5];


	tByteBuf = RxData0[6];
	SpAreaNo = tByteBuf;
	Save_Byte_Data(eSETSTART + eSpecialArea, tByteBuf);     // Special Area Number
	delay_ms(6);

	tByteBuf = RxData0[7];
	Save_Byte_Data(eSETSTART + eMaidArea, tByteBuf);      // Maid Area  
	delay_ms(6);    

	tByteBuf = RxData0[8];
	Save_Byte_Data(eSETSTART + eSuiteArea, tByteBuf);     // Suite Area
	delay_ms(6);

	tByteBuf = RxData0[9];
	Save_Byte_Data(eSETSTART + eMasterArea, tByteBuf);    // Master Area  
	delay_ms(6);

	tByteBuf = RxData0[10];
	Save_Byte_Data(eSETSTART + eMiniArea, tByteBuf);      // Minibar Area 
	delay_ms(3);

	tBuf = RxData0[11];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[12];  
	tAddr = eSETSTART + eGuestSecurity;
	Save_Word_Data(tAddr,tBuf);  // Guest security
	delay_ms(6);

	tBuf = RxData0[13];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[14];  
	tAddr = eSETSTART + eStandbySecurity;
	Save_Word_Data(tAddr,tBuf);  // Standby security
	delay_ms(6);

	tBuf = RxData0[15];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[16];  
	tAddr = eSETSTART + eOnetimeSecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);

	tBuf = RxData0[17];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[18];  
	tAddr = eSETSTART + ePreCheckSecurity;
	Save_Word_Data(tAddr,tBuf);  // Pre-Check security
	delay_ms(6);

	tBuf = RxData0[19];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[20];  
	tAddr = eSETSTART + eEmgSecurity;
	Save_Word_Data(tAddr,tBuf);  // Emergency security
	delay_ms(6);
	
	tByteBuf = RxData0[21];
	ReaderType = tByteBuf;				//2020.11.18 김태성 사원 수정 Spcial Reader Data Fix
	tAddr = eSETSTART + eReaderType; //2020.11.18 김태성 사원 수정 Spcial Reader Data Fix                         
	Save_Byte_Data(tAddr, tByteBuf);   // Type of Reader
	delay_ms(6);

	for(i=0; i<64; i++)
	{  
		tByteBuf = RxData0[22 + i];
		Save_Byte_Data(eSETSTART + eCheckOutTime1 + i, tByteBuf);
		delay_ms(6);
		WDT;
	}

	rtc_week  = RxData0[86];
	rtc_year  = RxData0[87];
	rtc_month = RxData0[88];
	rtc_date  = RxData0[89];  

	rtc_hour  = RxData0[90];
	rtc_min   = RxData0[91];
	rtc_sec   = RxData0[92];

	Rtc_Write_Data();
	WDT;
       TransmitServerAck_On(ucCommand, ucSub, ACK);
	tAddr = eSETSTART + eGuestSecurity;
	Save_Page_Init(tAddr, 26, 0xff);
	delay_ms(7);
	WDT;

	// Grand Master Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eGMasterSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
		WDT;
	}

	// Master Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eMasterSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
		WDT;
	}  

	// Maid Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eMaidSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
		WDT;
	} 

	// Minibar Security Initializing
	for(i=0; i<16; i++) 
	{
		//Save_Byte_Data(eSETSTART + eMinibarSecurity + i, 0xff);
		tAddr = eSETSTART + eMinibarSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
		WDT;
	}                  

	//TransmitServerAck_On(SET_INITIALIZE, 1, ACK); 
	/*
	rtc_week  = RxData1[45];
	rtc_year  = RxData1[46];
	rtc_month = RxData1[47];
	rtc_date  = RxData1[48];  
	rtc_hour  = RxData1[49];
	rtc_min   = RxData1[50];
	rtc_sec   = RxData1[51];
	Rtc_Write_Data();  
	*/
	/*
	delay_ms(6);
	Rtc_Read_Data();
	tx_data0[0] = 0x7B;
	tx_data0[1] = rtc_week;   // Hign Byte
	tx_data0[2] = rtc_year;         // Low Byte
	tx_data0[3] = rtc_month;   // Hign Byte
	tx_data0[4] = rtc_date;         // Low Byte
	tx_data0[5] = rtc_hour;         // Low Byte
	tx_data0[6] = rtc_min;
	tx_data0[6] = rtc_sec;
	tx_data0[7] = 0x7D;     
	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(tx_data0, 8); 
	delay_ms(6);
	UART0_RX_EN;     
	*/


	//Rtc_Write_Data();
}

/*  //2020.11.12, 김태성 사원 수정, 사용 안함 
void Extend_CheckoutDate_On(void)
{
	BYTE i;   
	
	TxData0[0] = STX_ACU;
	TxData0[1] = RxData0[1];
	TxData0[2] = BusAddress;
	TxData0[3] = 0x09;
	TxData0[4] = RxData0[4];
	TxData0[5] = 0x01;
	TxData0[6] = 0x01; 	

	TxData0[7] = GetChecksum(&TxData0[0],7);
	TxData0[8] = ETX_ACU; 

	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, 9); 
	delay_ms(20);
	UART0_RX_EN;   
}
*/

/*//2020.11.12 김태성 사원 수정, 현재 사용 안함 추후 사용에 주석 처리 
void RequestOnLineCheck_On(void) //2020.11.12 김태성 사원 수정
{
	//WORD uiCRC;
	BYTE  i;
	BYTE length;
	//BYTE CRC_Low, CRC_High;

	TxData0[0] = STX_ACU;
	TxData0[1] = RxData0[1];
	TxData0[2] = BusAddress;
	TxData0[3] = 0x09;
	TxData0[4] = RxData0[4];
	TxData0[5] = 0x01;
	TxData0[6] = 0x00; 	

	TxData0[7] = GetChecksum(&TxData0[0],7);
	TxData0[8] = ETX_ACU; 

	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, 9); 
	delay_ms(20);
	UART0_RX_EN;   
}
*/

void RequestInformation_On(void)
{
	BYTE i;//, length = 0;
	BYTE length = 0;
	//BYTE CRC_Low, CRC_High;
	BYTE tHigh, tLow, tByteBuf, temp1, temp2;
	//WORD uiCRC; 
	WORD tBuf;
	//float fTemp = 0.0;

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = BusAddress;
	length++;
	//tx_data0[3] = 196;     // Length
	TxData0[length++] = REQ_INFORMATION_ON;
	TxData0[length++] = 0x01;

	TxData0[length++] = 1;                // Hardware Address	//2020.11.12 김태성 사원 수정 Spcial Reader Data Fix
	tByteBuf = Read_Byte_Data(eSETSTART + eReaderType);		
	if(tByteBuf == 0xff) tByteBuf = 0;    
	TxData0[length++] = tByteBuf;                // Reader Type(0:Guest Room, )	//8

	tBuf = Read_Word_Data(eSETSTART + eRoomNumber);
	if(tBuf == 0xffff) tBuf = 1;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;        // Room Number	//9
	TxData0[length++] = tLow;     				//10

	Rtc_Read_Data();
	TxData0[length++] = rtc_year;      	//Year		//11
	TxData0[length++] = rtc_month;     //Month		//12
	TxData0[length++] = rtc_date;      	//Date		//13
	TxData0[length++] = 0x00;      		//Week		//14 //2020.11.12 김태성 사원 수정
	TxData0[length++] = rtc_hour;      	//Hour		//15	
	TxData0[length++] = rtc_min;       	//Minite		//16
	TxData0[length++] = 0x00;       		//Second		//17 //2020.11.12 김태성 사원 수정

	TxData0[length++] = 0xff;          // Voltage Control	//18
	TxData0[length++] = 2;             // Firmware Version 1	//19 //2020.11.12 김태성 사원 수정 예정
	TxData0[length++] = 0;             // Firmware Version 2	//20 //2020.11.12 김태성 사원 수정 예정
	TxData0[length++] = 0;             // Firmware Version 3	//21 //2020.11.12 김태성 사원 수정 예정

	tBuf = Read_Word_Data(eSETSTART + eHOTELCODE);
	//tBuf = eeprom_read_word(nHOTELCODE);
	tLow = (BYTE)tBuf;           // crc Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // crc Hign Byte    
	TxData0[length++] = tHigh;        // Hotelcode		//22
	TxData0[length++] = tLow;					//23

	tByteBuf = Read_Byte_Data(eSETSTART + eSpecialArea);
	if(tByteBuf == 0xff) tByteBuf = 1;
	TxData0[length++] = tByteBuf;            // Special Area	//24

	//tBuf = eeprom_read_word(nEmgSecurity);                // Emergency Security
	tBuf = Read_Word_Data(eSETSTART + eEmgSecurity);
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;          				//25
	TxData0[length++] = tLow;                                 //26   

	tBuf = Read_Word_Data(eSETSTART + eSystemSecurity);     // System Security
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte        
	TxData0[length++] = tHigh;                              //27         
	TxData0[length++] = tLow;                               //28           

	//tBuf = eeprom_read_word(nGuestSecurity);              // Guest Security
	if(ReaderType == Type_GroupReader) tBuf = 0xffff;
	else
	{
		tBuf = Read_Word_Data(eSETSTART + eGuestSecurity);
		if(tBuf == 0xffff) tBuf = 0;
	}
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;          			//29
	TxData0[length++] = tLow;  				//30

	//tBuf = eeprom_read_word(nPreCheckSecurity);             // Pre-checkin Security
	tBuf = Read_Word_Data(eSETSTART + ePreCheckSecurity);
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;          
	TxData0[length++] = tLow;   

	//tBuf = eeprom_read_word(nStandbySecurity);             // Standby Security
	tBuf = Read_Word_Data(eSETSTART + eStandbySecurity);
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;          
	TxData0[length++] = tLow;        

	//tBuf = eeprom_read_word(nOnetimeSecurity);             // Onetime Security
	tBuf = Read_Word_Data(eSETSTART + eOnetimeSecurity);
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
	TxData0[length++] = tHigh;          
	TxData0[length++] = tLow;          

	tBuf = Read_Word_Data(eSETSTART + eSuiteSecurity);      // Suite Security
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte        
	TxData0[length++] = tHigh;                                       
	TxData0[length++] = tLow;    

	tBuf = Read_Word_Data(eSETSTART + eManagerSecurity);    // Manager Security
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte            
	TxData0[length++] = tHigh;                                      
	TxData0[length++] = tLow; 

	tBuf = Read_Word_Data(eSETSTART + eConfSecurity);       // Conference Security
	if(tBuf == 0xffff) tBuf = 0;
	tLow = (BYTE)tBuf;           // Low Byte
	tHigh = (BYTE)(tBuf >> 8);   // Hign Byte              
	TxData0[length++] = tHigh;                                     
	TxData0[length++] = tLow;     
	
	tByteBuf=Read_Byte_Data(eSETSTART + eMasterArea); 
	TxData0[length++] = tByteBuf;
	
	tHigh=Read_Byte_Data(eSETSTART + eMasterSecurity);
	tLow=Read_Byte_Data(eSETSTART + eMasterSecurity+1);
	TxData0[length++] = tHigh;
	TxData0[length++] = tLow;
	
	tByteBuf=Read_Byte_Data(eSETSTART + eMaidArea); 
	TxData0[length++] = tByteBuf;
	
	tHigh=Read_Byte_Data(eSETSTART + eMaidSecurity);
	tLow=Read_Byte_Data(eSETSTART + eMaidSecurity+1);
	TxData0[length++] = tHigh;
	TxData0[length++] = tLow;
	
	tByteBuf=Read_Byte_Data(eSETSTART + eMiniArea); 
	TxData0[length++] = tByteBuf;
	
	tHigh=Read_Byte_Data(eSETSTART + eMinibarSecurity);
	tLow=Read_Byte_Data(eSETSTART + eMinibarSecurity+1);
	TxData0[length++] = tHigh;
	TxData0[length++] = tHigh;

	temp1=Read_Byte_Data(eSETSTART+officeset);
	temp2=0;
	if(temp1)
	temp2=temp2+8;
	TxData0[length++] =temp2;//= Read_Byte_Data(eSETSTART + eParameter0);     // Duration Door Open
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter1);     // Opening Mode
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter2);     // Daylight Saving Time
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter3);     // Daylight Saving Time
	TxData0[length++] = Read_Byte_Data(eSETSTART + eParameter4);     // Status
	
	CallTime=Read_Byte_Data(eSETSTART+eRelayGreenLED);
	if(CallTime==0xff)
		CallTime=3;
	else
		CallTime=CallTime;
	if(CallTime<1)
		CallTime=3;
	TxData0[length++] = CallTime;       // Green LED Time
	
	 RLedTime=Read_Byte_Data(eSETSTART+eRedLed);
	if(RLedTime==0xff)
		RLedTime=1;
	else
		RLedTime=RLedTime;
	if(RLedTime<1)
		RLedTime=1;
	TxData0[length++] = RLedTime;         // Red LED Time

	for(i=0;i<64;i++)  TxData0[length++] = Read_Byte_Data(eSETSTART + eCheckOutTime1+i);        // Check-out Time 1 Hour
	

	//for(i=0;i<128;i++) tx_data[length++] = Read_Byte_Data(eSETSTART + eTimeSlot1 + i);



	TxData0[3] = length+2;
	//tx_data[3] = 10;
	//length=10;
	//uiCRC = CrcCompute(&tx_data[0], length);
	//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
	//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
	//tx_data[length++] = CRC_High;
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU; 

	
	delay_ms(2);
	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0, length); 
	delay_ms(20);
	UART0_RX_EN;   
	
   

}

void RequestEventData_On(void)
	{
    BYTE tmp,j,ctmp;
    BYTE CRC_Low, CRC_High;
	BYTE ucCommand,ucSub;
	BYTE length=0;
	ucCommand = RxData0[4];
	ucSub = RxData0[5];
    //BYTE tPage;
    WORD uiCRC, ckReadAddr, tAddr, tAddress;


    //UART_STRING("\r\n OverwriteState = ");UART_H2A(OverwriteState);
    //UART_STRING("\r\n fStartRead = ");UART_H2A(fStartRead);
    //TxCount0 = 0;
		    if(OverwriteState == 0)
		    {
		        if(pEnterSave > pEnterSend) 
		          CountSendEnter = pEnterSave - pEnterSend-1;
		        else CountSendEnter = 0;
		        RE_cnt=pEnterSave-pEnterSend-1;
			RE_SA=pEnterSend+1;
			RE_EA=pEnterSave-1;
		    }
		    
		    else  // Overwrite state
		    {  
		        if(pEnterSave >= pEnterSend||OverwriteState>1)
					{
						pEnterSend = pEnterSave;
						OverwriteState=1;
						if(pEnterSend>400)
							{
								pEnterSend=1;
								OverwriteState=0;
							}
						
						RE_cnt=400;
						RE_SA=pEnterSend;
						RE_EA=pEnterSend+400-1;
						
							
					}
			else
				{
					RE_cnt=400+pEnterSave-pEnterSend-1;
					RE_SA=pEnterSend+1;
					RE_EA=pEnterSave+400-1;
				}
				
			CountSendEnter=1;
		    }    
			tAddr = eSETSTART + eOVERWRITE;
		       Save_Byte_Data(tAddr, OverwriteState); 
		    if(CountSendEnter == 0) //
		    {  
		      TxData0[length++] = STX_ACU;
			TxData0[length++] = RxData0[1];
			TxData0[length++] = BusAddress;
			length++;
			TxData0[length++] = ucCommand;
			TxData0[length++] = ucSub;
			TxData0[length++] = ACK;
			
			
			TxData0[length++] = 0x00;	//Event Number
			TxData0[length++] = 0x00;
			TxData0[length++] = 0x00;	//Last Event Number
			TxData0[length++] = 0x00;

			//for(i=0;i<30;i++) tx_data[length+i] = 0;

			TxData0[3] = length+2;
			//uiCRC = CrcCompute(&tx_data[0], length);
			//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
			//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
			//tx_data[length++] = CRC_High;
			TxData0[length++] = GetChecksum(&TxData0[0],length);
			TxData0[length++] = ETX_ACU;  
			
			UART0_TX_EN;    
			UART0_PUT_STRING(TxData0,length); 
			delay_ms(5);
			UART0_RX_EN;
			pEnterSend=pEnterSave-1;
			Save_Word_Data(eSETSTART+eENTERSEND, pEnterSend);
		    }
		else
			{
				TxData0[length++] = STX_ACU;
				TxData0[length++] = RxData0[1];
				TxData0[length++] = BusAddress;
				length++;
				TxData0[length++] = ucCommand;
				TxData0[length++] = ucSub;
				TxData0[length++] = ACK;
				pEnterSend=pEnterSend+1;
				TxData0[length++] = 0x00;	//Event Number
				TxData0[length++] = 0x01;
				TxData0[length++] = 0x00;	//Last Event Number
				TxData0[length++] = 0x01;

				//for(i=0;i<30;i++) tx_data[length+i] = 0;

				
				
				ckReadAddr=pEnterSend*32;
				ReadExtMemoryPageData(ckReadAddr, 30, &TxData0[length]);
				length=length+30;
				//uiCRC = CrcCompute(&tx_data[0], length);
				//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
				//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
				//tx_data[length++] = CRC_High;
				TxData0[3] = length+2;
				TxData0[length++] = GetChecksum(&TxData0[0],length);
				TxData0[length++] = ETX_ACU;  

				UART0_TX_EN;    
				UART0_PUT_STRING(TxData0,length); 
				delay_ms(5);
				UART0_RX_EN;
			}
			
	/*
	LED_GRE=0;
	TxCount0=150;
	 for(i=0;i<TxCount0;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = i;
	 */
	if(pEnterSend>400)
		{
			OverwriteState = 0;
	            pEnterSend = 1;
		     tAddr = eSETSTART + eENTERSEND;
	            Save_Word_Data(tAddr, pEnterSend); 
	            tAddr = eSETSTART + eOVERWRITE;
	            Save_Byte_Data(tAddr, 0); 
		}
	
}
	/*
{
	BYTE ucCommand,ucSub;
	BYTE i;//, length;
	BYTE length=0;
	//BYTE CRC_Low, CRC_High;
	//BYTE tPage;
	WORD ckReadAddr, tAddr, tAddress;
	//WORD uiCRC;
	
	ucCommand = RxData[4];
	ucSub = RxData[5];
	//UART_STRING("\r\n OverwriteState = ");UART_H2A(OverwriteState);
	//UART_STRING("\r\n fStartRead = ");UART_H2A(fStartRead);

	//TxCount0 = 0;
	if(OverwriteState == 0)
	{
		if(pEnterSave > pEnterSend) 
		CountSendEnter = pEnterSave - pEnterSend-1;
		else CountSendEnter = 0;

		if(fStartRead == 0)
		{
			pLastEvent = CountSendEnter;
			fStartRead = 1;
		}
	}
	else  // Overwrite state : exceed 400 events
	{  
		if(pEnterSave >= pEnterSend||OverwriteState>1) pEnterSend = pEnterSave;
		CountSendEnter = 401 - pEnterSend;
		OverwriteState=1;
		if(!fStartRead)
		{
			pLastEvent = 401 - pEnterSend;
			//fStartRead = 1;
		}        
		if(CountSendEnter == 0)
		{  
			OverwriteState = 0;
			pEnterSend = 1;
			tAddr = eSETSTART + eOVERWRITE;
			Save_Byte_Data(tAddr, 0); 
			CountSendEnter = pEnterSave - pEnterSend;
		}    
	}    

	//UART_STRING("\r\n pEnterSave = ");UART_H2A((BYTE)(pEnterSave >> 8));UART_H2A((BYTE)(pEnterSave));
	//UART_STRING("\r\n pEnterSend = ");UART_H2A((BYTE)(pEnterSend >> 8));UART_H2A((BYTE)(pEnterSend));
	//UART_STRING("\r\n pLastEvent = ");UART_H2A((BYTE)(pLastEvent >> 8));UART_H2A((BYTE)(pLastEvent));
	//UART_STRING("\r\n CountSendEnter = ");UART_H2A((BYTE)(CountSendEnter >> 8));UART_H2A((BYTE)(CountSendEnter));

	if(CountSendEnter == 0) //
	{  
		fStartRead = 0;

		tx_data[length++] = STX_ACU;
		tx_data[length++] = RxData[1];
		tx_data[length++] = BusAddress;
		length++;
		tx_data[length++] = ucCommand;
		tx_data[length++] = ucSub;
		tx_data[length++] = ACK;
		tx_data[length++] = 0;		//Hardware Address
		
		tx_data[length++] = 0x00;	//Event Number
		tx_data[length++] = 0x00;
		tx_data[length++] = 0x00;	//Last Event Number
		tx_data[length++] = 0x00;

		//for(i=0;i<30;i++) tx_data[length+i] = 0;

		tx_data[3] = length+2;
		//uiCRC = CrcCompute(&tx_data[0], length);
		//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
		//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
		//tx_data[length++] = CRC_High;
		tx_data[length++] = GetChecksum(&tx_data[0],length);
		tx_data[length++] = ETX_ACU;  

		UART0_TX_EN;    
		UART0_PUT_STRING(tx_data,length); 
		delay_ms(5);
		UART0_RX_EN;

		//for(i=0;i<44;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
		//UART_STRING("\r\n TX Data = ");for(i=0;i<TRH_NFCTxLen;i++) UART_H2A(tx_data0[i]);
	}
	else
	{       
		tx_data[length++] = STX_ACU;
		tx_data[length++] = RxData[1];
		tx_data[length++] = BusAddress;
		length++;
		tx_data[length++] = ucCommand;
		tx_data[length++] = ucSub;
		tx_data[length++] = 0;        //Hardware Address

		tx_data[length++] = (BYTE)(CountSendEnter >> 8);
		tx_data[length++] = (BYTE)(CountSendEnter & 0x00FF);
		tx_data[length++] = (BYTE)(pLastEvent >> 8);
		tx_data[length++] = (BYTE)(pLastEvent & 0x00FF);
		
		//tAddress = RxData[6];
		//tAddress = (tAddress << 8);
		//tAddress |= RxData[7]; 
		tAddress = 0x0000;//tAddress-1;

		ckReadAddr = (pEnterSend + tAddress) * 32; 
		ReadExtMemoryPageData(ckReadAddr, 30, &tx_data[length]);
		length = length + 30;  

		pEnterSend++;   
		if(pEnterSave <= pEnterSend )
		{
			fStartRead = 0;
			//pEnterSend = pEnterSend ;
		}      
	
		tx_data[3] = length+2;
		//uiCRC = CrcCompute(&tx_data[0], length);
		//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
		//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
		//tx_data[length++] = CRC_High;
		tx_data[length++] = GetChecksum(&tx_data[0],length);
		tx_data[length++] = ETX_ACU;  

		UART0_TX_EN;    
		UART0_PUT_STRING(tx_data,length); 
		delay_ms(10);
		UART0_RX_EN;      
	}
	if(pEnterSend>400)
		{
			OverwriteState = 0;
	            pEnterSend = 1;
		     tAddr = eSETSTART + eENTERSEND;
	            Save_Word_Data(tAddr, pEnterSend); 
	            tAddr = eSETSTART + eOVERWRITE;
	            Save_Byte_Data(tAddr, 0); 
		}
}
*/

void RequestEventDataSet_On(void)
{
	BYTE i,cnt,ucCommand,ucSub;
    BYTE CRC_Low, CRC_High;
	WORD TxCount;
    //BYTE tPage;
    WORD uiCRC, ckReadAddr, tAddr, tAddress;
	BYTE length=0;
	ucCommand = RxData0[4];
	ucSub = RxData0[5];
   
    //UART_STRING("\r\n OverwriteState = ");UART_H2A(OverwriteState);
    //UART_STRING("\r\n fStartRead = ");UART_H2A(fStartRead);
    
    

  	 tAddress = RxData0[6];
        tAddress = (tAddress << 8);
        tAddress |= RxData0[7]; 
        tAddress = tAddress;
	
    
    
			
				TxData0[length++] = STX_ACU;
				TxData0[length++] = RxData0[1];
				TxData0[length++] = BusAddress;
				length++;
				TxData0[length++] = ucCommand;
				TxData0[length++] = ucSub;
				TxData0[length++] = ACK;
				
				TxData0[length++] = RxData0[6];	//Event Number
				TxData0[length++] = RxData0[7];
				TxData0[length++] = RxData0[6];	//Last Event Number
				TxData0[length++] = RxData0[7];

				//for(i=0;i<30;i++) tx_data[length+i] = 0;

				
				
				ckReadAddr=tAddress*32;
				ReadExtMemoryPageData(ckReadAddr, 30, &TxData0[length]);
				length=length+30;
				//uiCRC = CrcCompute(&tx_data[0], length);
				//CRC_Low = (BYTE)uiCRC;                // crc Low Byte
				//CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
				//tx_data[length++] = CRC_High;
				TxData0[3] = length+2;
				TxData0[length++] = GetChecksum(&TxData0[0],length);
				TxData0[length++] = ETX_ACU;  

				UART0_TX_EN;    
				UART0_PUT_STRING(TxData0,length); 
				delay_ms(5);
				UART0_RX_EN;
			
	

	  
}  

void Request_EnterData_On(void)
{
    BYTE cCommand, cSub;

    cCommand = RxData0[4];
    cSub = RxData0[5];
    pEnterSave=Read_Word_Data(eSETSTART+eENTERSAVE);
    pEnterSend=Read_Word_Data(eSETSTART+eENTERSEND);
    OverwriteState=Read_Byte_Data(eSETSTART+eOVERWRITE);
     
    switch(cSub)
    {
        case SUB_REQ_ENTER_DATA:
            RequestEventData_On();            
            break;
            
        case SUB_REQ_ENTER_SEL:
            RequestEventDataSet_On(); 
            break;
            
        default:
            break;
    }      
   Save_Word_Data(eSETSTART+eENTERSAVE, pEnterSave);
   Save_Word_Data(eSETSTART+eENTERSEND, pEnterSend);
   Save_Byte_Data(eSETSTART+eOVERWRITE, OverwriteState);	
}

/*
void SetCardSector_On(void)
{
	BYTE sector_temp;
	BYTE cCommand, cSub;

	cCommand = RxData[5];
	cSub = RxData[6];

	sector_temp = RxData[7];
	Mifare_Set_Addr = sector_temp;
	Save_Byte_Data(eSETSTART + eMifareAddr,sector_temp);
	delay_ms(6);  
	Save_Byte_Data(eSETSTART + eMifareAddrSet,1);
	delay_ms(6);  
        WDT =~ WDT;
	TransmitServerAck_On(cCommand, cSub, ACK);
}

void SetCardSectorKey_On(void)
{
	BYTE temp;
	BYTE i;
	BYTE cCommand, cSub,tAddr;

	cCommand = RxData[5];
	cSub = RxData[6];

	Save_Byte_Data(eSETSTART + eMifareSectorKeySet,1);
	delay_ms(6);
	for(i=0;i<6;i++)
	{
		temp=RxData[7+i];
		Set_Sectorkey[i] = temp;
		tAddr = eSETSTART + eMifareSectorKey +i;
		Save_Byte_Data(tAddr, temp);
		delay_ms(6);
        	WDT =~ WDT;
	}
	TransmitServerAck_On(cCommand, cSub, ACK);
}
*/

//2020.11.17 김태성 사원 수정, 우이동 ACU 연동 함수
void Request_ACU_Control(void)
{
	BYTE ucCommand, ucSub, tByteBuf,i;
	WORD tAddr;

	pEnterSave=Read_Word_Data(eSETSTART+eENTERSAVE);
	
	ucCommand = RxData0[4];
    	ucSub = RxData0[5];

       if(ACU_DATA_CARD == 1) RequestACUEventData();
	else   ACUTransmitServerAck(ucCommand, ucSub, ACK);		


}

//2020.11.17 김태성 사원 수정, 우이동 ACU 연동 변수 
void ACUTransmitServerAck(BYTE ucCommand, BYTE ucSub, BYTE ucAcknak)
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;

    TxData0[0] = STX_ACU;
    TxData0[1] = RxData0[1];
    TxData0[2] = RxData0[2];
    TxData0[3] = 9;
    TxData0[4] = ucCommand;  
    TxData0[5] = 0x00;
    TxData0[6] = ucAcknak;
	
    TxData0[7] = GetChecksum(&TxData0[0],7);
    TxData0[8] = ETX_ACU;
	
    delay_ms(2);
    UART0_TX;    
    UART0_PUT_STRING(TxData0, 9); 
    delay_ms(2);
    UART0_RX;    

}

//2020.11.17 김태성 사원 수정, 우이동 ACU 연동 함수
void RequestACUEventData(void)
{
	BYTE i;
	BYTE ucCommand,ucSub;
	BYTE length=0;
	ucCommand = RxData0[4];
	ucSub = RxData0[5];

	TxData0[length++] = STX_ACU;
	TxData0[length++] = RxData0[1];
	TxData0[length++] = RxData0[2];
	TxData0[length++] = 0x26;
	TxData0[length++] = RxData0[4];
	TxData0[length++] = RxData0[5];
	for(i=0; i<30; i++) 
		TxData0[length++] = SaveBuf[i];
	TxData0[length++] = GetChecksum(&TxData0[0],length);
	TxData0[length++] = ETX_ACU;  

	UART0_TX_EN;    
	UART0_PUT_STRING(TxData0,length); 
	delay_ms(5);
	UART0_RX_EN;

	
	ACU_DATA_CARD = 0;
}


void UART_OnlineRcvDataChk(void)
{
	BYTE cCommand = 0, cSub;
	cCommand = RxData0[4];
	cSub = RxData0[5];
	/*
    UART0_TX_EN;    
	//UART0_PUT_BYTE(cCommand);
    UART0_PUT_STRING(RxData0, 20); 
    delay_ms(6);
    UART0_RX_EN; 
	*/
	switch(cCommand)
	{
		case SET_PARAMETER_ON :					//0xA1
			SetParameter_On();
			break;
		/* //2020.11.12 김태성 사원 수정, 사용 안함
		case SET_STAFF_GROUP_MASK_ON :			//0xA2
			SetStaffGroup_On();
			break;
		*/
		case SET_GUEST_WORKTIME_ON :		//0xA4 //2020.11.12 김태성 사원 수정
			SetGuestWorkTime_On();
			break;

		case SET_INITIALIZE_ON:					//0xA5	-	Busy Flag
			SetInitialize_On();
			// MFT_SetInitCurrentTime();
			break;

		case SET_SPECIAL_AREA_ON:				//0xA6			
			SetSpecialAreaNumber_On();
			break;

		case SET_STAFF_WORKTIME_ON:   		//0xA7	-	Busy Flag //2020.11.12 김태성 사원 수정
			SetStaffWorkTime_On();
			break;   

		case SET_GUEST_CKOUTTIME_ON:			//0xA8	-	Busy Flag //2020.11.12 김태성 사원 수정
			SetGuestCKOutTime_On();
			break;             
		/* //2020.11.12 김태성 사원 수정, 사용 안함
		case SET_GROUP_NUMBER_ON:				//0xAA
			SetGroupNumber_On();
			break;

		case SET_GROUP_SECURITY_ON:				//0xAB
			SetGroupSecurity_On();	
			break;
			
		case EXTEND_CHECKOUTDATE_ON:			//0xAC
			Extend_CheckoutDate_On();
			break;
		*/	
		case SET_CURRENT_TIME_ON:				//0xB3
			SetCurrentTime_On();
			break;
		/* //2020.11.19 김태성 사원 수정, 사용 안함
		case REQ_ONLINECHECK_ON:				//0xC0 
			RequestOnLineCheck_On();
			break;
		*/
		case REQ_INFORMATION_ON:				//0xC1
			RequestInformation_On();
			break;

		case REQ_GUEST_WORKOUTTIME_ON:		//0xC2 //2020.11.12 김태성 사원 수정
			RequestGuestWorkTime_On();
			break;

		case REQ_EVENTS_ON:						//0xC3
			Request_EnterData_On();
			//RequestEventData_On();
			//RequestEventDataSet_On();
			break;

		case REQ_CURRENT_TIME_ON :				//0xC4
			RequestCurrentTime_On();
			break;

		case REQ_PARAMETER_ON :					//0xC5
			RequestReaderParam_On();
			break;
		
		case SET_STAFF_SECURITY_ON :				//0xC6
			SetSecurityNumber_On();
			break;
		/* //2020.11.12 김태성 사원 수정, 사용 안함
		case SET_ALL_STAFF_SECURITY_ON:  		//0xC7
			SetAllStaffSecurity_On();
			break;
		*/
		case REQ_STATUS_ON:					//0xCA
			RequestReaderStatus_On();
			break;

		case REQ_ACU_FALLING:					//0xE1 우이동 ACU 연동 커멘드
			Request_ACU_Control();
			break;

		default:
			break;
	}  
}

void Check_Online_Data(void)
{
	BYTE temp;
	if(NewData)
	{
		NewData = 0;
		//UART_OnlineRcvDataChk();  // 21.02.23 hy 수정 (address check)
		if(UART_RcvDataChkSum())
		{
			UART_OnlineRcvDataChk();
		}
		
	}  
} 
