/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : doorlock.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/12
* Description : 
********************************************************************************
*/
#include "common.h"

//BYTE SwitchMode;
//BYTE secu_sw_flag;
//BYTE door_sw_flag;
//volatile BYTE DoorOpenFg = 0, DoorCloseFg = 0;
//volatile BYTE GreenLedFg = 0, RedLedFg = 0;
//volatile BYTE SuccessBuzzerOnFg = 0;
//volatile BYTE FailedBuzzerOnFg = 0;
//volatile BYTE BuzzzerCnt = 0;

/*
void Buzzer_On(void)   //  2kHz   
{
	TCCR2 = 0x72;   // ck/8 = 0x72
	OCR2 = 125;     // 125
}

void Buzzer_Off(void)
{
  	TCCR2 = 0x00;
}
*/

//============================================================//
/*
void BuzzerOn(void)
{
    BUZZER=1;
	//BUZZER = 1;
	//TIMSK |= 0x40; 


	//TCCR2=0x72;   // ck/8 = 0x72
	//OCR2=125;     // 125
} 

void BuzzerOff(void)
{
    BUZZER=0;
	//BUZZER = 0;


	//TCCR2 = 0x00;
}  
*/

void BuzzerOn(void)   //  2kHz   
{
	BUZZER=1;
	//TCCR2 = 0x72;   // ck/8 = 0x72
	//OCR2 = 125;     // 125
}

void BuzzerOff(void)
{
	BUZZER=0;
  	//TCCR2 = 0x00;
}


/*
void FailedBuzzerOnProcess(void)
{
    switch(BuzzzerCnt)
    {   
    case 1: 
        BuzzzerCnt = 2;
        Timer0_mSec_Start(180);
        BuzzerOff();
        break;            
    case 2:
    
        BuzzzerCnt = 3;
        Timer0_mSec_Start(250);
        BuzzerOn();
        break;
        
    case 3:   
        BuzzzerCnt = 0;
        FailedBuzzerOnFg = 0;
        BuzzerOff();
        break;
    default:
        break;
    }
}  
*/

void TransmitDefaultData(BYTE ucCommand)
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;   
    
    delay_ms(2);
	UART0_TX_EN;

    tx_data[0] = STX_READER;
    tx_data[1] = ReaderAddr;
    tx_data[2] = 8;                       // Packet Length
    tx_data[3] = ucCommand;
    tx_data[4] = 1;
   		
    uiCRC = CrcCompute(&tx_data[0], 5);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    tx_data[5] = CRC_High;
    tx_data[6] = CRC_Low;
    tx_data[7] = ETX_READER;   
	 
    UART0_PUT_STRING(tx_data, 8);        
    delay_ms(3);
    UART0_RX_EN;
}


BYTE UART_RcvDataChk(void)
{
    BYTE TempAddr = 0;
	BYTE RcvChk = 0;
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;  
  
    if(RxData0[1] == ReaderAddr)  TempAddr = 1;
    else TempAddr = 0;

    //tCount = RxData0[3];

    if(TempAddr) 
    {
     	uiCRC = CrcCompute(&RxData0[0], 6);
        CRC_Low = (BYTE)uiCRC;                // crc Low Byte
        CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
        
        if( CRC_Low == RxData0[7] && CRC_High == RxData0[6] ) RcvChk = 1;
        else RcvChk = 0;
			

    }        
    return RcvChk;
    //return TempAddr;
}


void ReaderRunFunction(void)
{
	/*
    if(RxData0[3] == REQ_UID)
    {  
        if(DETECT_CARD)
        {   
            DETECT_CARD = 0;
            REQUEST_CARD = 1;
            TransmitCardData();
        }    
        else TransmitDefaultData(REQ_UID);         // Request Card Data        
    }
	*/

    if(RxData0[3] == UID_CHK)
    {
        if(RxData0[5] == ACK)   
        {
            if(REQUEST_CARD)
            {  
				REQUEST_CARD = 0;
				NEW_CARD = 1;
                Time = RxData0[4] ;
                //Card_Sucess(); 
                
                Timer1_Sec_Stop();
	        	T1IRQ = 0;	
	        	Timer1_Sec_Start(Time);  
				              
            }
        }  
        else if (RxData0[5] == NAK)
        {
            if(REQUEST_CARD)
            {  
				REQUEST_CARD = 0;
				NEW_CARD = 1;
                //Card_Failed();
	        	Time = 0;
                
                Timer1_Sec_Stop();
	        	T1IRQ = 0;	
	        	Timer1_Sec_Start(10);                    
            }
        }
        //delay_ms(2);  
        //TransmitDefaultData(UID_CHK);
    } 	  
}

void UART_Rcv_Command(void)
{
    BYTE  temp = 0;
    
    if(NewData0)
    {
        temp = UART_RcvDataChk();
        if(temp) ReaderRunFunction();
		NewData0 = 0;
    }  
}  
