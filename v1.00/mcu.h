/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2011, Messerschmitt Korea
*
* Hardware : RCS-100DL
* Filename : mcu.h
* Version  : 1.20
* Programmer(s): Jong-min Kim
* Created : 2011/10/25
* Description : 
********************************************************************************
*/
#ifndef _MCU_H
#define _MCU_H


//==============================================================================
//MCU IO Define
//==============================================================================
typedef union{
	uint8_t ALL; //The sfrb as 1byte
	struct{ //The sfrb as 8 bits
		uint8_t B0:1,
				B1:1,
				B2:1,
				B3:1,
				B4:1,
				B5:1,
				B6:1,
				B7:1;
	};
} IOREG8;

#define SFR_IO8(addr)(*(volatile IOREG8 *)(addr+0x20))

#define OUTA	(SFR_IO8(0x1B))
#define SETA	(SFR_IO8(0x1A))
#define INA		(SFR_IO8(0x19))

#define OUTB	(SFR_IO8(0x18))
#define SETB	(SFR_IO8(0x17))
#define INB		(SFR_IO8(0x16))

#define OUTC	(SFR_IO8(0x15))
#define SETC	(SFR_IO8(0x14))
#define INC		(SFR_IO8(0x13))

#define OUTD	(SFR_IO8(0x12))
#define SETD	(SFR_IO8(0x11))
#define IND		(SFR_IO8(0x10))

#define OUTE	(SFR_IO8(0x03))
#define SETE	(SFR_IO8(0x02))
#define INE		(SFR_IO8(0x01))

#define OUTF	(SFR_IO8(0x42))
#define SETF	(SFR_IO8(0x41))
#define INF		(SFR_IO8(0x40))

#define OUTG	(SFR_IO8(0x45))
#define SETG	(SFR_IO8(0x44))
#define ING		(SFR_IO8(0x43))

//==============================================================================
//BUZZER Interface define
//==============================================================================
#define BUZZER			OUTB.B7


//==============================================================================
//LED Interface define
//==============================================================================
//#define BLUE_LED		OUTD.B6
//#define RED_LED		OUTD.B7
//#define BLUE_LED		OUTD.B7

/*
#define LED_BLUE		OUTB.B2//OUTB.B0
#define LED_GREEN		OUTB.B0
#define LED_RED			OUTB.B1//OUTB.B2
*/
#define LED_BLUE		OUTB.B0//OUTB.B0
#define LED_WHITE		OUTB.B1//OUTB.B1
#define LED_RED			OUTB.B2//OUTB.B2	
#define WDT				PORTB=PORTB^0x40//OUTA.B2
#define RELAY			OUTB.B3
#define SWITCH_IN		INE.B3
//#define LED2	OUTD.B1
//#define LED3	OUTD.B2

//------------------------------------------------------------------------------
// MCU I/O Bit Control Define
//------------------------------------------------------------------------------
#define BUZZER_ON           PORTB = PORTB | _BV(7);   	// Buzzer On	
#define BUZZER_OFF          PORTB = PORTB & ~_BV(7);   	// Buzzer OFF
//-------------------------------
#define BLUE_LED_ON        	PORTB = PORTB | _BV(0);   	// Blue LED ON
#define BLUE_LED_OFF       	PORTB = PORTB & ~_BV(0);   	// Blue LED OFF
#define WHITE_LED_ON        PORTB = PORTB | _BV(1);   	// White LED ON
#define WHITE_LED_OFF       PORTB = PORTB & ~_BV(1);   	// White LED OFF
#define RED_LED_ON          PORTB = PORTB | _BV(2);   	// Red LED ON
#define RED_LED_OFF         PORTB = PORTB & ~_BV(2);   	// Red LED OFF


//#define WHITE_LED_ON        PORTD = PORTD | _BV(6);   	// Green LED ON
//#define WHITE_LED_OFF       PORTD = PORTD & ~_BV(6);   	// Green LED OFF
//#define RED_LED_ON          PORTD = PORTD | _BV(7);   	// Red LED ON
//#define RED_LED_OFF         PORTD = PORTD & ~_BV(7);   	// Red LED OFF

#define UART0_TX_EN         PORTE = PORTE | _BV(2);
#define UART0_RX_EN         PORTE = PORTE & ~_BV(2);
 
//#define DOOR_ADDR0          PINC & 0x40;         		// Security Set Button Input
//#define DOOR_ADDR1          PINC & 0x80;

#define RELAY_ON           PORTB = PORTB | _BV(3);   	// Relay On	
#define RELAY_OFF          PORTB = PORTB & ~_BV(3);   	// Relay OFF

//==============================================================================
//TRH Port Interface define
//==============================================================================
#define TRH_RSTH	PORTC |= 0x80
#define TRH_RSTL	PORTC &= ~0x80

#define TRH_ALEH	PORTC  |= 0x40			// Pale
#define TRH_ALEL	PORTC  &= ~0x40

#define TRH_NCSH	PORTC |= 0x20
#define TRH_NCSL	PORTC &= ~0x20

#define TRH_NWRH	PORTC |= 0x10
#define TRH_NWRL	PORTC &= ~0x10

#define TRH_NRDH	PORTC |= 0x08
#define TRH_NRDL	PORTC &= ~0x08	


//------------------------------------------------------------------------------
// RFID Chip Control Define
//------------------------------------------------------------------------------ 
//#define ON           0x01
//#define OFF          0x00
#define TRUE         1
#define FALSE        0
#define ACK          1
#define NAK          0

//------------------------------------------------------------------------------
// UART Command Define
//------------------------------------------------------------------------------
#define STX               	0xF1      // Start of Frame
#define ETX               	0xF2      // End of Frame

#define STX_READER          0x22      // Start of Frame
#define ETX_READER          0x23      // Start of Frame

//#define UID_CHK           0xA0      // Card Certification
#define SECURITY_SET      	0xB0      // Security Setting
#define OPEN_SET          	0xB4      // Open Mode Setting
//#define EMERGENCY         0xC0      // Emergency Generation

//===============Smart Mecha Define======================================
//#define STX_0             	0x88      // Start of Frame
//#define STX_R				0x99		// Return Start if Frane
//#define STX_1             	0x02      // Start of Frame
//#define ETX               	0x03      // End of Frame

#define UID_CHK           	0xA0      // Card Certification
#define DATA_CHK          	0xA1      // Card Certification
#define REQ_UID           	0xC0      // Request Card Data
#define DOOR_MODE         	0xC1      // Door Mode Setting(0 = Block ID, 1 = UID)
#define DOOR_CON          	0xE0      // Door Lock/Unlock Control
#define AUTODOOR_CON      	0xE1      // Door Auto Control

//------------------------------------------------------------------------------
// EEPRom Define
//------------------------------------------------------------------------------
#define DEV_MODE_SET		0

//------------------------------------------------------------------------------
// CRC Parameter Define
//------------------------------------------------------------------------------ 
typedef unsigned int  width; 

/*  The CRC16 parameters.                        */
#define POLYNOMIAL          0x8005
#define INITIAL_REMAINDER   0x0000
#define FINAL_XOR_VALUE     0x0000
 
/* The width of the CRC calculation and result */
#define WIDTH   (8 * sizeof(width))
#define TOPBIT  (1 << (WIDTH - 1))


#ifdef _SET_GLOBAL_
		#define _GLOBAL_
#else
		#define _GLOBAL_   extern
#endif

extern volatile unsigned char TOV1Count, TOV0Count, TOV2Count;
//extern volatile unsigned char T3OVF;
extern volatile unsigned char T0IRQ, T1IRQ, T2IRQ;
extern volatile unsigned char RcvData0;
extern volatile unsigned char RcvCmd0;
extern volatile unsigned char NewData0;
extern unsigned char RxData0[255];
extern unsigned char tx_data[60];
//extern volatile unsigned char DoorAddr;
extern volatile unsigned char NEW_CARD;
extern volatile unsigned char DETECT_CARD;
extern volatile unsigned char REQUEST_CARD;
extern volatile unsigned char NORMAL_RESET;
//extern volatile unsigned char OPEN_RESET;
//extern volatile unsigned char NormalOpenMode;
extern volatile unsigned char DETECT_14443A;
extern volatile unsigned char DETECT_14443A_MEM;

//extern volatile unsigned char DoorOpenFg, DoorCloseFg;
//extern volatile unsigned char GreenLedFg, RedLedFg;
//extern volatile unsigned char BuzzerOnFg;
//extern volatile unsigned char ResetFg;
extern volatile unsigned int CrcTable[256];
//extern unsigned char CARD_DATA[20];
extern unsigned char CardBuf[20];
extern unsigned char TotalData[48];
extern volatile unsigned char DeviceMode;
extern volatile unsigned char ReaderAddr;
extern volatile unsigned char Time;
extern char AsciiBuf[2];
extern volatile unsigned int CountSendEnter;
_GLOBAL_ volatile BYTE rtc_sec;
_GLOBAL_ volatile BYTE rtc_min;
_GLOBAL_ volatile BYTE rtc_hour;
_GLOBAL_ volatile BYTE rtc_week;
_GLOBAL_ volatile BYTE rtc_date;
_GLOBAL_ volatile BYTE rtc_month;
_GLOBAL_ volatile BYTE rtc_year;
_GLOBAL_ volatile BYTE FloorTime, CallTime,RLedTime;
//------------------------------------------------------------------------------
//each other TAG UID& Reseponse variable
//------------------------------------------------------------------------------
_GLOBAL_ volatile BYTE BUFFER[3];
_GLOBAL_ volatile BYTE CARD_DATA[50];
_GLOBAL_ volatile BYTE DETECT_15693;

_GLOBAL_ volatile WORD pEnterSend; 
_GLOBAL_ WORD pEnterSave;
_GLOBAL_ BYTE OverwriteState;

_GLOBAL_ volatile BYTE SpAreaNo;
//------------------------------------------------------------------------------
// BLE
//------------------------------------------------------------------------------
_GLOBAL_ volatile WORD uiBlePower;


//==============================================================================
//Firmware Version
//==============================================================================
#define Version0	1;
#define Version1	0;
#define Version2	2;

//------------------------------------------------------------------------------
// System Initiation Function
//------------------------------------------------------------------------------
void System_Init(void);
void MCU_Init(void);
void TRH031M_Init(void);
//void Init_Doorlock_Address(void);
void TransmitCardData(void);
void TransmitTest(unsigned char command, unsigned char Addr);
void Init_Device_Mode(void);
void Init_Reader_Address(void);
//------------------------------------------------------------------------------
// USART Function
//------------------------------------------------------------------------------
//void Uart0_Init(void);
//------------------------------------------------------------------------------
// RFID Chip Control Function
//------------------------------------------------------------------------------
void Write(unsigned char Command, unsigned char Address);
unsigned char Read(unsigned char Address);
void ISO14443A_UID(void);
void ISO14443A_Memory_LOOP(unsigned char ucCommand);
//void ISO14443A_HID(void);
void ISO14443A_HID(unsigned char nAddress);
//------------------------------------------------------------------------------
// Timer/Delay Function
//------------------------------------------------------------------------------
void delay_us(unsigned int time_us);
void delay_ms(unsigned int time_ms);
//------------------------------------------------------------------------------
//  UART Fucntions
//------------------------------------------------------------------------------
void Check_Main_Data(void);
/*
void Uart0_Disabled(void);
void UART_INIT(int BAUD);
void UART0_PUTCH(char Txdata);
void UART0_PUTSTR(char *pt);
void UART0_HEX2ASCII(char SD);
char Ascii2Hex(void);
void PUT_STRING(const char *str);
void UART0_PUT_BYTE(unsigned char Txdata);
void UART0_PUT_STRING(unsigned char *str, unsigned char size);
void UART_Rcv_Data(void);
void UART_Rcv_Command(void);
void UART_Rcv_Chk(void);
unsigned char UART_RcvDataChk(void);
*/
void DoorRunFunction(void);
width CrcCompute(unsigned char *message, unsigned int nBytes);
//------------------------------------------------------------------------------
//  Card Fucntions
//------------------------------------------------------------------------------
//  void Card_Function(void);
void ReadCardMemory(unsigned char Command);
void ReadCardUID(unsigned char Command);
void TransmitCardID(void);
void Transmit_ACU(unsigned char ucCommand);
void Transmit_OpenMode(unsigned char ucData);
void TransmitNoneCardData(unsigned char ucCommand);
void TransmitDefaultData(unsigned char ucCommand);

void Timer0_Init(void);
void Timer0_mSec_Start(unsigned int msecond);
void Timer0_mSec_Stop(void);
void Timer1_Init(void);
void Timer1_Sec_Start(unsigned char second);
void Timer1_Sec_Stop(void);
void Timer2_Init(void);
void Timer2_mSec_Start(unsigned char msecond);
void Timer2_mSec_Stop(void);
void Timer3_Init(void);
void Timer3_1Sec_Start(unsigned char sec);
void Timer3_1Sec_Stop(void);

void Doorlock_Open_B(void);
void Doorlock_Close_B(void);
//void Timer3_Start(BYTE sec);
//void Timer3_Stop(void);

void All_LED_On(void);
void All_LED_Off(void);
void Blue_LED_On(void);
void Green_LED_On(void);
void Red_LED_On(void);

void Buzzer_On(void);
void Buzzer_Off(void);

void BLE_Rcv_Command(void);
void BLE_Cmd_Function(void);
void BLE_CardResult_Send(void);
void BLE_DataTest(u8 data);
void BLE_AdPk_Set(void);
void BLE_Time_Send(void);
void BLE_Test1(void);
void BLE_Time_Send1(void);

void Init_Set_Data(void);

#endif
