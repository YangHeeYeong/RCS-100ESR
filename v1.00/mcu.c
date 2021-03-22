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
* Filename : mcu.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2011/06/20
* Description : 
********************************************************************************
*/

#include "common.h"

BYTE  tx_data[60];
BYTE  CARD_UID[4];
BYTE TxData1[255];
volatile BYTE ReaderAddr;
//volatile BYTE DoorAddr = 0;
volatile BYTE DETECT_CARD = 0;
volatile BYTE REQUEST_CARD = 0;
volatile BYTE NEW_CARD = 0;
volatile BYTE ACU_DATA_CARD= 0; //2020.11.17 김태성 사원 수정, 우이동 ACU 연동 변수 
volatile BYTE NORMAL_RESET = 0;
//volatile BYTE OPEN_RESET = 0;
//volatile BYTE BuzzerOnFg = 0;
//volatile BYTE ResetFg = 0;
volatile BYTE Time= 0;

void MCU_Init(void)     // initialize ATmege64 MCU 
{ 
	// 1:out , 0:in	
    // Adress Data Port //  
    // Adress Data Port //   
	DDRA  = 0xFF;				
	PORTA = 0x00;

	DDRB  = 0xFF;					
	PORTB = 0x00;

	DDRC = 0xFF;
	PORTC = 0xFF;

	DDRD  = 0x08;
	PORTD = 0x00;

	DDRE  = 0xF6;			
	PORTE = 0x0F;

	DDRG = 0xFF;
	PORTG = 0xFF;

	DDRF  = (0<<3)||(0<<2)||(0<<1)||(0<<0);			
	PORTF = (1<<3)||(1<<2)||(1<<1)||(1<<0);


	//UART TXD IO Output setting
	//DDRE |= (1<<PE1)|(1<<PE2);
	//RS485_EN = 1;
	//LED IO Output setting
	//DDRD |= (1<<PD6)| (1<<PD7);
	//BUZZER IO Output setting
	//DDRB |= (1<<PB7);
	//TRH IO Output setting
	
	DDRC |= (1<<PC3)|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7);

	//DDRE |= (0<<3);	
	ADCSRA = 0x00;


	// Timer/Counter 0 initialization
	TCCR0=0x00;
	ASSR=0x00;
	TCNT0=0x00;
	OCR0=0x00;

	// Timer/Counter 1 initialization
	TCCR1A=0x00;                       
	TCCR1B=0x05;                       // CK / 1024
	TCNT1H=0x00;
	TCNT1L=0x00;

	// Timer/Counter 2 initialization
	TCCR2=0x00;
	ASSR=0x00;
	TCNT2=0x00;
	OCR2=0x00;

	// Timer/Counter 3 initialization
	TCCR3A=0x00;
	TCCR3B=0x05;                       // CK / 1024
	TCNT3H=0x00;
	TCNT3L=0x00;
 
    // External Interrupt(s) initialization
	// External Interrupt(s) initialization
	EICRA=0x00;
	EICRB=0x00;
	EIMSK=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x00;
  	ETIMSK=0x00;
}



// CRC table for the CRC-16. The poly is 0x8005 (x^16 + x^15 + x^2 + 1)

const WORD crcTable[256] = {
       0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
       0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
       0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
       0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
       0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
       0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
       0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
       0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
       0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
       0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
       0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
       0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
       0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
       0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
       0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
       0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
       0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
       0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
       0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
       0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
       0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
       0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
       0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
       0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
       0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
       0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
       0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
       0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
       0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
       0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
       0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
       0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


/*****************************************************************
 * Function:    crcCompute()
 *
 * Description: Compute the CRC checksum of a binary message block.
 * Notes:       This function expects that crcInit() has been called
 *              first to initialize the CRC lookup table.
 * Returns:     The CRC of the data.
 *****************************************************************/
width CrcCompute(unsigned char *message, unsigned int nBytes)
{
    unsigned int   offset;
    unsigned char  byte;
    width remainder = INITIAL_REMAINDER;
 
    /* Divide the message by the polynomial, a byte at time. */
    for (offset = 0; offset < nBytes; offset++)
    {
        byte = (remainder >> (WIDTH - 8)) ^ message[offset];
        remainder = crcTable[byte] ^ (remainder << 8);
    } 
 
    /* The final remainder is the CRC result. */
    return (remainder ^ FINAL_XOR_VALUE);
}


void TransmitCardData(void)
{
    BYTE CRC_Low, CRC_High, i;
    WORD uiCRC;
    
    UART0_TX_EN;
    
    tx_data[0] = STX_READER;
    tx_data[1] = ReaderAddr;
    tx_data[2] = 55; 				// Packet Length
    tx_data[3] = DATA_CHK;
	
	for(i=0; i<48; i++) tx_data[4+i] = TotalData[i];
	/*         
    tx_data[4] = CardBuf[0];
    tx_data[5] = CardBuf[1];
    tx_data[6] = CardBuf[2];
    tx_data[7] = CardBuf[3];
	tx_data[8] = CardBuf[4];
    tx_data[9] = CardBuf[5];
    tx_data[10] = CardBuf[6];
    tx_data[11] = CardBuf[7];
    tx_data[12] = CardBuf[8];
	tx_data[13] = CardBuf[9];
    tx_data[14] = CardBuf[10];
    tx_data[15] = CardBuf[11];
    tx_data[16] = CardBuf[12];
    tx_data[17] = CardBuf[13];
	tx_data[18] = CardBuf[14];
	tx_data[19] = CardBuf[15];	
	tx_data[20] = CardBuf[16]; 
	tx_data[21] = CardBuf[17];   
	tx_data[22] = CardBuf[18];
	*/
    uiCRC = CrcCompute(&tx_data[0], 52);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    tx_data[52] = CRC_High;
    tx_data[53] = CRC_Low;

    tx_data[54] = ETX_READER; 

	delay_us(700);
	UART0_TX_EN;
    UART0_PUT_STRING(tx_data, 55);
    delay_ms(3);                  // 9 Byte Transmission Delay
    UART0_RX_EN;
}

/*  
//============================================================//
void Card_Sucess(void)
{
    BLUE_LED_ON;
    RED_LED_OFF;
    GREEN_LED_OFF;
 
	//BUZZER_ON;
	BUZZER = 1;
	delay_ms(250);
	BUZZER = 0;
	//BUZZER_OFF;
	delay_ms(100);
}

//============================================================//
void Card_Failed(void)
{
    BLUE_LED_OFF;
    GREEN_LED_OFF;
    RED_LED_ON;

   	//BUZZER_ON;
	BUZZER = 1;
   	delay_ms(250);
	BUZZER = 0;
   	//BUZZER_OFF;
   	delay_ms(180);
        
   	//BUZZER_ON;
	BUZZER = 1;
   	delay_ms(250);
	BUZZER = 0;
   	//BUZZER_OFF;
   	delay_ms(180);
}
*/

void All_LED_On(void)
{
	LED_BLUE = 1;
	LED_WHITE = 1;
	LED_RED = 1;
}

void All_LED_Off(void)
{
	LED_BLUE = 0;
	LED_WHITE = 0;
	LED_RED = 0;
}


void Blue_LED_On(void)
{
	LED_BLUE = 0;
	LED_WHITE = 1;
	LED_RED = 0;
}

void White_LED_On(void)
{
	LED_BLUE = 1;
	LED_WHITE = 0;
	LED_RED = 0;
}

void Red_LED_On(void)
{
	LED_BLUE = 0;
	LED_WHITE = 0;
	LED_RED = 1;
}
/*
void Buzzer_On(void)   //  2kHz   
{
   //TCCR2 = 0x72;   // ck/8 = 0x72
   TCCR2 = 0x72;   // ck/8 = 0x72
   //OCR2 = 125;     // 125
   OCR2 = 125;     // 125
}
*/
void Buzzer_On(void)   //  2.8kHz   
{
   //TCCR2 = 0x72;   // ck/8 = 0x72
   //OCR2 = 125;     // 125
   TCCR2 = 0x1B; //ck/64
   OCR2 = 0x15; // 45   
}

void Buzzer_Off(void)
{
     TCCR2 = 0x00;
}

/*
void Buzzer_On(void)
{
    //BUZZER_ON;
   BUZZER = 1;
} 

void Buzzer_Off(void)
{
    //BUZZER_OFF;
   BUZZER = 0;
}
*/
void CardNotReturn(void)
{
	RED_LED_OFF;
	BLUE_LED_ON;
	WHITE_LED_ON;

	//BUZZER_ON;
	Buzzer_On();
	delay_ms(250);
	//BUZZER_OFF;
	Buzzer_Off();
	delay_ms(100);
	Buzzer_On();
	//BUZZER_ON;
	delay_ms(250);
	Buzzer_Off();
	//BUZZER_OFF;
	delay_ms(100);
	Buzzer_On();
	//BUZZER_ON;
	delay_ms(250);
	Buzzer_Off();
	//BUZZER_OFF;
	delay_ms(100);

	White_LED_On();
}

void ISO14443A_UID_LOOP(BYTE ucCommand)
{
    BYTE i;
    ISO14443A_UID();
   
    if(DETECT_14443A)             // Detect ISO14443A CARD
    {
        DETECT_CARD = 1;     
        DETECT_14443A = 0;        
        for(i=0; i<4; i++)  CARD_UID[i] = ACID[i];
        
        if(CARD_UID[0] != 0 || CARD_UID[1] != 0 || CARD_UID[2] != 0 || CARD_UID[3] != 0)
        { 
			Timer1_Sec_Stop();
			T1IRQ = 0;			 
            Timer1_Sec_Start(3);

			//if(!NormalOpenMode)
			//{
            	Timer0_mSec_Stop();
            	T0IRQ = 0;
            	Timer0_mSec_Start(40);
            	//Buzzer_On();
				Buzzer_On();
            	//BuzzerOnFg = 1;
			//}
        }
    }     
}  

void ISO14443A_Memory_LOOP(BYTE ucCommand)
{
    //BYTE LoopCnt, TimeChk;
    //BYTE temp=0;
    //BYTE i=0;

	ISO14443A_HID(12);		// Read Sector 3


    if(DETECT_14443A_MEM)             // Detect ISO14443A CARD
    {
        //DETECT_CARD = 1;     
        DETECT_14443A_MEM = 0;
		 
		Buzzer_On();
		delay_ms(200);
		Buzzer_Off();

		//TransmitCardData();
		//if(DETECT_14443A_MEM == 1)
		CheckCardData();
		                                                                                                                                       
		//GREEN_LED_ON;
		/*
    	for(LoopCnt=0; LoopCnt <3 ; LoopCnt++)
    	{
        	TimeChk = 0;
        	TransmitCardData();
			//delay_ms(2);

			DETECT_CARD = 0; 
			//REQUEST_CARD = 1;
			
			Timer1_Sec_Stop();
			T1IRQ = 0;
      		Timer1_Sec_Start(10);		// 1초 timer 


			while( !T1IRQ && !TimeChk)
        	{  
            	//UART0_Rcv_Data();
            	if(NewData0)
            	{
                	TimeChk = 1;
                	NewData0 = 0;
        			temp = UART_RcvDataChk();
        			if(temp) ReaderRunFunction();  
            	}
        	}
        	if(temp) break;
    	}
		if(TimeChk == 0) 
		{
			CardNotReturn();
			REQUEST_CARD = 0;
		}
		*/
    } 
}  


void TransmitNoneCardData(BYTE ucCommand)
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;   
    	
    delay_ms(1);  
    tx_data[0] = STX_READER;
    tx_data[1] = ReaderAddr; 
    tx_data[2] = 8;                       // Packet Length
    tx_data[3] = ucCommand;
    tx_data[4] = 0;

    uiCRC = CrcCompute(&tx_data[0], 5);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    
    tx_data[5] = CRC_High;
    tx_data[6] = CRC_Low;
    tx_data[7] = ETX_READER;
    
    UART0_TX_EN;
    delay_us(10);
    UART0_PUT_STRING(tx_data, 8);        
    delay_ms(2);
    UART0_RX_EN;
} 

void TransmitCardID(void)
{
    BYTE CRC_Low, CRC_High;
    WORD uiCRC;
    
    //UART0_TX_EN;
    
    //tx_data[0] = STX_R;
    //tx_data[1] = STX_1;
    //tx_data[2] = DoorAddr; 
    tx_data[3] = 12;         // Packet Length
    tx_data[4] = UID_CHK;
    tx_data[5] = CARD_UID[0];
    tx_data[6] = CARD_UID[1];
    tx_data[7] = CARD_UID[2];
    tx_data[8] = CARD_UID[3];
    
    uiCRC = CrcCompute(&tx_data[0], 9);
    CRC_Low = (BYTE)uiCRC;                // crc Low Byte
    CRC_High = (BYTE)(uiCRC >>8);         // crc Hign Byte
    tx_data[9] = CRC_High;
    tx_data[10] = CRC_Low;
    tx_data[11] = ETX; 

	delay_us(700);
	UART0_TX_EN;
    UART0_PUT_STRING(tx_data, 12);
    delay_us(800);                  // 9 Byte Transmission Delay
    UART0_RX_EN;
}


void ReadCardUID(BYTE Command)
{
    ISO14443A_UID_LOOP(Command);
    //if(!DETECT_CARD)  ISO14443B_LOOP(Command);
    //if(!DETECT_CARD)  ISO15693_LOOP(Command);
    //if(!DETECT_CARD)  TAGIT_LOOP(Command);
    //if(!DETECT_CARD && !REQUEST_CARD)
    //{  
		delay_ms(200);
		TRH_INIT();
    //}  
}
void BLE_Rcv_Command(void)
{
	//u8 temp=0;       
	//u8 i;
	if(NewData1)
	{
		 NewData1=0;
	//	UART0_PUT_STRING(RxData1, RxData1[3]);
		BLE_Cmd_Function();
	}
}

void BLE_Cmd_Function(void)
{
	u8 Command,data;
	u8 i;//,j;
	
	Command=RxData1[4];
	data=RxData1[5];
	
	switch(Command)
	{
		case 'A' :
			BLE_Time_Send1();
			break;
		case 'T' :
				BLE_Test1();//BLE_Time_Send();
				break;
		case 'C' :
			if(!NEW_CARD)
			{
				BLE_DataTest(data);
				delay_ms(10);
				//BLE_CardResult_Send();
			BLE_Time_Send1();
			}
			break;
			
		case REQ_CARD_DATA : 
			//if(!NEW_CARD)
			//{
				//NEW_CARD = 1;

			    //Buzzer_On();
			    //delay_ms(100);
			   // Buzzer_Off();

				for(i=0;i<32;i++) MyBuf[i]=RxData1[i+6];

				//UART0_TX;
				//UART0_PUT_STRING(MyBuf,32);
				//delay_ms(15);
				//UART0_RX;

				CheckCardClass();
				BLE_CardResult_Send();
				delay_ms(2);
			//}
			//else 	BLE_CardResult_Send();
			break;
		case 0xBA :	//BLE Module MAC Address Setting value transmit
			BLE_AdPk_Set();
			break;
		default : break;
	}
}


void BLE_DataTest(u8 data)
{
	if(data=='a')
	{
		nCardResult=0x01;
		nErrorCode=0x00;
	        //GreenLedFg = 1;         
		NEW_CARD = 1;
		Timer1_Sec_Stop();
		T1IRQ = 0;
	    	Timer1_Sec_Start(CallTime);		//4초 timer 
		Buzzer_On();
		delay_ms(100);
		Buzzer_Off();
		//Doorlock_Open();
		
		CardFailed();
		//Transmit_Class(0x31);
		//RelayOn();
	       // Doorlock_Open();
	}
	else if(data=='b')
	{
		nCardResult=0x02;	
		nErrorCode=0x01;
		//RedLedFg=1;
		Timer1_Sec_Stop();
		T1IRQ = 0;
	    	Timer1_Sec_Start(CallTime);	
		Buzzer_On();
		delay_ms(100);
		Buzzer_Off();
		CardFailed();
	}
}



void BLE_AdPk_Set(void)
{
	BYTE addr_data[6];
	BYTE passkey_data[10];
	BYTE RNBlePower;
	WORD RN_temp;
	
	DWORD addr_temp;
	RNBlePower = Read_Byte_Data(eSETSTART + eBlePower);
	if(RNBlePower == 0xff) RNBlePower = 0x11; 
	//RN_temp=Read_Byte_Data(eSETSTART + eAREANUMBER);
	Read_Hotelcode_eeprom();
	
	//if(RN_temp == 0xffff) RN_temp = 101; 
	RN_temp=0xFF00;
	//RN_temp=0x00CA;
	addr_data[0]=(BYTE)(RN_temp);
	addr_data[1]=(BYTE)(RN_temp>>8);
	
	addr_temp = uiHotelCode * 65535;

	addr_data[2]=(BYTE)(addr_temp);
	addr_data[3]=(BYTE)(addr_temp>>8);
	addr_data[4]=(BYTE)(addr_temp>>16);
	addr_data[5]=(BYTE)(addr_temp>>24);

	TxData1[0]=STX_ACU;
	TxData1[1]=0x00;
	TxData1[2]=0x00;
	TxData1[3]=0x15;
	TxData1[4]=0xBA;
	TxData1[5]=addr_data[0];
	TxData1[6]=addr_data[1];
	TxData1[7]=addr_data[2];
	TxData1[8]=addr_data[3];
	TxData1[9]=addr_data[4];
	TxData1[10]=addr_data[5];
	
	passkey_data[0] = ('0'+(uiHotelCode/1000))^0x55;
	passkey_data[1] = ('0'+((uiHotelCode/100)%10))^0x55;
	passkey_data[2] = ('0' +((uiHotelCode/10)%10))^0x55;
	passkey_data[3] = ('0' +(uiHotelCode%10))^0x55;
	passkey_data[4] = ('0' +(RN_temp/1000))^0x55;
	passkey_data[5] = ('0'+((RN_temp/100)%10))^0x55;
	passkey_data[6] = ('0' +((RN_temp/10)%10))^0x55;
	passkey_data[7] = ('0'+(RN_temp%10))^0x55;
	
	TxData1[11]=passkey_data[0];
	TxData1[12]=passkey_data[1];
	TxData1[13]=passkey_data[2];
	TxData1[14]=passkey_data[3];
	TxData1[15]=passkey_data[4];
	TxData1[16]=passkey_data[5];
	TxData1[17]=passkey_data[6];
	TxData1[18]=passkey_data[7];
	TxData1[19]=RNBlePower; //BLePower
	TxData1[20]=ETX_ACU;
	
	delay_ms(2);
	UART1_PUT_STRING(TxData1,21);
	
	
	if(!NEW_CARD)
	{
		Timer1_Sec_Stop(); 
		//BLE_ADDR_SET=1;
		Timer1_Sec_Start(2);
		
	}	
}

void BLE_CardResult_Send(void)
{
	//u8 crc_high,crc_low;
	//u16 uiCRC;
	
	TxData1[0]=STX_ACU;
	TxData1[1]=0x00;
	TxData1[2]=0x00;
	TxData1[3]=11;
	TxData1[4]=REQ_CARD_DATA;
	TxData1[5]=0x01;
	TxData1[6]=0x01;
	TxData1[7]=nCardResult;
	TxData1[8]=nErrorCode;
	
	TxData1[9]=GetChecksum(&TxData1[0],9);
	TxData1[10]=ETX_ACU;

	delay_ms(2);
	UART1_PUT_STRING(TxData1,11);
	nErrorCode=0;
}

void BLE_Time_Send(void)
{
	Rtc_Read_Data();
	
	TxData1[0]=STX_ACU;
	TxData1[1]=0x00;
	TxData1[2]=0x00;
	TxData1[3]=0x0F;
	TxData1[4]='T';
	TxData1[5]=0x01;
	TxData1[6]=rtc_year;
	TxData1[7]=rtc_month;
	TxData1[8]=rtc_date;
	TxData1[9]=rtc_week;
	TxData1[10]=rtc_hour;
	TxData1[11]=rtc_min;
	TxData1[12]=rtc_sec;
	
	TxData1[13]=GetChecksum(&TxData1[0],13);
	TxData1[14]=ETX_ACU;

	delay_ms(2);
	UART1_PUT_STRING(TxData1,15);
}


void BLE_Time_Send1(void)
{
	//BYTE i;	
	Rtc_Read_Data();
	TxData1[0]=STX_ACU;
	TxData1[1]=0x00;
	TxData1[2]=0x00;
	TxData1[3]=0x0E;
	TxData1[4]='T';
	TxData1[5]=0x01;
	
	TxData1[6]=rtc_year;
	TxData1[7]=rtc_month;
	TxData1[8]=rtc_date;
	TxData1[9]=rtc_hour;
	TxData1[10]=rtc_min;
	TxData1[11]=rtc_sec;
	
	
	
	TxData1[12]=GetChecksum(&TxData1[0],12);
	TxData1[13]=ETX_ACU;

	delay_ms(2);
	UART1_PUT_STRING(TxData1,14);
}

void BLE_Test1(void)
{
	BYTE tCnt = 0;
	//WORD uiCRC, tAddr;
	WORD tAddress, ckReadAddr;

	TxData1[0] = STX_ACU;
	TxData1[1] = 0;
	TxData1[2] = 0;
	TxData1[3] = 41; 

	TxData1[4] = 'T';
	TxData1[5] = 'a';                // sub command
	TxData1[6] = ACK;                  // Hardware Address
	//TxData1[7] = (BYTE)(pLastEvent >> 8);
	//TxData1[8] = (BYTE)pLastEvent;

	tCnt = 9;

	tAddress = 0;

	ckReadAddr = (pEnterSend + tAddress) * 32; 
	ReadExtMemoryPageData(ckReadAddr, 30, &TxData1[9]);
	tCnt = tCnt + 30;
	pEnterSend++;   
	if(pEnterSave <= pEnterSend )
	{
		//fStartRead = 0;
		//pEnterSend = pEnterSend ;
	}        

	TxData1[39] = GetChecksum(&TxData1[0],tCnt);
	TxData1[40] = ETX_ACU; 

	delay_ms(50);
	UART1_PUT_STRING(TxData1,41);
}


void ReadCardMemory(BYTE Command)
{
    ISO14443A_Memory_LOOP(Command);		// Read Card

    if(!DETECT_CARD && !NEW_CARD)	// None Card Reading/ None Request
    { 
		/*
		Timer1_Sec_Stop();
		T1IRQ = 0;	
		Timer1_Sec_Start(1);
		NORMAL_RESET = 1;
		*/
		delay_ms(10);
		TRH_INIT();				 
    }  
}
