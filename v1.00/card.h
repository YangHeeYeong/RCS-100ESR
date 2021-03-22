
#ifndef _CARD_H
#define _CARD_H

//------------------------------------------------------------------------------
//                      Type of Reader
//------------------------------------------------------------------------------
#define Type_GuestRoom        0
#define Type_SpecialReader    1
#define Type_StaffReader      2
#define Type_ElevatorReader   3
#define Type_GroupReader      8
//------------------------------------------------------------------------------
//                      CARD TYPE DEFINITION
//------------------------------------------------------------------------------
/*
#define AUTHORIZED_CARD       0
#define CLOCK_CARD            1
#define INSTALL_CARD          2
#define MEETING_CARD          8
#define EMERGENCY_CARD        9
#define MASTER_CARD           10
#define GATE_CARD             11
#define AREA_CARD             12
#define BUILDING_CARD         13
#define FLOOR_CARD            14
#define GUEST_CARD            15
#define BLANK_CARD            255
*/
#define GUEST_CARD            0
#define PRECHECK_CARD         1     // Pre-check in Card
#define STANDBY_CARD          2
#define ONETIME_CARD          3
#define EMERGENCY_CARD        4
#define GRANDMASTER_CARD      5
#define MASTER_CARD           6
#define MAID_CARD             7
#define MINIBAR_CARD          8
#define CASH_CARD             11
#define FUNCTION_CARD         14
#define RESET_CARD            15    // Reset Card
#define TIME_CARD             16    // Time Card
#define INIT_CARD             17    // Init Card
#define PARAMETER_CARD        18    // Parameter Card
#define SETADDRESS_CARD 20	//SetAddress Card
#define Hotelcode_CARD       25 //2020.11.19 김태성 사원 수정
#define HANDHELD_CARD         28    // Dimming Level Setting Card //2020.11.19 김태성 사원 수정

#define HOTELCODE_CARD        36    // Hotel Code Set Card
#define CONTACT_CARD          37    // Contact Card
#define TIME_SET_CARD         38    // Time Set Card
#define AREA_SET_CARD         39    // Contact Card
//----------------------------------------------------------------------------//
//                          ERROR CODE FOR CARD                               //
//----------------------------------------------------------------------------//
#define nEr_HotelCode         1       // hotel code
#define nEr_ValidDate         2       // date
#define nEr_Security          3       // security no
#define nEr_RoomNo            4       // Room No
#define nEr_StaffArea         5       // Staff Area
#define nEr_StaffNo           6       // Staff No      
//#define nEr_StaffIndex      7       // staff index
#define nEr_ChkOutTime        8       // Checkout Time
#define nEr_specialArea       9       // Special area
//#define nEr_doNotDisturb    10      // Do Not Disturb
//#define nEr_staffEntries    11      // Staff Entries
//#define nEr_typeOfCard      12      // Type of Card
#define nEr_Floor             13

#define nEr_Building          14

_GLOBAL_ BYTE BusAddress;
_GLOBAL_ BYTE ReaderType;

_GLOBAL_ BYTE SaveBuf[32];
_GLOBAL_ BYTE ReadBuf[32];
_GLOBAL_ volatile WORD uiHotelCode;
_GLOBAL_ volatile BYTE nCardResult;
_GLOBAL_ volatile BYTE nErrorCode;
_GLOBAL_ WORD EnterSaveCnt, pEnterSave;
_GLOBAL_ WORD uiSecurity, cSecurity;
_GLOBAL_ BYTE ResetState, OverwriteState;
_GLOBAL_ volatile BYTE ContactTime0, ContactTime1, ContactTime2, ContactTime3;
//_GLOBAL_ volatile BYTE SpcArea0[5], SpcArea1[5];
_GLOBAL_ BYTE SubBuf0[20], SubBuf1[20], SubBuf2[20];
_GLOBAL_ volatile BYTE RoomBuffer0, RoomBuffer1, RoomBuffer2;
_GLOBAL_ BYTE ReaderAck0, ReaderAck1, ReaderAck2, ReaderAck3;
_GLOBAL_ volatile BYTE MasterArea, MaidArea, MiniBarArea;
_GLOBAL_ BYTE save_flag;
_GLOBAL_ volatile BYTE fHandheld, HHTime; //2020.11.19 김태성 사원 수정
_GLOBAL_ BYTE Office; //2020.11.19 김태성 사원 수정


extern volatile unsigned int uiHotelCode;
extern volatile unsigned char BuildNo, AreaNo, FloorNo;
extern volatile unsigned char fAuthCard, fSuccess;
extern BYTE ResetState, OverwriteState;
extern WORD EnterSaveCnt, pEnterSave;
extern BYTE SaveBuf[32];


void GetSettingCard(void);
void GetGuestCard(void);
void GetStandbyCard(void);
void GetGuestSpecialArea(void);
void GetStaffCard(void);
void GetEmergencyCard(void);
void GetGrandmasterCard(void);
void GetCashCard(void);
void GetFunctionCard(void);
void GetResetCard(void);
void GetTimeCard(void);
void GetInitCard(void);
void CheckCardClass(void);

#endif
