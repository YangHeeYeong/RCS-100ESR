

#ifndef _24LC256_H
#define _24LC256_H

// 256K bit = 32KB = 32768 Byte
//#define SAVE_COUNT_ADDR   12800 
//------------------------------------------------------------------------------
// External I2C EEPROM
//------------------------------------------------------------------------------
//#define SCL             GET_BITFIELD(&PORTC).bit0
//#define SDA             GET_BITFIELD(&PORTC).bit1



#define SCL 		OUTG.B3
#define SDA			OUTG.B4

#define SDA_H		PORTG=PORTG|0x10
#define SDA_L		PORTG=PORTG&0xEF

#define SCL_H 		PORTG=PORTG|0x08
#define SCL_L 		PORTG=PORTG&0xF7

//=========== SDA ===========//
#define MEM_IO_HIGH        	PORTB = PORTB | _BV(6); 
#define MEM_IO_LOW         	PORTB = PORTB & ~_BV(6);
//=========== SCL ===========//
#define MEM_CLK_HIGH		PORTB = PORTB | _BV(5);
#define MEM_CLK_LOW         PORTB = PORTB & ~_BV(5);


#define SetBit(addr,bit) (addr |=(1<<bit))
#define ClrBit(addr,bit) (addr &=~(1<<bit))
#define ToggleBit(addr,bit) (addr ^=(1<<bit))
#define Chkbit(addr,bit) (addr &(1<<bit))

#define nop()   asm("nop")

//------------------------------------------------------------------------------
// External I2C EEPROM Address Mapping
//------------------------------------------------------------------------------
#define SAVE_COUNT_ADDR   12800     // 32*400=12800
#define eSETSTART       19200      // Setting Start Address
#define eHOTELCODE      1
#define eRoomNumber     3
#define eBusAddress     5
//#define eRedLed		  5
//#define eAREANUMBER     6 
#define eSpecialArea    8 // 
#define eReaderType     9
#define eHotelCodeSet   11 //2020.11.18 김태성 사원 수정
#define eFullWrite 		13
#define eGreenLed       20		// Parameter
#define eRedLed         21		// Parameter
#define eBlinkLed       22		// Parameter
#define eRelayGreenLED  23		// Parameter

#define eAUTOTIME       28
#define eCALLTIME       29
//#define eAREANUMBER     55
#define eSuiteArea      30
#define eMasterArea     31
#define eMaidArea       32
#define eMiniArea       33              // Mini bar Area

#define eGuestSecurity		38	// Guest Security
#define ePreCheckSecurity       40     // Pre-checkin
#define eStandbySecurity        42     // Standby
#define eOnetimeSecurity        44     // Onetime
#define eSuiteSecurity          46     // Suite Security
#define eEmgSecurity  		48	// Emergency Security
#define eResetSecurity	        50	// Reset Security
#define eTimeSecurity	        52	// Time Security
#define eInitSecurity	        54	// Init Security
#define eManagerSecurity	56	// Manager Security
#define eConfSecurity	        58	// Conference Security
#define eOfficeSecurity	        60	// Conference Security

#define eSystemSecurity	        62	// System Security

#define eParameter0	        64	// Parameter
#define eParameter1	        65	// Parameter
#define eParameter2	        66	// Parameter
#define eParameter3	        67	// Parameter
#define eParameter4	        68	// Parameter
#define eParameter5	        69	// Parameter

#define eCheckOutTime1          70     // Checkout Time 1
#define eCheckOutTime2          72     // Checkout Time 2
#define eCheckOutTime3          74     // Checkout Time 3
#define eCheckOutTime4          76     // Checkout Time 4
#define eCheckOutTime5          78     // Checkout Time 5
#define eCheckOutTime6          80     // Checkout Time 6
#define eCheckOutTime7          82     // Checkout Time 7
#define eCheckOutTime8          84     // Checkout Time 8

#define eCheckOutTime9          86     // Checkout Time 9
#define eCheckOutTime10          88     // Checkout Time 10
#define eCheckOutTime11          90     // Checkout Time 11
#define eCheckOutTime12          92     // Checkout Time 12
#define eCheckOutTime13          94     // Checkout Time 13
#define eCheckOutTime14          96     // Checkout Time 14
#define eCheckOutTime15          98     // Checkout Time 15
#define eCheckOutTime16          100     // Checkout Time 16

#define eCheckOutTime17          102     // Checkout Time 17
#define eCheckOutTime18          104     // Checkout Time 18
#define eCheckOutTime19          106     // Checkout Time 19
#define eCheckOutTime20          108     // Checkout Time 20
#define eCheckOutTime21          110     // Checkout Time 21
#define eCheckOutTime22          112     // Checkout Time 22
#define eCheckOutTime23          114     // Checkout Time 23
#define eCheckOutTime24          116     // Checkout Time 24

#define eCheckOutTime25          118     // Checkout Time 25
#define eCheckOutTime26          120     // Checkout Time 26
#define eCheckOutTime27          122     // Checkout Time 27
#define eCheckOutTime28          124     // Checkout Time 28
#define eCheckOutTime29          126     // Checkout Time 29
#define eCheckOutTime30          128     // Checkout Time 30
#define eCheckOutTime31          130     // Checkout Time 31
#define eCheckOutTime32          132     // Checkout Time 32

#define eOVERWRITE      134
#define eENTERSAVE      135
#define eENTERSEND      137
#define eFLOORSET       139
#define eFLOOR          140
#define ePUBLIC         204

#define eTimeSlot1      212//149     // Timeslot 1 = Start time + End Time (4 Byte)

#define eTimeSlotPermanent  362//299 
#define eTimeSlotGuest      366//303

#define eStaffGroup      387//328

#define eStaffTimeSlot	395//336

//#define eRoomGroup1     523//464
//#define eRoomGroup2     525//466
//#define eRoomGroup3     527//468
#define eRoomGroupStart	523	

//#define eRoomSecurity1      651//592
//#define eRoomSecurity2      653//594
//#define eRoomSecurity3      655//596
#define eRoomSecurityStart 731

#define eBuildingNumber	811
#define eFloorNumber		812
#define eBlePower	813

//For Page Write
#define eGMasterSecurity        850//720     // GrandMaster StaffNumber Security
#define eMasterSecurity         1874//1744    // Master StaffNumber Security
#define eMaidSecurity           2898//2768    // Maid StaffNumber Security
#define eMinibarSecurity        3922//3792    // Maid StaffNumber Security
#define eStaffSecurity		4946//4816 
#define officeset	 5969//5852
//#define eMifareAddrSet			5850
//#define eMifareAddr			5851
#define eMifareSectorKeySet	5970//5852
#define eMifareSectorKey		5971//5853
#define eOnetimeFlag			5979
#define eOnetimeChkTime		5980
#define eBluetoothEnable		5986
#define eCheckOutExtend_Flag	5987
#define eCCDate				5988
#define eNCDate				5991
#define edetect_level			5994
#define emod_cond			5995
#define ertc_sec    			5996
#define ertc_min 				5997
#define ertc_hour 			5998
#define ertc_date 			5999
#define ertc_month 			5960
#define ertc_week 			5961
#define ertc_year 			5962

//  Group Room : 0xAA Command protocal CheckOutDate  hy 수정 
#define eGRCkoutDate_year	5963
#define eGRCkoutDate_Mon	5964
#define eGRCkoutDate_day	5965

// RoomChange : 0xAB Command Protocal Card Room Data  hy 수정 
#define eRoomChangeFlage	5966
#define eRoomChangeNo		5967
#define eRoomChangeSecurity		5969


//For Page Write
#define eGMasterSecurity        850//720     // GrandMaster StaffNumber Security
#define eMasterSecurity         1874//1744    // Master StaffNumber Security
#define eMaidSecurity           2898//2768    // Maid StaffNumber Security
#define eMinibarSecurity        3922//3792    // Maid StaffNumber Security
#define eStaffSecurity		4946//4816 
#define officeset	 5969//5852
//#define eMifareAddrSet			5850
//#define eMifareAddr			5851
#define eMifareSectorKeySet	5970//5852
#define eMifareSectorKey		5971//5853
#define eOnetimeFlag			5979
#define eOnetimeChkTime		5980
#define eBluetoothEnable		5986
#define eCheckOutExtend_Flag	5987
#define eCCDate				5988
#define eNCDate				5991
#define edetect_level			5994
#define emod_cond			5995
#define  ertc_sec    			5996
#define    ertc_min 			5997
#define    ertc_hour 			5998
#define    ertc_date 			5999
#define    ertc_month 			5960
#define    ertc_week 			5961
#define    ertc_year 			5962
/*
#define eSETSTART       32000      // Setting Start Address
#define eHOTELCODE      1
#define eAUTOTIME       3
#define eCALLTIME       4
#define eAREANUMBER     5
#define eMASTERAREA     6
#define eMAIDAREA       7
#define eMBAREA         8       // Mini bar Area
#define eOVERWRITE      26
#define eENTERSAVE      27
#define eENTERSEND      29
#define eFullWrite 		31
#define eFLOORSET       100
#define eFLOOR          101
#define ePUBLIC         201
eRedLed
*/
/*
void write_i2c_byte(unsigned char byte);
BYTE read_byte(unsigned char add);
BYTE read_word(unsigned char add);
void write_word(unsigned char ucAddr, unsigned char ucData);
unsigned char read_i2c_byte(unsigned char ch);
void i2c_clock(void);
void nops(unsigned char num);
*/
void i2c_Init(void);
BYTE i2c_Byte_Read(BYTE ucAck);
void Save_Byte_Data(WORD uiAddr, BYTE ucData);
void Save_Word_Data(WORD uiAddr, WORD uiData);
void Save_Page_Data(WORD uiAddr, BYTE nSize, BYTE *ucData);
void Save_Page_Init(WORD uiAddr, BYTE nSize, BYTE ucData);

BYTE i2c_Byte_Read(BYTE ucAck);
BYTE Read_Byte_Data(WORD uiAddr);
WORD Read_Word_Data(WORD uiAddr);
WORD read_word_seeprom(WORD uiAddr);
void Read_Page_Data(WORD uiAddr, BYTE nSize, BYTE *pData);
void ReadExtMemoryPageData(WORD uiAddr, BYTE nSize, BYTE *pData);

#endif
