/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : uart.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/11
* Description : 
********************************************************************************
*/
#include "common.h"

#define BEL			0x07
#define BS			0x08
#define LF			0x0A
#define CR			0x0D
#define ESC			0x1B

BYTE RxData0[255];
BYTE DataCnt0 = 0; 
BYTE tx_data0[250];
BYTE RxData1[255];
BYTE DataCnt1 = 0; 

volatile BYTE NewData0;
volatile BYTE NewData1;
volatile BYTE NewData;

BYTE RcvLength0 = 4;
BYTE RcvLength1 = 4;
char AsciiBuf[2];

ISR(USART0_RX_vect)
{
    BYTE temp;
	BYTE i,tmp;
	
    temp = UDR0;
	
    switch( temp ) 
    {			
    	case STX_MAIN:         			// Start of Frame is STX_MAIN 
          	if(DataCnt0 == 0)
          	{  
	      		RxData0[0] = STX_MAIN;       	// Save STX
	      		DataCnt0 = 1;           	// Increase save count
          	}
          	else
              	RxData0[DataCnt0++] = temp;      
	  			break;
        
        default :
          	RxData0[DataCnt0++] = temp;
          	if(RxData0[0] != STX_MAIN)
          	{
              	DataCnt0 = 0;
              	RcvLength0 = 5;
          	}    
          	if(DataCnt0 >= RcvLength0) // check end byte
          	{
              	if (temp == ETX_MAIN) 
              	{
	                  	NewData = TRUE;
	                  	DataCnt0 = 0;
				RcvLength0 = 5;
              	}
				
			else
			{
				NewData = 0;
				DataCnt0 = 0;
				RcvLength0 = 5;    // Save data length
			}
          	}
		if(DataCnt0 == 4)
          	{ 
              	RcvLength0 = temp;    // Save data length
          	}   
			
          	break;
	}  
}


ISR(USART1_RX_vect)
{
    BYTE temp;
    temp = UDR1;

    switch( temp ) 
    {			
    	case STX_MAIN:         			// Start of Frame is STX_MAIN 
          	if(DataCnt1 == 0)
          	{  
	      		RxData1[0] = STX_MAIN;       	// Save STX
	      		DataCnt1 = 1;           	// Increase save count
          	}
          	else
              	RxData1[DataCnt1++] = temp;      
	  			break;
        
        default :
          	RxData1[DataCnt1++] = temp;
          	if(RxData1[0] != STX_MAIN)
          	{
              	DataCnt1 = 0;
              	RcvLength1 = 4;
          	}  
          	if(DataCnt1 >= RcvLength1) // check end byte
          	{
              	if (temp == ETX_MAIN) 
              	{
                  	NewData1 = TRUE;
                  	DataCnt1 = 0;
			
              	}
          	}
          	if(DataCnt1 == 4)
          	{ 
              	RcvLength1 = temp;    // Save data length
          	}            
          	break;
	}  
}

BYTE GetChecksum(BYTE *message, BYTE ucLength)
{
	//unsigned char
	WORD i = 0;
	unsigned char tReturn = 0;

	tReturn = message[i];

	for(i = 1; i<ucLength; i++)
	{
		tReturn = tReturn + message[i];	
	}

	tReturn = tReturn & 0x55;                 //

	return tReturn;
}
void Uart0_Init(void)
{
    BYTE dummy;

    UCSR0A=0x02;          // Double Speed Mode
    UCSR0B=0x98;          // USART0 Receiver: On,  USART0 Transmitter: On
    UCSR0C=0x06;          // Communication Parameters: 8 Data, 1 Stop, No Parity
//  UBRRL=0x0C;       // USART0 Baud rate: 38400 (Double Speed Mode) - 4MHz
    UBRR0L=51;         // USART0 Baud rate: 19200 (Double Speed Mode) - 16MHz
    UBRR0H=0x00;

    dummy = UDR0;                     // dummy read	
}

void UART_H2A(u8 conv)
{
	//UART0_TX_EN;
	u8 temp;
	
	temp = (conv&0xF0)>>4;
	if(temp/10) temp += 7;
	temp += '0';
	UART_TX(temp);
	
	temp = conv&0x0F;
	if(temp/10) temp += 7;
	temp += '0';
	UART_TX(temp);

	//UART0_RX_EN;
}


void UART_TX(u8 tx)
{
	UART0_TX_EN;

	while ( !( UCSR0A & (1<<UDRE)));
	UDR0 = tx;

	UART0_RX_EN;
}

void UART_STRING(char *str)
{
	//UART0_TX_EN;
	u8 i=0;
  	do{
			UART_TX(str[i]);
   	}while(str[++i]!='\0');
	//UART0_RX_EN;
}


void UART0_PUT_BYTE(BYTE Txdata)		// UART Transmitt
{	
    while(!(UCSR0A & 0x20));               // UDRE = '1'?
    UDR0 = Txdata;
}

void UART0_PUT_STRING(BYTE *str, BYTE size)
{
    BYTE i;
    for(i=0; i<size; i++)
    {           
        UART0_PUT_BYTE(*str++);
    }    
}


void UART0_PUTCH(char Txdata)		// UART Transmitt
{
	UART0_TX_EN;	
    while(!(UCSR0A & 0x20));               // UDRE = '1'?
    UDR0 = Txdata;
	delay_ms(1);
	UART0_RX_EN;
}
/*
void Uart1_Init(void)
{
    BYTE dummy;
	UBRR1H=0x00;
	 UBRR1L=51;         // USART0 Baud rate: 19200 (Double Speed Mode) - 16MHz
	
    UCSR1A=0x02;          // Double Speed Mode
    UCSR1B=0x98;          // USART0 Receiver: On,  USART0 Transmitter: On
    UCSR1C=0x06;          // Communication Parameters: 8 Data, 1 Stop, No Parity
//  UBRRL=0x0C;       // USART0 Baud rate: 38400 (Double Speed Mode) - 4MHz
   
    

    dummy = UDR1;                     // dummy read	
}
*/

void Uart1_Init(void)
{
    BYTE dummy;
	UBRR1H=0x00;
	 UBRR1L=25;         // USART0 Baud rate: 19200 (Double Speed Mode) - 16MHz
	
    UCSR1A=0x00;          // Double Speed Mode
    UCSR1B=0x98;          // USART0 Receiver: On,  USART0 Transmitter: On
    UCSR1C=0x06;          // Communication Parameters: 8 Data, 1 Stop, No 

//  UBRRL=0x0C;       // USART0 Baud rate: 38400 (Double Speed Mode) - 4MHz
}

void UART1_PUT_BYTE(BYTE Txdata)      // UART Transmitt
{   
	while(!(UCSR1A & 0x20));               // UDRE = '1'?
	UDR1 = Txdata;
}

void UART1_PUT_STRING(BYTE *str, BYTE size)
{
	BYTE i;
	for(i=0; i<size; i++)
	{           
	UART1_PUT_BYTE(str[i]);
	}    
}

void UART1_PUTCH(char Txdata)      // UART Transmitt
{   
	while(!(UCSR1A & 0x20));               // UDRE = '1'?
	UDR1 = Txdata;
}

void UART1_HEX2ASCII(char SD)
{
    BYTE temp;

    temp = SD;
    temp &= 0xf0;   
    temp >>= 4;
    if(temp >= 10) 
    {
        temp += 7;    
    }
    temp += '0'; 
   
    UART1_PUTCH(temp);
      
    temp=SD;
 
    temp &= 0x0f;
    if(temp >= 10)    
    {
        temp += 7;  
    }
    temp += '0';
    UART1_PUTCH(temp);
}


void UART_HEX2ASCII(char SD)
{
    BYTE temp;

    temp = SD;
    temp &= 0xf0;   
    temp >>= 4;
    if(temp >= 10) 
    {
        temp += 7;    
    }
    temp += '0'; 
	
    UART0_PUTCH(temp);
		
    temp=SD;
 
    temp &= 0x0f;
    if(temp >= 10) 	
    {
		temp += 7;  
    }
    temp += '0';
    UART0_PUTCH(temp);
}

char Ascii2Hex(void)
{
	BYTE temp;

	temp = AsciiBuf[0];

    if((temp >= 'A' && temp <= 'F') || (temp >= 'a' && temp <= 'f')) temp += 9;

    temp &= 0x0f; 
	temp <<= 4; 
	AsciiBuf[0] = temp;

    temp = AsciiBuf[1];
    if((temp >= 'A' && temp <= 'F') || (temp >= 'a' && temp <= 'f')) temp += 9;
    temp &= 0x0f; 
	temp |= AsciiBuf[0];

    return(temp);
}

void PUT_STRING(const char *str)
{
    int cnt;
//	int temp = 0;
    for(cnt=0; *(str+cnt) !=0; cnt++)
    {
        if(*(str+cnt)=='\a') UART0_PUTCH(BEL);
		if(*(str+cnt)=='\b') UART0_PUTCH(BS);
		if(*(str+cnt)=='\n') //UART_PUTCH(LF);
		if(*(str+cnt)=='\r') UART0_PUTCH(CR);
		UART0_PUTCH(*(str+cnt));
    }
}



