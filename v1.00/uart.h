
#ifndef _UART_H
#define _UART_H

#define BYTE  unsigned char
#define WORD  unsigned int

//------------------------------------------------------------------------------
// 							UART
//------------------------------------------------------------------------------
#define UART0_DIR		OUTE.B2
#define UART0_TX		(UART0_DIR = 1);
#define UART0_RX		(UART0_DIR = 0);

//------------------------------------------------------------------------------
// UART Command Define
//------------------------------------------------------------------------------
#define STX_MAIN            0x7B      // Start of Frame
#define ETX_MAIN            0x7D      // End of Frame
#define STX_ACU             0x28      // Start of Frame
#define ETX_ACU             0x29      // End of Frame

#define SET_TIMECODE            0xB0        // Security Setting
#define SET_DEFAULT             0xB1        // 
#define mobily_key			0xB5
#define SET_HOTEL_CODE          0x01        // Hotel Code Setting
#define SET_CONTACT_TIME        0x02        // Set Contact Time

#define SET_FLOOR               0xB2        //
#define SET_CURRENT_TIME        0xB3        //
#define SET_SPEACIAL_AREA       0xB4        //
//#define CARD_REQ                0xB4      // Open Mode Setting

#define REQ_SET_CODE            0xB8
#define SUB_REQ_HOTEL_CODE      0x01
#define SUB_REQ_CONTACT_TIME    0x02
#define SUB_REQ_CURRENT_TIME    0x03
#define SUB_REQ_FLOOR_SET       0x04
#define SUB_REQ_AREA_SET        0x05

//#define REQ_REAL_TIME     0xB6          // Request Current Time

#define REQ_ENTER_DATA          0xC3
#define REQ_ENTER_DATA_SELECT   0xC4
#define REQ_ENTER_SET           0XA0
#define REQ_CARD_DATA           0xB5
#define REQUEST_CARD_DATA       0XC0
#define READER_CARD_REQ         0xA0      // Open Mode Setting
#define READER_CARD_DATA        0xB4

#define SUB_CARD_DATA           0xA3
#define SUB_MODE_DATA           0xA2

//NFC Command
#define REQ_NCARD_DATA         	0xB5
#define SET_NRELAY_TIME        	0xBA  
#define REQ_NRELAY_TIME        	0xB8
#define SET_NHOTEL_CODE  		0xB1
#define REQ_NHOTEL_CODE  		0xB7
#define SET_NCURRENT_TIME 		0xB3
#define REQ_NCURRENT_TIME 		0xC4
#define SET_NSPEACIAL_AREA 	0xA6
#define REQ_NSPEACIAL_AREA 	0xB4
#define REQ_NENTER_DATA 		0xC3 
#define SUB_REQ_ENTER_DATA 	0x00
#define SUB_REQ_ENTER_SEL 		0x01
#define REQ_NINFOMATION 		0xC1
#define SET_NPARAMETER			0xA1
#define REQ_NPARAMETER			0xC5
#define SET_NINITIALIZE 			0xA5
#define SET_NBLE_POWER			0xD7	
#define REQ_NBLE_POWER			0xD8
//--------TEST NFC Command----------
#define SET_NTIME_SLOT                0xA4
#define SET_NSTAFF_TIME	       0xA7
#define SET_NGROUP_NUMBER		0xAA
#define SET_NELE_FLOOR 		       0xB2
#define REQ_NELE_FLOOR		      	0xB6
#define SET_NDOOR_ADDRESS		0xBB
#define REQ_NDOOR_ADDRESS		0xBC
#define REQ_NTIME_SLOT 		       0xC2
#define SET_NBUILDING_NO 		0xD0	
#define REQ_NBUILDING_NO 		0xD1
#define SET_NFLOOR_NUMBER		0xD2
#define REQ_NFLOOR_NUMBER		0xD3
#define DELETE_ALL_EVENT	 	0xD4
#define SET_NCARD_SECTOR		0xD5
#define SET_NCARD_SECTORKEY	0xD6
#define DOOR_NLOCK_CONTROL	0x10
#define DOOR_NOPEN_CONTROL      0x11 
//--------------------------------------------------------------------------
//ONline Command
#define SET_PARAMETER_ON				0XA1
#define SET_STAFF_GROUP_MASK_ON		0XA2
#define SET_GUEST_WORKTIME_ON  		0xA4 //2020.11.12 김태성 사원 수정
#define SET_INITIALIZE_ON        			0xA5
#define SET_SPECIAL_AREA_ON     	  		0xA6
#define SET_STAFF_WORKTIME_ON		0xA7 //2020.11.12 김태성 사원 수정
#define SET_GUEST_CKOUTTIME_ON      	0xA8
#define SET_GROUP_NUMBER_ON			0xAA
#define SET_GROUP_SECURITY_ON			0xAB
#define EXTEND_CHECKOUTDATE_ON		0xAC

#define SET_CURRENT_TIME_ON 	       	0xB3

#define REQ_ONLINECHECK_ON			0xC0 //2020.11.12 김태성 사원 수정
#define REQ_INFORMATION_ON	       	0xC1
#define REQ_GUEST_WORKOUTTIME_ON      	0xC2
#define REQ_EVENTS_ON	       			0xC3
#define REQ_CURRENT_TIME_ON			0xC4
#define REQ_PARAMETER_ON				0xC5
#define SET_STAFF_SECURITY_ON			0xC6
#define SET_ALL_STAFF_SECURITY_ON		0xC7
#define REQ_STATUS_ON					0xCA
#define REQ_ACU_FALLING				0xE1 //2020.11.12 김태성 사원 수정, 우이동 현장
//--------------------------------------------------------------------------

extern BYTE RxData0[255];
extern BYTE tx_data0[250];
extern BYTE TxData0[250];
extern volatile BYTE NewData0, NewData1, NewData;
BYTE RxData1[255];
BYTE TxData1[255];
extern BYTE DataCnt1, RcvLength1;
extern volatile BYTE RE_flagm,RE_tmp,RE_Com,RE_SCom,RE_mode,RE_flag;
extern  volatile WORD RE_SA,RE_EA,RE_cnt,RE_ecnt,RE_index;

//--------------------------------------------------------------------------//
//                              UART Fucntions                              //
//--------------------------------------------------------------------------//
void Uart0_Init(void);
void UART0_PUTCH(char Txdata);
void UART0_PUTSTR(char *pt);
void UART0_HEX2ASCII(char SD);
void PUT_STRING(const char *str);
void UART0_PUT_BYTE(BYTE Txdata);
void UART0_PUT_STRING(BYTE *str, BYTE size);

void Uart1_Init(void);
void UART1_PUT_BYTE(BYTE Txdata);
void UART1_PUT_STRING(BYTE *str, BYTE size);
void UART1_PUTCH(char Txdata);
void UART1_HEX2ASCII(char SD);
BYTE GetChecksum(BYTE *message, BYTE ucLength);
void TransmitServerTest(void);
void Transmit_Acknak(void);
void Transmit_ACU(BYTE ucCommand);
void TransmitDecodingCard(void);
void TransmitReadingCard(void);
void Transmit_Date(void);
void Transmit_Data(void);
void Check_Main_Data(void);

#endif
