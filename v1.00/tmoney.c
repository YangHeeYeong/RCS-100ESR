
#include "common.h"


volatile BYTE DETECT_14443A;
volatile BYTE DETECT_14443A_MEM;
//BYTE CARD_DATA[20];
BYTE AsciiRoom[4];
BYTE RoomNumber[2];
BYTE TotalData[48];
//BYTE CardBuf[20];

//==============================================================================
// ISO14443A Functions
//==============================================================================
void ISO14443A_REG_HID(void)
{
	TRH_WRITE(0x11, 0x5B);
	TRH_WRITE(0x12, 0x3F);
	TRH_WRITE(0x19, 0x01);
	TRH_WRITE(0x1C, 0xA5);
	
	TRH_WRITE(0x14, 0x01);
	TRH_WRITE(0x1E, 0x01);
	TRH_WRITE(0x21, 0x06);
	TRH_WRITE(0x26, 0x04);
	delay_ms(4);
}

u8 ISO14443A_REQA_HID(void)
{
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
		return 0;
	}
}

u8 ISO14443A_ANTISELECT_HID(u8 sel)
{
	u8 i;
	TRH_WRITE(0x0F, 0x00);
	TRH_WRITE(0x22, 0x03);
	TRH_WRITE(0x09, 0x01);
	TRH_WRITE(0x02, sel);
	TRH_WRITE(0x02, 0x20);
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(2);
	//delay_us(650);
	delay_ms(2);

	DETECT_14443A = 0;
	
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	if((LEN == 5)&&!(ERROR&0x02))
	{
		for(i=0;i<5;i++) ACID[i] = TRH_READ(0x02);
		//UART_STRING("ISO14443A UID = ");for(i=0;i<5;i++) UART_H2A(ACID[i]);UART_TX(0x0A);
		TRH_WRITE(0x22, 0x0F);
		TRH_WRITE(0x09, 0x01);
		TRH_WRITE(0x02, sel);
		TRH_WRITE(0x02, 0x70);
		for(i=0;i<5;i++) TRH_WRITE(0x02, ACID[i]);
		TRH_WRITE(0x01, 0x14);
		
		//_delay_ms(2);
		delay_ms(2);

		LEN = TRH_READ(0x04);
		ERROR = TRH_READ(0x0A);
		TRH_WRITE(0x01, 0x00);
		TRH_WRITE(0x2E, 0x00);
		

		if((LEN == 1)&&!(ERROR&0x08)&&!(ERROR&0x02))
		{
			SAK = TRH_READ(0x02);
			//UART_STRING("ISO14443A ATQA = ");UART_H2A(ATQA[0]);UART_H2A(ATQA[1]);UART_TX(0x0A);
			//UART_STRING("ISO14443A UID = ");for(i=0;i<5;i++) UART_H2A(ACID[i]);UART_TX(0x0A);
			//PUT_STRING("%");
			//UART_STRING("ISO14443A SELECT = ");UART_H2A(SAK);UART_TX(0x0A);
			DETECT_14443A = 1;
			return 1;
		}
		else
		{
			//UART_STRING("ISO14443A ANTISELECT Fail!!\n");
			return 0;
		}
	}
	else
	{
		//UART_STRING("ISO14443A ANTISELECT Fail!!\n");
		return 0;
	}
}

u8 Mifare_Authentication_HID(u8 keysel, u8 addr)
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
	delay_ms(2);
	
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
	
		//_delay_ms(2);
		delay_ms(2);
		
		TRH_WRITE(0x3A, 0x00);
		
		LEN = TRH_READ(0x04);
		ERROR = TRH_READ(0x0A);
		TRH_WRITE(0x01, 0x00);
		TRH_WRITE(0x2E, 0x00);
		

 		//UART_STRING(" LEN = ");UART_H2A(LEN);UART_TX(0x0A);

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

u8 Mifare_Read_HID(u8 addr)
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
	for(i=0;i<temp;i++)
	{
		TRH_WRITE(0x27, TempBitBuf[i]);
	    TRH_WRITE(0x02, TempDataBuf[i]);
	}
	
	TRH_WRITE(0x01, 0x14);
	
	//_delay_ms(10);
	delay_ms(6);
	
	TRH_WRITE(0x3A, 0x00);
		
	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);
	
	if(LEN == 18)
	{
		for(i=0;i<18;i++) TempDataBuf[i] = TRH_READ(0x02);
		temp = MifareCrypto1Function(18, TempDataBuf, TempBitBuf, 4);

		for(i=0;i<temp;i++) CARD_DATA[i] = TempDataBuf[i];
		if(temp != 0)
		{
			//DETECT_14443A_MEM = 1;
			//UART_STRING("Mifare Memory Read = ");
			//for(i=0;i<temp;i++) UART_H2A(TempDataBuf[i]);
			//UART_TX(0x0A);
			//PUT_STRING("%");
		}
		else return 0;
	}
	
	return 0;
}

void MIFARE_WRITE_HID(u8 addr, char *txdata)
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
	
	if(LEN == 18){
		for(i=0;i<18;i++) TempDataBuf[i] = TRH_READ(0x02);
		temp = MifareCrypto1Function(18, TempDataBuf, TempBitBuf, 4);
		
		if(temp != 0){
			check = 0;
			for(i=0;i<16;i++){
				if(txdata[i] == TempDataBuf[i]) check += 0;
				else check += 1;
			}
			
			//if(check == 0) UartString("Mifare Memory WRITE OK!!");UartTx(0x0A);
		}
	}
}


unsigned char ISO14443A_RATS(void)
{
	//u8 i;
	/*
	TRH_WRITE(0x22, 0x0F);
	TRH_WRITE(0x09, 0x01);

	TRH_WRITE(0x02, 0xE0);
	TRH_WRITE(0x02, 0x50);
	TRH_WRITE(0x01, 0x14);

	delay_ms(10);
	//delay_ms(2);
	*/
	
  u8 i;
  
  TRH_WRITE(0x09, 0x01);
  TRH_WRITE(0x02, 0xE0);
  TRH_WRITE(0x02, 0x80);
  //###################################################################
  TRH_WRITE(0x01, 0x14);
  
  delay_ms(30);
  //###################################################################
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN != 0)&&!(TRH_ERR&0x08))
  {
    TRH_RxLen = 0;
    for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
    
    ISO14443_PCB = 0x0B;
    #if rfid_debug
    UART_STRING("\r\n ATR = ");for(i=0;i<TRH_RxLen;i++)UART_H2A(TRH_RxBuf[i]);
    #endif
    
    return 1;
  }
  else
  {
    TRH_WRITE(0x11,0x58);
    delay_ms(50);
    TRH_WRITE(0x11,0x5B);
  return 0;
  }
}

unsigned char ISO14443A_ExchangeBlock1(void)
{
	u8 i;
	
	TRH_WRITE(0x19, 0x00);
	TRH_WRITE(0x22, 0x0F);
	TRH_WRITE(0x09, 0x01);
	
	if(cid_toggle == 0)
	{
		TRH_WRITE(0x02, 0x0A);// I-block PCB with CID
		cid_toggle = 1;
	}
	else
	{
		TRH_WRITE(0x02, 0x0B);// I-block PCB with CID
		cid_toggle = 0;
	}
	TRH_WRITE(0x02, 0x00);  	// CID
	TRH_WRITE(0x02, 0x00);
	TRH_WRITE(0x02, 0xCA);
	TRH_WRITE(0x02, 0x01);
	TRH_WRITE(0x02, 0x01);
	TRH_WRITE(0x02, 0x08);

	TRH_WRITE(0x01, 0x14);

	_delay_ms(70);
	//delay_ms(4);

	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);

	if((LEN != 0) && !(ERROR&0x08))
	{
		for(i=0;i<LEN;i++) RFIDBUF[i] = TRH_READ(0x02);
		//UART_STRING("ISO14443A ExchangeBlock1 = ");for(i=0;i<LEN;i++) UART_H2A(RFIDBUF[i]);UART_TX(0x0A);
		for(i=0;i<8;i++) CRYPTOBUF[i] = RFIDBUF[i+2];
		return 1;
	}
	else
	{
		//UartString("ISO14443A ExchangeBlock1 Fail!!\n");
		return 0;
	}

}

/*
unsigned char ISO14443A_ExchangeBlock2(void)
{
	u8 i;
	
	des_proc();

	TRH_WRITE(0x19, 0x00);
	TRH_WRITE(0x22, 0x0F);
	TRH_WRITE(0x09, 0x01);
	//#################################################
	if(cid_toggle == 0)
	{
		TRH_WRITE(0x02, 0x0A);// I-block PCB with CID
		cid_toggle = 1;
	}
	else
	{
		TRH_WRITE(0x02, 0x0B);// I-block PCB with CID
		cid_toggle = 0;
	}
	TRH_WRITE(0x02, 0x00);  	// CID
	TRH_WRITE(0x02, 0x90);
	TRH_WRITE(0x02, 0xBA);
	TRH_WRITE(0x02, 0x00);
	TRH_WRITE(0x02, 0x00);
	TRH_WRITE(0x02, 0x08);

	for(i=0;i<8;i++) TRH_WRITE(0x02, CRYPTOBUF[i]);

	TRH_WRITE(0x01, 0x14);

	_delay_ms(70);
	//delay_ms(4);

	LEN = TRH_READ(0x04);
	ERROR = TRH_READ(0x0A);
	TRH_WRITE(0x01, 0x00);
	TRH_WRITE(0x2E, 0x00);

	if((LEN != 0) && !(ERROR&0x08))
	{
		for(i=0;i<LEN;i++) RFIDBUF[i] = TRH_READ(0x02);
		DETECT_14443A_MEM = 1;
		for(i=0;i<16;i++) CARD_DATA[i] = RFIDBUF[2+i];

		//UART0_TX_EN;
		//UART_STRING("ISO14443A ExchangeBlock2 = ");for(i=0;i<LEN;i++) UART_H2A(RFIDBUF[i]);UART_TX(0x0A);
		//_delay_ms(50);
		//UART0_RX_EN;
		return 1;
	}
	else
	{
		//UartString("ISO14443A ExchangeBlock2 Fail!!\n");
		return 0;
	}

}
*/

void ISO14443A_UID(void)
{
	u8 temp;	
		
	
	ISO14443A_REG_HID();
	temp = ISO14443A_REQA_HID();
	if(temp == 0) return;
	temp = ISO14443A_ANTISELECT_HID(0x93);
	if(temp == 0) return;

}
/*
void ISO14443A_HID(void)
{
	//u8 i;
	u8 temp;
	
	TRH_WRITE(0x11, 0x58);
	_delay_ms(50);
	ISO14443A_REG_HID();
	_delay_ms(10);
	temp = ISO14443A_REQA_HID();
	if(temp == 0) return;
	temp = ISO14443A_ANTISELECT_HID(0x93);
	if(temp == 0) return;
	
	if((SAK&0x24) == 0x20)
	{
		temp = ISO14443A_RATS();
		if(temp == 1)
		{
			temp = ISO14443A_ExchangeBlock1();
			if(temp == 1)
			{
				ISO14443A_ExchangeBlock2();
			}
		}
	}
	else
	{
		temp = Mifare_Authentication_HID(1, Block_Address);
		if(temp == 1)
		{
			//UART_STRING("Mifare Authentication OK!!\n");
			Mifare_Read_HID(Block_Address);
		}
	}
}
*/


unsigned char CalculateChecksum(unsigned char ucLength)
{
	unsigned char tResult=0, i;

	//tResult = str[0];
	for(i=0; i<ucLength-1; i++) tResult += TotalData[i];
	//tResult = tResult & 0x7f;

	return tResult;
}
/*
void SearchCardData(void)
{
	CardBuf[0] = TotalData[0];
	CardBuf[1] = TotalData[1];
	CardBuf[2] = TotalData[2];
	CardBuf[3] = TotalData[3];
	CardBuf[4] = TotalData[4];
	CardBuf[5] = TotalData[5];
	CardBuf[6] = TotalData[6];
	CardBuf[7] = TotalData[32];
	CardBuf[8] = TotalData[33];
	CardBuf[9] = TotalData[34];
	CardBuf[10] = TotalData[35];
	CardBuf[11] = TotalData[36];
	CardBuf[12] = TotalData[37];
	CardBuf[13] = TotalData[38];
	CardBuf[14] = TotalData[39];
	CardBuf[15] = TotalData[40];
	CardBuf[16] = TotalData[41];
	CardBuf[17] = TotalData[42];
	CardBuf[18] = TotalData[43];	
}
*/
void ISO14443A_HID(unsigned char nAddress)
{
	u8 temp, i, tSum;
	
	
	ISO14443A_REG_HID();
	temp = ISO14443A_REQA_HID();
	if(temp == 0) return;
	temp = ISO14443A_ANTISELECT_HID(0x93);
	if(temp == 0) return;
	temp = Mifare_Authentication_HID(0, nAddress);
	if(temp == 1)
	{
		//UART_STRING("Mifare Authentication OK!!\n");
		//PUT_STRING("%");
		Mifare_Read_HID(nAddress);
		for(i=0; i<4; i++) AsciiRoom[i] = CARD_DATA[8+i];		
		for(i=0; i<16; i++) TotalData[i] = CARD_DATA[i];

		Mifare_Read_HID(nAddress+1);
		for(i=0; i<16; i++) TotalData[16 + i] = CARD_DATA[i];

		Mifare_Read_HID(nAddress+2);
		for(i=0; i<16; i++) TotalData[32 + i] = CARD_DATA[i];

		//Mifare_Read_HID(nAddress+3);


		for(i=0; i<2; i++) AsciiBuf[i] = AsciiRoom[i];
		RoomNumber[0] = Ascii2Hex();

		for(i=0; i<2; i++) AsciiBuf[i] = AsciiRoom[2+i];
		RoomNumber[1] = Ascii2Hex();

		/*
		UART_STRING("Room Number = ");
		for(i=0;i<2;i++) UART_H2A(RoomNumber[i]);
		UART_TX(0x0A);
		PUT_STRING("%");
		*/

		tSum = CalculateChecksum(47);
		
		if(tSum == TotalData[47])
		{
			DETECT_14443A_MEM = 1;	
		}
		else DETECT_14443A_MEM = 0;



		//SearchCardData();
		//UART_STRING("Check Sum = ");
		//UART_H2A(tSum);
		//PUT_STRING("%");
	}
}

