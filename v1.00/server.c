
#include "common.h"

BYTE TxData0[250];
WORD TxCount0;
BYTE FloorArray[64];
BYTE PublicArray[3];
volatile BYTE FloorTime, CallTime,RLedTime;
volatile BYTE ConAddr;
volatile WORD CountSendEnter;
volatile BYTE SpAreaNo;
volatile WORD pEnterSend, pEnterSelect;
volatile BYTE RE_flagm,RE_tmp,RE_Com,RE_SCom,RE_mode,RE_flag;
volatile WORD RE_SA,RE_EA,RE_cnt,RE_ecnt,RE_index;

void TransmitServerAck(BYTE ucCommand, BYTE ucSub, BYTE ucAcknak)
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;

    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;
    TxData0[3] = 10;
    TxData0[4] = ucCommand;  
    TxData0[5] = ucSub;
    TxData0[6] = ucAcknak;
    
    uiCRC = CrcCompute(&TxData0[0], 7);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[7] = CRC_High;
    TxData0[8] = CRC_Low;
    TxData0[9] = ETX_ACU; 
    
    delay_ms(2);
    UART0_TX;    
    UART0_PUT_STRING(TxData0, 10); 
    delay_ms(2);
    UART0_RX;    
}

void SendEnterData(BYTE ucCommand, BYTE ucSub)
{
    //BYTE i;
    //BYTE CRC_Low, CRC_High;
    //BYTE tPage;
    //WORD uiCRC;
	WORD ckReadAddr, tAddr;
	
    if(OverwriteState == 0) CountSendEnter = pEnterSave - pEnterSend;
    
    else  // Overwrite state
    {  
        if(pEnterSave >= pEnterSend)
		{
		 	pEnterSend = pEnterSave;
        	tAddr = eSETSTART + eENTERSEND;
        	Save_Word_Data(tAddr, pEnterSend);  
		}
        CountSendEnter = 400 - pEnterSend;
        if(CountSendEnter == 0)
        {  
            OverwriteState = 0;
            pEnterSend = 0;
        	tAddr = eSETSTART + eENTERSEND;
        	Save_Word_Data(tAddr, pEnterSend);  
			delay_ms(2);
            tAddr = eSETSTART + eOVERWRITE;
            Save_Word_Data(tAddr, 0); 
            CountSendEnter = pEnterSave - pEnterSend;
        }    
    }    
    
    if(CountSendEnter == 0) //
    {  
        TxData0[0] = STX_ACU;
        TxData0[1] = 0;
        TxData0[2] = 0;
        TxData0[3] = 11;
        TxData0[4] = ucCommand;
        TxData0[5] = ucSub;
        TxData0[6] = ACK;
        TxData0[7] = 0;
        TxData0[8] = 0;
        TxData0[9] = 0;
        TxData0[10] = ETX_ACU;  
    
        UART0_TX;    
        UART0_PUT_STRING(TxData0, 11); 
        delay_ms(5);
        UART0_RX;
    }
    
    else
    {
        tx_data[0] = STX_ACU;
        tx_data[1] = 0;
        tx_data[2] = 0;
        tx_data[3] = 43; 
        tx_data[4] = ucCommand;
        tx_data[5] = ucSub;                // sub command
        tx_data[6] = ACK;
        tx_data[7] = 1;                 // Number of Data
        
        TxCount0 = 8;
        
        ckReadAddr = pEnterSend * 32;
        ReadExtMemoryPageData(ckReadAddr, 32, &tx_data[TxCount0]);
        TxCount0 += 32;
        
        tx_data[TxCount0++] = 0;
        tx_data[TxCount0++] = 0;
        tx_data[TxCount0++] = ETX_ACU; 
        
        UART0_TX_EN; 
        pEnterSend++; 

        tAddr = eSETSTART + eENTERSEND;
        Save_Word_Data(tAddr, pEnterSend);   

        UART0_PUT_STRING(tx_data, TxCount0); 
        delay_ms(5);
        UART0_RX_EN;     
    }
}

void SendEnterDataSelect(BYTE ucCommand, BYTE ucSub)
{
    //BYTE i;
    //BYTE CRC_Low, CRC_High;
    //BYTE tPage;
    WORD tStartAddr, tEndAddr;
    WORD ckReadAddr;    
    
    tStartAddr = RxData0[6];
    tStartAddr = (tStartAddr << 8);
    tStartAddr |= RxData0[7];
    tEndAddr = RxData0[8];
    tEndAddr = (tEndAddr << 8);
    tEndAddr |= RxData0[9];    

    if(RxData0[5] == 0) pEnterSelect = tStartAddr;
    if(tEndAddr < pEnterSelect) //
    {  
        tx_data[0] = STX_ACU;
        tx_data[1] = 0;
        tx_data[2] = 0;
        tx_data[3] = 11;
        tx_data[4] = ucCommand;
        tx_data[5] = ucSub;
        tx_data[6] = ACK;
        tx_data[7] = 0;

        //uiCRC = CrcCompute(&tx_data[0], 8);
        //CRC_Low = (BYTE)uiCRC;                // crc Low Byte
        //CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
        tx_data[8] = 0;
        tx_data[9] = 0;
        tx_data[10] = ETX_ACU;  
    
        UART0_TX_EN;    
        UART0_PUT_STRING(tx_data, 11); 
        delay_ms(5);
        UART0_RX_EN;
    }
    
    else 
    {
        tx_data[0] = STX_ACU;
        tx_data[1] = 0;
        tx_data[2] = 0;
        tx_data[3] = 43; 
        tx_data[4] = ucCommand;
        tx_data[5] = ucSub;                // sub command
        tx_data[6] = ACK;
        tx_data[7] = 1;
        TxCount0 = 8;
        /*
        if(pEnterSelect >= 2000)
        {  
            tPage = HighPage;
            ckReadAddr = (pEnterSelect - 2000) * 32;
        }    
        else
        {  
            tPage = LowPage;
            ckReadAddr = pEnterSelect * 32;
        }  
        */
        //ckReadAddr = pEnterSend * 32;
        ckReadAddr = pEnterSelect * 32; 
        ReadExtMemoryPageData(ckReadAddr, 32, &tx_data[TxCount0]);
        //ReadExtMemoryPageData_1M(tPage, ckReadAddr, 29, &tx_data0[TxCount0]);
        TxCount0 = TxCount0 + 32;
        
            
        //uiCRC = CrcCompute(&tx_data[0], TxCount0-1);
        //CRC_Low = (BYTE)uiCRC;                // crc Low Byte
        //CRC_High = (BYTE)(uiCRC >> 8);         // crc Hign Byte
        tx_data[TxCount0++] = 0;
        tx_data[TxCount0++] = 0;
        tx_data[TxCount0++] = ETX_ACU; 
        
        pEnterSelect++;
        
        UART0_TX_EN;    
        UART0_PUT_STRING(tx_data, TxCount0); 
        delay_ms(5);
        UART0_RX_EN;        
    }           
} 

void SendEnterByteData(BYTE ucCommand)
{
    BYTE CRC_Low, CRC_High;
    BYTE FloorBuf, CallBuf;
    WORD uiCRC;

    FloorBuf = Read_Byte_Data(RxData0[2]);    
    CallBuf = Read_Byte_Data(RxData0[2] + 1);
      
    UART0_TX;
    
    TxData0[0] = STX;
    TxData0[1] = ucCommand;
    
    TxData0[2] = FloorBuf;
    TxData0[3] = CallBuf;
    uiCRC = CrcCompute(&TxData0[1], 3);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[4] = CRC_High;
    TxData0[5] = CRC_Low;
    TxData0[6] = ETX; 

    UART0_PUT_STRING(TxData0, 7);
    delay_ms(2);
    UART0_RX;    
}

void SendEnterDataSet(BYTE ucCommand, BYTE ucSub)
{
    BYTE tDataLow, tDataHigh;
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;
    WORD tEnterSave;
    
    tEnterSave = pEnterSave;
    //BufHotelcode = eeprom_read_word(nHOTELCODE);
    tDataLow = (BYTE)tEnterSave;           // crc Low Byte
    tDataHigh = (BYTE)(tEnterSave >> 8);   // crc Hign Byte  
  
   	TxData0[0] = STX_ACU;
    TxData0[1] = ucCommand;
    TxData0[2] = tDataLow;
    TxData0[3] = tDataHigh;
    TxData0[4] = 0x00;

    uiCRC = CrcCompute(&TxData0[0], 5);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[5] = CRC_High;
    TxData0[6] = CRC_Low;
    TxData0[7] = ETX_ACU; 
    
  	UART0_TX;    
    UART0_PUT_STRING(TxData0, 8); 
    delay_ms(5);
    UART0_RX;    
}  

void RequestSpecialArea()
{
    BYTE CRC_Low, CRC_High, BufArea;
    WORD uiCRC;
    
    //Rtc_Read_Data();
    BufArea = Read_Byte_Data(eSETSTART + eSpecialArea);
    if(BufArea == 0xff) BufArea = 0;
    UART0_TX;
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;
    TxData0[3] = 11;     // Length
    TxData0[4] = REQ_SET_CODE;
    TxData0[5] = SUB_REQ_AREA_SET;
    TxData0[6] = ACK; 
    TxData0[7] = BufArea;
    
    uiCRC = CrcCompute(&TxData0[0], 8);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[8] = CRC_High;
    TxData0[9] = CRC_Low;
    TxData0[10] = ETX_ACU; 

    UART0_PUT_STRING(TxData0, 11);
    delay_ms(2);
    UART0_RX;    
}

void RequestElevatorFloor(void)
{
    BYTE CRC_Low, CRC_High;
    BYTE i, tCount;
    BYTE tCommand, tSub;
    WORD uiCRC;  

    tCommand = RxData0[4];    
    tSub = RxData0[5]; 
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;
    TxData0[4] = tCommand;
    TxData0[5] = tSub;
    TxData0[6] = ACK;
    tCount = 7;
    
    //tAddr = eSETSTART + eFLOOR; 
    //tx_data0[7] = Read_Byte_Data(tAddr);
    for(i=0; i<64; i++)
    {
        //tx_data0[7+i] = Read_Byte_Data(tAddr + i);
        //TxData0[7+i] = Read_Byte_Data(tAddr + i);
        tCount++;
    }
    
    //tx_data0[tCount++] = eeprom_read_byte(nPublic01);
    //tx_data0[tCount++] = eeprom_read_byte(nPublic02);
    
    //tAddr = eSETSTART + ePUBLIC;
    for(i=0; i<4; i++)
    {
        //tx_data0[71+i] = Read_Byte_Data(tAddr + i);
        //TxData0[71+i] = Read_Byte_Data(tAddr + i);
        tCount++;
    }    
    
    uiCRC = CrcCompute(&TxData0[0], TxCount0-1);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >> 8);         // crc Hign Byte
    TxData0[tCount++] = CRC_High;
    TxData0[tCount++] = CRC_Low;
    TxData0[tCount++] = ETX_ACU; 
    TxData0[3] = tCount;  
    
    UART0_TX;    
    UART0_PUT_STRING(TxData0, tCount); 
    delay_ms(5);
    UART0_RX;      
}  

void RequestRealTime()
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;
    
    Rtc_Read_Data();
    UART0_TX;
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;
    TxData0[3] = 17;     // Length
    TxData0[4] = REQ_SET_CODE;
    TxData0[5] = SUB_REQ_CURRENT_TIME;
    TxData0[6] = ACK;
    
    TxData0[7] = rtc_week;
    TxData0[8] = rtc_year;
    TxData0[9] = rtc_month;
    TxData0[10] = rtc_date;
    TxData0[11] = rtc_hour;
    TxData0[12] = rtc_min;
    TxData0[13] = rtc_sec;
    uiCRC = CrcCompute(&TxData0[0], 14);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[14] = CRC_High;
    TxData0[15] = CRC_Low;
    TxData0[16] = ETX_ACU; 

    UART0_PUT_STRING(TxData0, 17);
    delay_ms(2);
    UART0_RX;    
}

void RequestRelayContactTime(void)
{
    BYTE CRC_Low, CRC_High;
    BYTE FloorBuf, CallBuf;
    BYTE tCommand, tSub;
    WORD uiCRC;

    tCommand = RxData0[4];    
    tSub = RxData0[5];
    
   // FloorBuf = e2prom_read_byte(nFLOORTIME);    // crc Hign Byte
    //CallBuf = e2prom_read_byte(nCALLTIME);     // crc Low Byte
    FloorBuf = Read_Byte_Data(eSETSTART + eAUTOTIME);

    CallBuf = Read_Byte_Data(eSETSTART + eRelayGreenLED);
    CallBuf = CallBuf * 10;  
    
    UART0_TX;
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;
    TxData0[3] = 12;
    TxData0[4] = tCommand;
    TxData0[5] = tSub;
    TxData0[6] = ACK;
    
    if(FloorBuf == 0xff) TxData0[7] = 5;
    else TxData0[7] = FloorBuf;
    if(CallBuf == 0xff) TxData0[8] = 40;
    else TxData0[8] = CallBuf;
    
    uiCRC = CrcCompute(&TxData0[0], 9);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[9] = CRC_High;
    TxData0[10] = CRC_Low;
    TxData0[11] = ETX_ACU; 

    UART0_PUT_STRING(TxData0, 12);
    delay_ms(2);
    UART0_RX;    
}

void RequestHotelCode(void)
{
    BYTE CRC_Low, CRC_High;
    BYTE Hotel_High, Hotel_Low;
    WORD uiCRC, BufHotelcode;   
    BYTE tCommand, tSub;
    
    tCommand = RxData0[4];    
    tSub = RxData0[5];    
    
    BufHotelcode = Read_Word_Data(eSETSTART + eHOTELCODE);
    //BufHotelcode = HOTEL_CODE_SET;
	//BufHotelcode = e2prom_read_word(nHOTELCODE);
    Hotel_Low = (BYTE)BufHotelcode;           // crc Low Byte
    Hotel_High = (BYTE)(BufHotelcode >> 8);   // crc Hign Byte
    
    UART0_TX;
    
    TxData0[0] = STX_ACU;
    TxData0[1] = 0;
    TxData0[2] = 0;

    TxData0[3] = 10;
    TxData0[4] = tCommand;
    TxData0[5] = tSub;
    
    TxData0[6] = Hotel_High;
    TxData0[7] = Hotel_Low;
    
    uiCRC = CrcCompute(&TxData0[0], 9);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    TxData0[8] = CRC_High;
    TxData0[9] = CRC_Low;
    TxData0[10] = ETX_ACU; 

    UART0_PUT_STRING(TxData0, 12);
    delay_ms(2);
    UART0_RX;


   
    tx_data0[8] = GetChecksum(&tx_data0[0],8);
    tx_data0[9] = ETX_ACU;    
}


void SetCurrentTime(void)
{
    BYTE ucCommand, ucSub;
    //BYTE tYear, temp;

    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    //====== Messerschmitt 발급 시스템은 연도가 16년 단위로 반복 ======//
    //temp = RxData0[7];
    //if((temp >= 12) && (temp > 28)) tYear = temp - 12;  // 2012 ~ 2027
    //else if((temp >= 28) && (temp > 44)) tYear = temp - 28;  // 2028 ~ 2043
    
    rtc_week  = RxData0[6];
    rtc_year  = RxData0[7];
    rtc_month = RxData0[8];
    rtc_date  = RxData0[9];  
   
    rtc_hour  = RxData0[10];
    rtc_min   = RxData0[11];
    rtc_sec   = RxData0[12];
    
    Rtc_Write_Data();
    ResetState |= 0x02;        

    TransmitServerAck(ucCommand, ucSub, ACK);
}  

void SetRelayContactTime(void)
{
    BYTE ucCommand, ucSub;
    BYTE BufCall, BufFloor;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    BufFloor = RxData0[6];
    BufCall = RxData0[7];
    
    if(!(BufFloor == 0))
    {
        FloorTime = BufFloor * 0.1;
		if(FloorTime==0)
			FloorTime=1;
        //e2prom_write_byte(nFLOORTIME,BufFloor);
        tAddr = eSETSTART + eAUTOTIME;
        Save_Byte_Data(tAddr, FloorTime);
        delay_ms(6);
    } 
    
    if(!(BufCall == 0))
    {
		CallTime = BufCall * 0.1;
		if(CallTime==0)
			CallTime=4;
        tAddr = eSETSTART + +eRelayGreenLED;;
        Save_Byte_Data(tAddr, CallTime);
        //e2prom_write_byte(nCALLTIME,BufCall);
        delay_ms(6);
    }  
    
    TransmitServerAck(ucCommand, ucSub, ACK);
}  

void SetHotelCode(void)
{
    WORD tHotelCode,tAddr;
    BYTE ucCommand, ucSub;
    
    tHotelCode = RxData0[6];
    tHotelCode = (tHotelCode << 8);
    tHotelCode |= RxData0[7];    
    uiHotelCode = tHotelCode;
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    tAddr = eSETSTART + eHOTELCODE;
    //e2prom_write_word(nHOTELCODE,uiHotelCode);
    Save_Word_Data(tAddr, uiHotelCode);
	
    delay_ms(1);
    TransmitServerAck(ucCommand, ucSub, ACK);
}

void SetAreaNumber(void)
{
    BYTE ucCommand, ucSub;
    BYTE BufArea;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    BufArea = RxData0[6];
    SpAreaNo = BufArea;
    tAddr = eSETSTART + eSpecialArea;
 
    Save_Byte_Data(tAddr, BufArea);
    delay_ms(6);
    
    TransmitServerAck(ucCommand, ucSub, ACK);  
}  

void RequestSetCode(void)
{
    BYTE temp;
    
    temp = RxData0[5];
    
    switch(temp)
    {
        case SUB_REQ_HOTEL_CODE:
            RequestHotelCode();
            break;
            
        case SUB_REQ_CONTACT_TIME:
            RequestRelayContactTime();
            break;
            
        case SUB_REQ_CURRENT_TIME:
            RequestRealTime();
            break;  
            
        case SUB_REQ_FLOOR_SET:
            RequestElevatorFloor();
            break; 
        
        case SUB_REQ_AREA_SET:
            RequestSpecialArea();
            break;          
            
        default:
            break;
    }   
}

void SetDefaultCode(void)
{
    BYTE temp;
    
    temp = RxData0[5];
    
    switch(temp)
    {
        case SET_HOTEL_CODE:
            SetHotelCode();
            break;
            
        case SET_CONTACT_TIME:
            SetRelayContactTime();
            break;
            
        default:
            break;
    }         
}  



void UART0_RcvDataChk(void)
{
    BYTE cCommand = 0, cSub;
	//BYTE i=0;
    cCommand = RxData0[4];
    cSub = RxData0[5];
	
    switch(cCommand)
    {
        //case SET_TIMECODE:
        //    SetTimeRegion(cCommand, cSub);
        //    brea
        case SET_DEFAULT:
            SetDefaultCode();
            break;
		
        case SET_FLOOR:
           // SetElevatorFloor();
            break;   

        case SET_CURRENT_TIME:
            SetCurrentTime();
            break;             

        case REQ_SET_CODE:
            RequestSetCode();
            break;   
		
        case REQ_ENTER_DATA:
            SendEnterData(cCommand, cSub);            
            break;
		
        case REQ_ENTER_DATA_SELECT:
            SendEnterDataSelect(cCommand, cSub);            
            break;            
          
        case REQ_ENTER_SET:
            SendEnterDataSet(cCommand, cSub); 
            break;
            
        case 0xC2:
            SendEnterByteData(cCommand);            
            break; 

        case SET_SPEACIAL_AREA:
            SetAreaNumber();
			break;
        
        default:
            break;
    } 
	
}

void Check_Main_Data(void)
{
    if(NewData)
    {
        NewData= 0;
        UART0_RcvDataChk();
    }  
} 
void TransmitNServerAck(BYTE ucCommand, BYTE ucSub, BYTE ucAcknak)
{
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    BYTE i;

    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 9;
    tx_data0[4] = ucCommand;  
    tx_data0[5] = ucSub;
    tx_data0[6] = ucAcknak;
	
    tx_data0[7] = GetChecksum(&tx_data0[0],7);
    tx_data0[8] = ETX_ACU;

    for(i=0;i<9;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}    
void CheckMobileKey(void)
{
    BYTE i;
    
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    
    //fMobileKey = 1;
    for(i=0;i<32;i++) MyBuf[i] = RxData0[6+i];
    CheckCardClass();
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;            
    tx_data0[3] = 11; 		// Packet Length
    tx_data0[4] = REQ_CARD_DATA;
    tx_data0[5] = 0x01;
    tx_data0[6] = 0x01;
    tx_data0[7] = SaveBuf[2];         // Result  
    tx_data0[8] = SaveBuf[3];         // Error Code
	
    tx_data0[9] = GetChecksum(&tx_data0[0], 9);
    tx_data0[10] = ETX_ACU;
    

    for(i=0;i<11;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];

}

void SetNInitialize(void)
{
	WORD tAddr, tBuf = 0;
	BYTE tByteBuf, i;
	float fTemp;

	tBuf = RxData0[6];
	tBuf = (tBuf << 8);
	tBuf |= RxData0[7];  
	tAddr = eSETSTART + eRoomNumber;
	Save_Word_Data(tAddr, tBuf);                          // Room Number
	delay_ms(6);
    

	BusAddress = RxData0[9];
	tAddr = eSETSTART + eBusAddress;
	Save_Byte_Data(tAddr, BusAddress);                          // Bus Address
	delay_ms(6);  
    
	tByteBuf = RxData0[10];
	SpAreaNo = tByteBuf;
	Save_Byte_Data(eSETSTART + eSpecialArea, tByteBuf);     // Special Area Number
	delay_ms(6);

	tByteBuf = RxData0[11];
	Save_Byte_Data(eSETSTART + eMaidArea, tByteBuf);      // Maid Area  
	delay_ms(6);    

	tByteBuf = RxData0[12];
	Save_Byte_Data(eSETSTART + eSuiteArea, tByteBuf);     // Suite Area
	delay_ms(6);

	tByteBuf = RxData0[13];
	Save_Byte_Data(eSETSTART + eMasterArea, tByteBuf);    // Master Area  
	delay_ms(6);

	tByteBuf = RxData0[14];
	Save_Byte_Data(eSETSTART + eMiniArea, tByteBuf);      // Minibar Area 
	delay_ms(3);

	//tBuf = RxData0[15];
	tBuf = 0xFFFF;//(tBuf << 8);
	//tBuf |= RxData0[16];  
	tAddr = eSETSTART + eGuestSecurity;
	Save_Word_Data(tAddr,tBuf);  // Guest security
	delay_ms(6);

	//tBuf = RxData0[17];
	tBuf = 0xFFFF;//(tBuf << 8);
	//tBuf |= RxData0[18];  
	tAddr = eSETSTART + eStandbySecurity;
	Save_Word_Data(tAddr,tBuf);  // Standby security
	delay_ms(6);

	//tBuf = RxData0[19];
	tBuf = 0xFFFF;//tBuf = (tBuf << 8);
	//tBuf |= RxData0[20];  
	tAddr = eSETSTART + eOnetimeSecurity;
	Save_Word_Data(tAddr,tBuf);  // Onetime security
	delay_ms(6);

	//tBuf = RxData0[21];
	tBuf = 0xFFFF;//tBuf = (tBuf << 8);
	//tBuf |= RxData0[22];  
	tAddr = eSETSTART + ePreCheckSecurity;
	Save_Word_Data(tAddr,tBuf);  // Pre-Check security
	delay_ms(6);

	//tBuf = RxData0[23];
	tBuf = 0xFFFF;//tBuf = (tBuf << 8);
	//tBuf |= RxData0[24];  
	tAddr = eSETSTART + eEmgSecurity;
	Save_Word_Data(tAddr,tBuf);  // Emergency security
	delay_ms(6);
	
	tByteBuf = RxData0[25];
	ReaderType = tByteBuf;
	tAddr = eSETSTART + eReaderType;                         
	Save_Byte_Data(tAddr, tByteBuf);   // Type of Reader
	delay_ms(6);

	for(i=0; i<64; i++)
	{  
		tByteBuf = RxData0[26 + i];
		Save_Byte_Data(eSETSTART + eCheckOutTime1 + i, tByteBuf);
		delay_ms(6);
	}

	rtc_week  = RxData0[90];
	rtc_year  = RxData0[91];
	rtc_month = RxData0[92];
	rtc_date  = RxData0[93];  

	rtc_hour  = RxData0[94];
	rtc_min   = RxData0[95];
	rtc_sec   = RxData0[96];

	Rtc_Write_Data();

	tAddr = eSETSTART + eGuestSecurity;
	Save_Page_Init(tAddr, 26, 0xff);
	delay_ms(7);

	// Grand Master Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eGMasterSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
	}

	// Master Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eMasterSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
	}  

	// Maid Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eMaidSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
	} 

	// Minibar Security Initializing
	for(i=0; i<16; i++) 
	{
		tAddr = eSETSTART + eMinibarSecurity + (64*i);
		Save_Page_Init(tAddr, 64, 0xff);
		delay_ms(7);
	}                  
	
	TransmitNServerAck(SET_NINITIALIZE, 1, ACK); 
	//DDRD=DDRD|0x02;
	//PORTD=PORTD|0x02;
	//delay_ms(5);
	//BLE_AdPk_Set();
	//PORTD=PORTD&0xFD;
	//DDRD=DDRD&0xFD;
}

void SetNRelayContactTime(void)
{
	BYTE ucCommand, ucSub, tByteBuf;
	WORD tAddr;
    
	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	//autotime은 사용하지 않음
	
	tAddr = eSETSTART + eAUTOTIME;
	tByteBuf = RxData0[6];
	Save_Byte_Data(tAddr,tByteBuf);
	delay_ms(6);
	
	tAddr = eSETSTART +eRelayGreenLED; 
	tByteBuf = RxData0[7]; // ex)16 그대로 받음
	
	tByteBuf = tByteBuf * 0.1;
	Save_Byte_Data(tAddr,tByteBuf);
	delay_ms(6);

	TransmitNServerAck(ucCommand, ucSub, ACK);
}

void RequestNRelayContactTime(void)
{
    BYTE i;
   BYTE tByteBuf;
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 10; 	            // Packet Length	
    tx_data0[4] = REQ_NRELAY_TIME;
    tx_data0[5] = 0x01;
    
    
    tx_data0[6] = Read_Byte_Data(eSETSTART + eAUTOTIME);
	 
	tByteBuf = Read_Byte_Data(eSETSTART + eRelayGreenLED);
	if(tByteBuf==0xff)
		tByteBuf=4;
	tByteBuf = tByteBuf * 10;
    tx_data0[7] = tByteBuf;// ex)16 그대로 표현 
	
    tx_data0[8] = GetChecksum(&tx_data0[0],16);
    tx_data0[9] = ETX_ACU;

    for(i=0;i<10;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNHotelCode(void)
{
    WORD tHotelCode, tAddr;
    BYTE ucCommand, ucSub;
    
    tHotelCode = RxData0[6]; //6
    tHotelCode = (tHotelCode << 8);
    tHotelCode |= RxData0[7];   //7 
    uiHotelCode = tHotelCode;
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    tAddr = eSETSTART + eHOTELCODE;
    Save_Word_Data(tAddr, uiHotelCode);
    delay_ms(5);


    TransmitNServerAck(ucCommand, ucSub, ACK);

	delay_ms(5);
	BLE_AdPk_Set();

	
}

void RequestNHotelCode(void)
{
    BYTE Hotel_High, Hotel_Low;
    WORD  BufHotelcode;   
    BYTE tCommand, tSub;
    BYTE i;
    
    tCommand = RxData0[4];    
    tSub = RxData0[5];    
    //uiHotelCode = Read_Word_Data(eSETSTART + eHOTELCODE);
    BufHotelcode = Read_Word_Data(eSETSTART + eHOTELCODE);
    Hotel_Low = (BYTE)BufHotelcode;           // crc Low Byte
    Hotel_High = (BYTE)(BufHotelcode >> 8);   // crc Hign Byte
    
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
   
    tx_data0[3] = 10;
    tx_data0[4] = tCommand;
    tx_data0[5] = tSub;
    
    tx_data0[6] = Hotel_High;
    tx_data0[7] = Hotel_Low;
   
    tx_data0[8] = GetChecksum(&tx_data0[0],8);
    tx_data0[9] = ETX_ACU; 

    for(i=0;i<11;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNCurrentTime(void)
{
    BYTE ucCommand, ucSub;
    //BYTE tYear, temp;

    ucCommand = RxData0[4];
    ucSub = RxData0[5];

    rtc_week  = RxData0[6];
    rtc_year  = RxData0[7];
    rtc_month = RxData0[8];
    rtc_date  = RxData0[9];  
   
    rtc_hour  = RxData0[10];
    rtc_min   = RxData0[11];
    rtc_sec   = RxData0[12];
    
    Rtc_Write_Data();
    ResetState |= 0x02;        

    TransmitNServerAck(ucCommand, ucSub, ACK);
}  

void RequestNCurrentTime()
{
    BYTE i;
//    BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    
    Rtc_Read_Data();
   
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 15;     // Length
    tx_data0[4] = REQ_NCURRENT_TIME;
    tx_data0[5] = ACK;
    
    tx_data0[6] = rtc_week;
    tx_data0[7] = rtc_year;
    tx_data0[8] = rtc_month;
    tx_data0[9] = rtc_date;
    tx_data0[10] = rtc_hour;
    tx_data0[11] = rtc_min;
    tx_data0[12] = rtc_sec;

	
    tx_data0[13] = GetChecksum(&tx_data0[0],13);
    tx_data0[14] = ETX_ACU; 

    for(i=0;i<15;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNSpecialAreaNumber(void)
{
    BYTE ucCommand, ucSub;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    tAddr = eSETSTART + eSpecialArea;
    SpAreaNo = RxData0[6];
    Save_Byte_Data(tAddr, RxData0[6]); 
    delay_ms(6);
     
    TransmitNServerAck(ucCommand, ucSub, ACK);
}

void RequestNSpecialAreaNumber(void)
{
    BYTE i;
    //BYTE CRC_Low, CRC_High;
    //WORD uiCRC;
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0; 
    tx_data0[3] = 9;            // Packet Length		
    tx_data0[4] = REQ_NSPEACIAL_AREA;
    tx_data0[5] = 0x01; 
    
    tx_data0[6] = Read_Byte_Data(eSETSTART + eSpecialArea);
    
    tx_data0[7] = GetChecksum(&tx_data0[0],7);
    tx_data0[8] = ETX_ACU;

    for(i=0;i<9;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SendNEnterData(BYTE ucCommand, BYTE ucSub)
{
    BYTE i,tmp, ctmp;
    //BYTE CRC_Low, CRC_High;
    //BYTE tPage;
    //WORD uiCRC, ckReadAddr, tAddress;
	WORD tAddr;

    RE_flagm=1;
    //UART_STRING("\r\n OverwriteState = ");UART_H2A(OverwriteState);
    //UART_STRING("\r\n fStartRead = ");UART_H2A(fStartRead);
    RE_mode=0;
    TxCount0 = 0;
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
		       
		        RE_flagm=0;
		        tx_data0[0] = STX_ACU;
		        tx_data0[1] = 0;
		        tx_data0[2] = 0;
		        tx_data0[3] = 41;
		        tx_data0[4] = ucCommand;
		        tx_data0[5] = ucSub;
		        tx_data0[6] = ACK;        //Hardware Address
		        tx_data0[7] = 0;          //
		        tx_data0[8] = 0;
		        TxCount0 = 9;

		        for(i=0;i<30;i++) tx_data0[TxCount0+i] = 0;

		        tx_data0[39] = GetChecksum(&tx_data0[0],38);
		        tx_data0[40] = ETX_ACU;  
		        
		        for(i=0;i<41;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
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

void SendNEnterDataSet(BYTE ucCommand, BYTE ucSub)
{
     BYTE i,cnt,Full,ecnt;
//    BYTE CRC_Low, CRC_High;
    //BYTE tPage;
    WORD ckReadAddr, tAddr, sAddress,eAddress,ctmp;
    RE_flagm=1;
   
    //UART_STRING("\r\n OverwriteState = ");UART_H2A(OverwriteState);
    //UART_STRING("\r\n fStartRead = ");UART_H2A(fStartRead);
    RE_mode=1;
    TxCount0 = 0;
	 ecnt=1;	
	 cnt=0;
	 
  	 sAddress = RxData0[6];
        sAddress = (sAddress << 8);
        sAddress |= RxData0[7]; 
		
        eAddress = RxData0[8];
        eAddress = (eAddress << 8);
        eAddress |= RxData0[9]; 
	 
	 tAddr = eSETSTART + eFullWrite;
	 Full=Read_Byte_Data(tAddr);
	


	
	if(!Full)
		{
			if(sAddress<pEnterSave)
				{
					if(eAddress<pEnterSave)
						{
							cnt=1;
							
						}
					else
						{
							eAddress=pEnterSave-1;
							cnt=1;
						}
				}
			else
				{
					cnt=0;
				}
		}
	else
		cnt=1;
    ctmp=eAddress-sAddress+1;
    RE_cnt=ctmp;
	RE_SA=sAddress;
	RE_EA=eAddress;
	RE_Com=ucCommand;
	RE_SCom=ucSub;

    if(cnt == 0) //
    {  
	       
	        RE_flagm=0;
	        tx_data0[0] = STX_ACU;
	        tx_data0[1] = 0;
	        tx_data0[2] = 0;
	        tx_data0[3] = 41;
	        tx_data0[4] = ucCommand;
	        tx_data0[5] = ucSub;
	        tx_data0[6] = ACK;        //Hardware Address
	        tx_data0[7] = 0;          //
	        tx_data0[8] = 0;
	        TxCount0 = 9;
	        for(i=0;i<30;i++) tx_data0[TxCount0+i] = 0;
	        tx_data0[39] = GetChecksum(&tx_data0[0],38);
	        tx_data0[40] = ETX_ACU;
	        for(i=0;i<41;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
    }
	
	
}  

void RequestNEnterData(void)
{
    BYTE cCommand, cSub;
	//BYTE tmp;
    
    cCommand = RxData0[4];
    cSub = RxData0[5];
	pEnterSave=Read_Word_Data(eSETSTART+eENTERSAVE);
    pEnterSend=Read_Word_Data(eSETSTART+eENTERSEND);
    OverwriteState=Read_Byte_Data(eSETSTART+eOVERWRITE);
    switch(cSub)
    {
        case SUB_REQ_ENTER_DATA:    
           // SendEnterDataSet(cCommand, cSub);
            SendNEnterData(cCommand, cSub);        
            break;
            
        case SUB_REQ_ENTER_SEL:
           // SendEnterData(cCommand, cSub);     
           SendNEnterDataSet(cCommand, cSub);
            break;	
            
        default:
            break;
    } 
      RE_index = (RE_cnt-1)/3;
      RE_index=RE_index+3;
       RE_flag=0;
	RE_ecnt=0;
	 Save_Word_Data(eSETSTART+eENTERSAVE, pEnterSave);
	   Save_Word_Data(eSETSTART+eENTERSEND, pEnterSend);
	   Save_Byte_Data(eSETSTART+eOVERWRITE, OverwriteState);
}

void RequestNInform(void)
{
    WORD i;
    BYTE tHigh = 0, tLow = 0, tByteBuf, temp, temp1, temp2;
    WORD  tBuf = 0; 
    //float fTemp = 0.0;
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;

	
    tx_data0[3] = 245;     // Length
    tx_data0[4] = REQ_NINFOMATION;
    tx_data0[5] = 0x01;
    
    tx_data0[6] = 0x00;                // Hardware Address
   
    tByteBuf = Read_Byte_Data(eSETSTART + eReaderType); // Reader Type(1:Doorlock, 2:Reader)
    if(tByteBuf == 0xff) tByteBuf = 1;    
    tx_data0[7] = tByteBuf;   
    
    tBuf = Read_Word_Data(eSETSTART + eRoomNumber);
    if(tBuf == 0xffff) tBuf = 1;
    tHigh = (BYTE)(tBuf >> 8);   // crc Hign Byte    
    tLow = (BYTE)(tBuf);           // crc Low Byte
    tx_data0[8] = tHigh;        // Room Number
    tx_data0[9] = tLow;     
  
    
    for(i=0;i<235;i++) tx_data0[10 + i] = 0x00;

    tx_data0[10] = 0xff;          // Voltage Control
    tx_data0[11] = 0x01;             // Firmware Version 1
    tx_data0[12] = 0x00;             // Firmware Version 2
    tx_data0[13] = 0x00;             // Firmware Version 3
	
    tBuf = Read_Word_Data(eSETSTART + eHOTELCODE);
    //tBuf = eeprom_read_word(nHOTELCODE);
    tLow = (BYTE)tBuf;           // crc Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // crc Hign Byte    
    tx_data0[14] = tHigh;        // Hotelcode
    tx_data0[15] = tLow;
    tByteBuf = Read_Byte_Data(eSETSTART + eSpecialArea);
    if(tByteBuf == 0xff) tByteBuf = 99;
    tx_data0[16] = tByteBuf;            // Special Area

    //tBuf = eeprom_read_word(nEmgSecurity);                // Emergency Security
    tBuf = Read_Word_Data(eSETSTART + eEmgSecurity);
    if(tBuf == 0xffff) tBuf = 0;
    tLow = (BYTE)tBuf;           // Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
    tx_data0[17] = tHigh;          
    tx_data0[18] = tLow;                                   

     tBuf = Read_Word_Data(eSETSTART + eSystemSecurity);     // System Security
    if(tBuf == 0xffff) tBuf = 0;
    tLow = (BYTE)tBuf;           // Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // Hign Byte        
    tx_data0[19] = tHigh;                                       
    tx_data0[20] = tLow;    

    tByteBuf = Read_Byte_Data(eSETSTART + eMasterArea);   // Master Area
    if(tByteBuf == 0xff) tByteBuf = 1;
    tx_data0[35] = tByteBuf;
    tBuf = Read_Word_Data(eSETSTART + eMasterSecurity);       // Master Security
    if(tBuf == 0xffff) tBuf = 1;             
    tLow = (BYTE)tBuf;           // Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
    tx_data0[36] = tHigh;          
    tx_data0[37] = tLow;     
    
    tByteBuf = Read_Byte_Data(eSETSTART + eMaidArea); 
    if(tByteBuf == 0xff) tByteBuf = 1;
    tx_data0[38] = tByteBuf;                // Maid Area
    tBuf = Read_Word_Data(eSETSTART + eMaidSecurity);         // Maid Security
    if(tBuf == 0xffff) tBuf = 1;
    tLow = (BYTE)tBuf;           // Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
    tx_data0[39] = tHigh;          
    tx_data0[40] = tLow; 

    tByteBuf = Read_Byte_Data(eSETSTART + eMiniArea);
    if(tByteBuf == 0xff) tByteBuf = 1;
    tx_data0[41] = tByteBuf;                // Minibar Area
    tBuf = Read_Word_Data(eSETSTART + eMinibarSecurity);      // Minibar Security
    if(tBuf == 0xffff) tBuf = 1;
    tLow = (BYTE)tBuf;           // Low Byte
    tHigh = (BYTE)(tBuf >> 8);   // Hign Byte    
    tx_data0[42] = tHigh;          
    tx_data0[43] = tLow;


    tx_data0[44] = Read_Byte_Data(eSETSTART + eParameter0);     // Duration Door Open
    tx_data0[45] = Read_Byte_Data(eSETSTART + eParameter1);     // Opening Mode
    tx_data0[46] = Read_Byte_Data(eSETSTART + eParameter2);     // Daylight Saving Time
    tx_data0[47] = Read_Byte_Data(eSETSTART + eParameter3);     // Daylight Saving Time
    //tx_data0[48] = Read_Byte_Data(eSETSTART + eParameter4);     // Status

	temp1=Read_Byte_Data(eSETSTART+officeset);
	temp2=0;
	if(temp1)
	temp2=temp2+1;
	tx_data0[48] =temp2;
	
	temp = Read_Byte_Data(eSETSTART + eRelayGreenLED); // Green LED Time
	if(temp==0xff)
		temp=3;
	tx_data0[49] = temp;       

	temp = Read_Byte_Data(eSETSTART + eRedLed); // Red LED Time
	if(temp==0xff)
		temp=1;
	tx_data0[50] = temp;   
    	for(i=0;i<64;i++)
	{
		tx_data0[51+i] = Read_Byte_Data(eSETSTART + eCheckOutTime1 +i);
	}

	tx_data0[243] = GetChecksum(&tx_data0[0],243);
	tx_data0[244] = ETX_ACU; 

	for(i=0;i<245;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];

}


void SetNParameter(void)
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
    
    tByteBuf = RxData0[12]; 
    fTemp = tByteBuf * 0.25;
    CallTime = (BYTE)(fTemp);
	
    Save_Byte_Data(eSETSTART + eRelayGreenLED, CallTime);      // Green LED Time
    delay_ms(6);    
 
    tByteBuf = RxData0[13];   
    fTemp = tByteBuf  * 0.25;
    RLedTime = (BYTE)(fTemp);
	
    Save_Byte_Data(eSETSTART + eRedLed, RLedTime);        // Red LED Time
    delay_ms(6); 
    
    tByteBuf = RxData0[14];   
    Save_Byte_Data(eSETSTART + eBlinkLed, tByteBuf);      // LED Blink Time
    delay_ms(6);  

	/* //스폐셜 리더는 DND가없다
    //
    tTemp = RxData0[10];
    tTemp = (tTemp & 0x08) >> 3;       //0000 1000 (1 bit)
    tAddr = eSETSTART + officeset; 
    Save_Byte_Data(tAddr, tTemp);
    delay_ms(6);    
	*/
    //Office Mode 설정
    tTemp = RxData0[10];
    tTemp = (tTemp & 0x01);
	Office=tTemp;
    tAddr = eSETSTART + officeset;
    Save_Byte_Data(tAddr,tTemp);
    delay_ms(6);
      
    TransmitNServerAck(ucCommand, ucSub, ACK);
}

void RequestNParameter(void)
{
	BYTE i,temp1;
	//BYTE CRC_Low, CRC_High;
	WORD temp;
	//WORD uiCRC;

	tx_data0[0] = STX_ACU;
	tx_data0[1] = 0;
	tx_data0[2] = 0;
	tx_data0[3] = 17; 	            // Packet Length	
	tx_data0[4] = REQ_NPARAMETER;
	tx_data0[5] = 0x01;


	for(i=0; i<6; i++) tx_data0[6+i] = Read_Byte_Data(eSETSTART + eParameter0 + i);
	temp1 = Read_Byte_Data(eSETSTART+officeset);
	tx_data0[10] = 0;
	if(temp1)
	tx_data0[10] = tx_data0[10] + 1;

	temp = Read_Byte_Data(eSETSTART + eRelayGreenLED);
	if(temp==0xff)
		temp=4;
	tx_data0[12] = temp * 4;
	temp = Read_Byte_Data(eSETSTART + eRedLed);
	if(temp==0xff)
		temp=2;
	tx_data0[13] = temp * 4;

	tx_data0[14] =Read_Byte_Data(eSETSTART + eBlinkLed);

	tx_data0[15] = GetChecksum(&tx_data0[0],15);
	tx_data0[16] = ETX_ACU;

	for(i=0;i<17;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNBlePower(void)
{
	WORD tAddr;
	BYTE ucCommand, ucSub;
	BYTE tBlePower;

	tBlePower = RxData0[6];		//TxPower
	tBlePower = (tBlePower << 4);
	tBlePower |= RxData0[7];	//ADVInterval
	uiBlePower = tBlePower;
	ucCommand = RxData0[4];
	ucSub = RxData0[5];

	tAddr = eSETSTART + eBlePower;
	Save_Byte_Data(tAddr, uiBlePower);
	delay_ms(5);

	TransmitNServerAck(ucCommand, ucSub, ACK);
	delay_ms(5);
	BLE_AdPk_Set();
}

void RequestNBlePower(void)
{
    BYTE Ble_High, Ble_Low;
    BYTE BufBlePower;   
    BYTE tCommand, tSub;
    BYTE i;
    
    tCommand = RxData0[4];    
    tSub = RxData0[5];    
    BufBlePower = Read_Byte_Data(eSETSTART + eBlePower);
    Ble_High = (BYTE)(BufBlePower >> 4);		//TxPower
    Ble_Low = (BYTE)(BufBlePower & 0x0F) ;   //ADVInterval
    
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
   
    tx_data0[3] = 10;
    tx_data0[4] = tCommand;
    tx_data0[5] = tSub;
    
    tx_data0[6] = Ble_High;	//TxPower
    tx_data0[7] = Ble_Low;	//ADVInterval
   
    tx_data0[8] = GetChecksum(&tx_data0[0],8);
    tx_data0[9] = ETX_ACU; 

    for(i=0;i<10;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

//----------------------------------------------------------

void SetNTimeSlot(void)
{
    BYTE i;
    BYTE ucCommand, ucSub;
    BYTE temp[4];
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    for(i=0; i<4; i++)
    {
        tAddr = eSETSTART + eTimeSlotPermanent + i;
        Save_Byte_Data(tAddr, RxData0[6+i]); 
        delay_ms(6);
    }

    
    for(i=0; i<4; i++)
    {
        tAddr = eSETSTART + eTimeSlotGuest + i;
        Save_Byte_Data(tAddr, RxData0[10+i]); 
        delay_ms(6);
    }    
      
    TransmitServerAck(ucCommand, ucSub, ACK);
}

void SetNStaffTimeSlot(void)
{
    BYTE ucCommand, ucSub;
    BYTE i;
    WORD tAddr;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
	
	for(i=0; i<128; i++)
	{
		tAddr = eSETSTART + eStaffTimeSlot + i;
		Save_Byte_Data(tAddr, RxData0[6+i]); 
		delay_ms(6);
	}
    TransmitServerAck(ucCommand, ucSub, ACK);
}

void SetNGroupRoom(void)
{
	BYTE ucCommand, ucSub, tByteBuf,i,RoomCnt;
	WORD tAddr;
    
	ucCommand = RxData0[4];
	ucSub = RxData0[5];


	RoomCnt = RxData0[6];
	
	for(i=0;i<RoomCnt;i++)
	{
		tAddr = eSETSTART + eRoomGroupStart + (i*2);
		tByteBuf = RxData0[7+(i*2)];
		Save_Byte_Data(tAddr,tByteBuf);
		delay_ms(6);
		tAddr = eSETSTART + eRoomGroupStart + (i*2) + 1;
		tByteBuf = RxData0[8+(i*2)];
		Save_Byte_Data(tAddr,tByteBuf);
		delay_ms(6);
	}
	
	TransmitServerAck(ucCommand, ucSub, ACK);
}

void SetNElevatorFloor(void)
{
    BYTE ucCommand, ucSub,i;
    WORD tAddr;

    ucCommand = RxData0[4];
    ucSub = RxData0[5];
//Floor Setting
    for(i=0;i<64;i++)
    {
    	tAddr = eSETSTART + eFLOOR + i;
	Save_Byte_Data(tAddr, RxData0[6+i]);
	delay_ms(6);
    }
//Public Floor Setting
    for(i=0;i<8;i++)
    {
	tAddr = eSETSTART + ePUBLIC + i;
	Save_Byte_Data(tAddr, RxData0[70+i]);
	delay_ms(6);
    }
	
    TransmitServerAck(ucCommand, ucSub, ACK);
}

void RequestNElevatorFloorSet(void)
{
    BYTE i;
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;            
    tx_data0[3] = 81; 		// Packet Length
    tx_data0[4] = REQ_NELE_FLOOR;
    tx_data0[5] = 0x01;
    tx_data0[6] = ACK;
    
    for(i=0;i<64;i++) tx_data0[7 + i] = Read_Byte_Data(eSETSTART + eFLOOR+i); // 1 + i;
    for(i=0;i<8;i++) tx_data0[71 + i] = Read_Byte_Data(eSETSTART + eFLOOR+ i +64);
    
    tx_data0[79] = GetChecksum(&tx_data0[0],79);
    tx_data0[80] = ETX_ACU;

    for(i=0;i<81;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNDoorAddress(void)
{
	BYTE temp;
	BYTE cCommand, cSub;
	WORD tAddr;
	
	cCommand = RxData0[4];
	cSub = RxData0[5];
	
	BusAddress = RxData0[6];
	tAddr = eSETSTART + eBusAddress;
	Save_Byte_Data(tAddr,RxData0[6]);
	delay_ms(6);
	TransmitServerAck(cCommand, cSub, ACK);
}

void RequestNDoorAddress(void)
{
	BYTE temp,i;
	BYTE cCommand, cSub;
	WORD tAddr;

	tx_data0[0] = STX_ACU;
	tx_data0[1] = 0;
	tx_data0[2] = 0;
	tx_data0[3] = 9;
	tx_data0[4] = REQ_NDOOR_ADDRESS;
	tx_data0[5] = 0x01;
	tx_data0[6] = BusAddress;
	tx_data0[7] = GetChecksum(&tx_data0[0],7);
	tx_data0[8] = ETX_ACU;

    for(i=0;i<9;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
	
}

void SetNBuildingNumber(void)
{
	BYTE ucCommand,ucSub;
	WORD tAddr;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	tAddr = eSETSTART + eBuildingNumber;
	Save_Byte_Data(tAddr,RxData0[6]);
	delay_ms(6);
     
	TransmitServerAck(ucCommand, ucSub, ACK);
}

void RequestNBuildingNumber(void)
{
    BYTE i;
	
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 9; 	            // Packet Length	
    tx_data0[4] = REQ_NBUILDING_NO;
    tx_data0[5] = 0x01; 
    tx_data0[6] = Read_Byte_Data(eSETSTART + eBuildingNumber); 
    tx_data0[7] = GetChecksum(&tx_data0[0],15); 
    tx_data0[8] = ETX_ACU;

    for(i=0;i<9;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
	
}
void RequestNTimeSlot(void)
{
    BYTE i;
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;
    
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 16; 	            // Packet Length	
    tx_data0[4] = REQ_NTIME_SLOT;
    tx_data0[5] = 0x01;
    
    
    for(i=0; i<4; i++) tx_data0[6+i] = Read_Byte_Data(eSETSTART + eTimeSlotPermanent + i);
    for(i=0; i<4; i++) tx_data0[10+i] = Read_Byte_Data(eSETSTART + eTimeSlotGuest + i);

    tx_data0[14] = GetChecksum(&tx_data0[0],14);
    tx_data0[15] = ETX_ACU;

    for(i=0;i<16;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void SetNFloorNumber(void)
{
	BYTE ucCommand,ucSub;
	WORD tAddr;

	ucCommand = RxData0[4];
	ucSub = RxData0[5];
	tAddr = eSETSTART + eFloorNumber;
	Save_Byte_Data(tAddr,RxData0[6]);
	delay_ms(6);
     
	TransmitServerAck(ucCommand, ucSub, ACK);
}

void RequestNFloorNumber(void)
{
    BYTE i;
	
    tx_data0[0] = STX_ACU;
    tx_data0[1] = 0;
    tx_data0[2] = 0;
    tx_data0[3] = 9; 	            // Packet Length	
    tx_data0[4] = REQ_NFLOOR_NUMBER;
    tx_data0[5] = 0x01; 
    tx_data0[6] = Read_Byte_Data(eSETSTART + eFloorNumber); 
    tx_data0[7] = GetChecksum(&tx_data0[0],15); 
    tx_data0[8] = ETX_ACU;

    for(i=0;i<9;i++) TRH_NFCTxBuf[TRH_NFCTxLen++] = tx_data0[i];
}

void DeleteAllEvent(void)
{
    BYTE ucCommand, ucSub;
    BYTE InitBuf[32] ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    WORD tAddr,i;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];

    OverwriteState = 0;
    pEnterSave = 1; 
    pEnterSend= 0;     
    tAddr = eSETSTART + eOVERWRITE;
    Save_Byte_Data(tAddr, 0); 
    delay_ms(6);
    tAddr = eSETSTART + eFullWrite;
    Save_Byte_Data(tAddr, 0);
    delay_ms(6);
    
   Save_Word_Data(eSETSTART+eENTERSAVE, pEnterSave);
   Save_Word_Data(eSETSTART+eENTERSEND, pEnterSend);
    for(i=1;i<401;i++)
    {
	tAddr = i * 32;
	Save_Page_Data(tAddr,30,&InitBuf[0]);
        delay_ms(10);
    }

    TransmitServerAck(ucCommand, ucSub, ACK);    
}

void SetCardSector(void)
{
	BYTE i;
	BYTE cCommand, cSub;
	WORD tAddr;

	cCommand = RxData0[4];
	cSub = RxData0[5];
	
	Mifare_Set_Addr = RxData0[6];

	TransmitServerAck(cCommand, cSub, ACK);
}

void SetCardSectorKey(void)
{
	BYTE temp;
	BYTE i;
	BYTE cCommand, cSub;
	WORD tAddr;

	cCommand = RxData0[4];
	cSub = RxData0[5];

	Save_Byte_Data(eSETSTART + eMifareSectorKeySet,1);
	delay_ms(6);
	for(i=0;i<6;i++)
	{
		temp=RxData0[6+i];
		Set_Sectorkey[5-i] = temp;
		tAddr = eSETSTART + eMifareSectorKey +i;
		Save_Byte_Data(tAddr, RxData0[6+i]);
		delay_ms(6);
	}
	TransmitServerAck(cCommand, cSub, ACK);
}

void DoorLockControl(void)
{
    BYTE ucCommand, ucSub;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];
    
    if(RxData0[6] == 0)
    {  
        //RelayOn();    // Door Open
        //OpenMode = 1;
    }    
    else if(RxData0[6] == 1)
    {  
        //RelayOff();    // Door Closed
        //OpenMode = 0;
    }    
    
    TransmitServerAck(ucCommand, ucSub, ACK);    
}

void DoorOnetimeControl(void)
{
    BYTE ucCommand, ucSub;
    
    ucCommand = RxData0[4];
    ucSub = RxData0[5];

    Timer1_Sec_Stop();
    T1IRQ = 0;
    Timer1_Sec_Start(4);		//4초 timer 
    //RelayOn();
    //DOORCOdktlgN = 1;
    
    TransmitServerAck(ucCommand, ucSub, ACK);    
}


void UART0_NRcvDataChk(void)
{
	 BYTE cCommand = 0, cSub;
	 cCommand = RxData0[4];
	 cSub = RxData0[5];

	  switch(cCommand)
    	{
		case REQ_NCARD_DATA:				//0xB5 현재 사용안함
			CheckMobileKey();
		break;
		
		case REQ_NENTER_DATA:				//0xC3
			RequestNEnterData();            
		break;  

		case REQ_NINFOMATION:				//0xC1
			RequestNInform();
		break;

	    	case SET_NINITIALIZE:				//0xA5
			 SetNInitialize();
	    	break;

		case SET_NRELAY_TIME:				//0xBA
			SetNRelayContactTime();
		break;

		case REQ_NRELAY_TIME:				//0xB8
			RequestNRelayContactTime();            
		break;

		case SET_NHOTEL_CODE:				//0xB1
			SetNHotelCode();
		break;  

		case REQ_NHOTEL_CODE:				//0xB7
			RequestNHotelCode();
		break;

		case SET_NCURRENT_TIME:				//0xB3
			SetNCurrentTime();
		break;

		case REQ_NCURRENT_TIME: 			//0xC4 
			RequestNCurrentTime();
		break;

		case SET_NSPEACIAL_AREA:			//0xA6
			SetNSpecialAreaNumber();
		break;

		case REQ_NSPEACIAL_AREA:			//0xB4
			RequestNSpecialAreaNumber();
		break;

		case SET_NPARAMETER:				//0xA1
			SetNParameter();
		break;  

		case REQ_NPARAMETER:				//0xC5
			RequestNParameter();
		break;

		case SET_NBLE_POWER:					//0xD7
			SetNBlePower();
		break;

		case REQ_NBLE_POWER:				//0xD8
			RequestNBlePower();
		break;
		
	//----------------------------------------------------------

		case SET_NTIME_SLOT: 				//0xA4  현재 사용안함
			SetNTimeSlot();
		break;
		
		case SET_NSTAFF_TIME:				//0xA7 현재 사용안함
			SetNStaffTimeSlot();
		break;
		
		case SET_NGROUP_NUMBER:			//0xAA 현재 사용안함
			SetNGroupRoom();
		break;
		
		case SET_NELE_FLOOR:				//0xB2 현재 사용안함
			SetNElevatorFloor();
		break;

		case REQ_NELE_FLOOR:				//0xB6 현재 사용안함
			RequestNElevatorFloorSet();
		break;
		
		case SET_NDOOR_ADDRESS :			//0xBB 현재 사용안함
			SetNDoorAddress();
		break;

		case REQ_NDOOR_ADDRESS :			//0xBC 현재 사용안함
			RequestNDoorAddress();
		break;

		case REQ_NTIME_SLOT:				//0xC2 현재 사용안함
			RequestNTimeSlot();
		break;

		case SET_NBUILDING_NO:				//0xD0 현재 사용안함
			SetNBuildingNumber();			
		break; 

		case REQ_NBUILDING_NO:				//0xD1 현재 사용안함
			RequestNBuildingNumber();		
		break; 

		case REQ_NFLOOR_NUMBER :			//0xD3 현재 사용안함
			RequestNFloorNumber();
		break;

		case DELETE_ALL_EVENT:				//0xD4 현재 사용안함
			DeleteAllEvent();
		break;

		case SET_NCARD_SECTOR :			//0xD5 현재 사용안함
			SetCardSector();
		break;

		case SET_NCARD_SECTORKEY :			//0xD6 현재 사용안함
			SetCardSectorKey();
		break;

		case DOOR_NLOCK_CONTROL: 			//0x10  현재 사용안함
			DoorLockControl();
		break;

		case DOOR_NOPEN_CONTROL:			//0x11  현재 사용안함
			DoorOnetimeControl();
		break; 

	  default:
		 	Red_LED_On();
			delay_ms(100);
			Buzzer_On();
			delay_ms(500);
			Buzzer_Off();
	   break;
	  }
}

void Check_NFC_Data(void)
{
    if(NewData0)
    {
    	All_LED_On();
		Buzzer_On();
		delay_ms(50);
		Buzzer_Off();
        White_LED_On();
	
		Timer1_Sec_Stop();
		Timer1_Sec_Start(5);
		T1IRQ = 0;
        NewData0 = 0;
		RE_flagm=0;
        UART0_NRcvDataChk();
	
	
    }  
} 

