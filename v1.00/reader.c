
#include "common.h"

volatile BYTE DETECT_CARD, NEW_CARD, NORMAL_RESET;
volatile BYTE MyBuf[50];

void TransmitDecodingCard(void)
{   
    BYTE i;
    UART0_TX_EN;
    delay_ms(1);  
    
    TxData0[0] = 0x7B;
    for(i=0;i<30;i++)
    {
        TxData0[1+i] = MyBuf[i];
    }
    TxData0[31] = 0x7D;    
	
    UART0_PUT_STRING(TxData0, 32);        
    delay_ms(4);
    UART0_RX_EN;
}

void TransmitReadingCard(void)
{   
    BYTE i;
    UART0_TX_EN;
    delay_ms(1);  
    
    TxData0[0] = 0x7B;
    for(i=0;i<25;i++)
    {
        TxData0[1+i] = CARD_DATA[i];
    }
    TxData0[26] = 0x7D;    
	
    UART0_PUT_STRING(TxData0, 27);        
    delay_ms(4);
    UART0_RX_EN;
}

void DecodeHotelCode(void)
{
    WORD tbuf0, tbuf1;
    WORD temp0; 
    
    tbuf0 = CARD_DATA[0];
    tbuf1 = CARD_DATA[1];
    tbuf0 = (tbuf0 & 0x0f) << 6;
    tbuf1 = (tbuf1 & 0xfc) >> 2;
    temp0 = tbuf0 + tbuf1;      // 10 bit
    
    MyBuf[1] = (BYTE)(temp0 >> 8);
    MyBuf[2] = (BYTE)temp0;    
}

void DecodeRoomNumber(void)
{
    WORD tbuf0, tbuf1, tbuf2;
    WORD temp0;  

    tbuf0 = CARD_DATA[1];
    tbuf1 = CARD_DATA[2];
    tbuf2 = CARD_DATA[3];
    tbuf0 = (tbuf0 & 0x03) << 10;   // 0000 1100 0000 0000
    tbuf1 = (tbuf1 & 0xff) << 2;    // 0000 0011 1111 1100 
    tbuf2 = (tbuf2 & 0xc0) >> 6;    // 0000 0000 0000 0011
    temp0 = (tbuf0 | tbuf1 | tbuf2);  // 12 bit
    MyBuf[3] = (BYTE)(temp0 >> 8);    
    MyBuf[4] = (BYTE)temp0;      
}  

void DecodeSecurityNumber(void)
{
    WORD tbuf0, tbuf1;
    WORD temp0;   
      
    tbuf0 = CARD_DATA[3];
    tbuf1 = CARD_DATA[4];                
    tbuf0 = (tbuf0 & 0x3f) << 3;    // 0000 0001 1111 1000  (6 bit)
    tbuf1 = (tbuf1 & 0xe0) >> 5;    // 0000 0000 0000 0111  (3 bit)
    temp0 = (tbuf0 | tbuf1);        // 9 bit
    MyBuf[5] = (BYTE)(temp0 >> 8);  // High Byte
    MyBuf[6] = (BYTE)temp0;       
}

void DecodeValidData(void)
{
    BYTE  temp;
    
    // date ���� 
    temp = CARD_DATA[4];
    MyBuf[7] = temp & 0x1f;    
    // month  ���� 
    temp = CARD_DATA[5];
    MyBuf[8] = (temp & 0xf0) >> 4;     
    //year ���� 
    temp = CARD_DATA[5];
    MyBuf[9] = temp & 0x0f;                     
}

void DecodeStaffNumber(void)
{
    WORD tbuf0, tbuf1, tbuf2;
    WORD temp0;   

    tbuf0 = CARD_DATA[1];
    tbuf1 = CARD_DATA[2];  
    tbuf2 = CARD_DATA[3];
    tbuf0 = (tbuf0 & 0x03) << 10;     // 0000 1100 0000 0000  (2 bit)
    tbuf1 = (tbuf1 & 0xff) << 2;      // 0000 0011 1111 1100  (8 bit)
    tbuf2 = (tbuf2 & 0xc0) >> 6;      // 0000 0000 0000 0011  (2 bit)
    temp0 = (tbuf0 | tbuf1 | tbuf2);  // 12bit 
    MyBuf[3] = (BYTE)(temp0 >> 8);    // High Byte 
    MyBuf[4] = (BYTE)temp0;      
}

void DecodeStaffArea(void)
{
    BYTE tbuf0;
            
    tbuf0 = CARD_DATA[7];  
    tbuf0 = (tbuf0 & 0xfc) >> 2;     // 0011 1111  (6 bit)
    MyBuf[12] = tbuf0;
}

void DecodeSpecialArea(void)
{
    WORD tbuf0, tbuf1, tbuf2;
    WORD temp0;  
    tbuf0 = CARD_DATA[7];
    tbuf1 = CARD_DATA[8];
    tbuf2 = CARD_DATA[9];
    tbuf0 = (tbuf0 & 0x03) << 14;   // 1100 0000 0000 0000  (2 bit)
    tbuf1 = tbuf1 << 6;             // 0011 1111 1100 0000  (8 bit)
    tbuf2 = (tbuf2 & 0xfc) >> 2;    // 0000 0000 0011 1111
    temp0 = (tbuf0 | tbuf1 | tbuf2);  
    MyBuf[13] = (BYTE)(temp0 >> 8);    
    MyBuf[14] = (BYTE)temp0;
    
    tbuf0 = CARD_DATA[9];
    tbuf1 = CARD_DATA[10];
    tbuf2 = CARD_DATA[11]; 
    tbuf0 = (tbuf0 & 0x03) << 14;   // 1100 0000 0000 0000  (2 bit)   
    tbuf1 = tbuf1 >> 6;             // 0011 1111 1100 0000  (8 bit)
    tbuf1 = tbuf1 << 6;             // 0011 1111 1100 0000  (8 bit)
    tbuf2 = (tbuf2 & 0xfc) >> 2;    // 0000 0000 0011 1111 
    temp0 = (tbuf0 | tbuf1 | tbuf2);  
    MyBuf[15] = (BYTE)(temp0 >> 8);    
    MyBuf[16] = (BYTE)temp0; 
    
    tbuf0 = CARD_DATA[11];
    tbuf1 = CARD_DATA[12];   
    tbuf0 = (tbuf0 & 0x03) << 6;    // 0000 0000 1100 0000 (2 bit)
    tbuf1 = (tbuf0 & 0xfc) >> 2;    // 0000 0000 0011 1111 (8 bit)    
    temp0 = (tbuf0 | tbuf1);
    MyBuf[17] = (BYTE)temp0; 
}

BYTE DecodeExtentionClass(void)
{
    BYTE tbuf0, tbuf1;
    BYTE temp0;   
      
    tbuf0 = CARD_DATA[1];
    tbuf1 = CARD_DATA[2];                
    tbuf0 = (tbuf0 & 0x03) << 3;    // 0001 1000 (2 bit)
    tbuf1 = (tbuf1 & 0xe0) >> 5;    // 0000 0111 (3 bit)
    temp0 = (tbuf0 | tbuf1);        // 5 bit  
    
    return temp0;
}

void DecodeCardData(void)
{
    BYTE temp = 0;
    WORD temp1, temp2, temp3;
    BYTE card_class;	
    BYTE setting_class;
    BYTE setting_plus;
	
    temp = CARD_DATA[0];		
    MyBuf[0] = (temp >> 4) & 0x0f;	// ī���� class�� �����ϱ� ����
    card_class = MyBuf[0];
    
    switch(card_class)
    {
        case 0:	// guest card
            DecodeHotelCode();        // Get Hotel Code
            DecodeRoomNumber();       // Get Room Number    
	    DecodeSecurityNumber();   // Get Scurity Number    
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time ���α׷� ���� ���� 
	    MyBuf[11] = 0x00;				// null �Է� 
	    // staff ���� ���� 
	    MyBuf[12] = 0x00;				// null �Է�        
	    DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ� 
	    // time code ���� 
	    //temp = CARD_DATA[13];
	    temp = CARD_DATA[6];		// �Ե� ���α׷� updata�� ��ġ ���� ��.
	    temp1 = temp&0x1c;
	    temp = temp1/4;		// ex) 0001 1100�� 0000 0111�� ����� ����.��������� right��  2ĭ shift ��Ų ��� 
	    MyBuf[18]  = temp;
            break;
            
	case 0x02:		// standby card
            DecodeHotelCode();        // Get Hotel Code  
            DecodeRoomNumber();       // Get Room Number    
            DecodeSecurityNumber();   // Get Scurity Number  
            DecodeValidData();        // Decode Valid Date
	    //index
	    MyBuf[10] = 0x00;				// ��� �� ��. null ���� �Է� 	
	    // time ���α׷� ���� ���� 
	    MyBuf[11] = 0x00;				// null �Է� 
	    // staff ���� ���� 
	    MyBuf[12] = 0x00;				// null �Է� 
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�                     
	    // time code ���� 
	    MyBuf[18]  = 0x00;
	    break;
                
	case 0x03:		// onetime card
            DecodeHotelCode();        // Get Hotel Code  
            DecodeRoomNumber();       // Get Room Number    
            DecodeSecurityNumber();   // Get Scurity Number  
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time ���α׷� ���� ���� 
	    MyBuf[11] = 0x00;				// null �Է� 
	    // staff ���� ���� 
	    MyBuf[12] = 0x00;				// null �Է� 
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�           
	    // time code ���� 
	    temp = CARD_DATA[13];
	    temp1 = temp&0x1c;
	    temp = temp1/4;		            // ex) 0001 1100�� 0000 0111�� ����� ����.��������� right��  2ĭ shift ��Ų ��� 
	    MyBuf[18]  = temp;		
	    break;
            
	case 0x04:  // Emergency card
            DecodeHotelCode();        // Get Hotel Code
            DecodeStaffNumber();
            DecodeSecurityNumber();   // Get Scurity Number
	    // date ���� 
	    MyBuf[7] = 0x00;		    // null �Է� 
	    // month  ���� 
	    MyBuf[8] = 0x00;		    // null �Է� 
	    //year ���� 
	    MyBuf[9] = 0x00;		    // null �Է� 
	    // index ���� 
	    MyBuf[10] = 0x00;				// null �Է� 
	    // time ���α׷� ���� ���� 
	    MyBuf[11] = 0x00;				// null �Է� 
	    // staff ���� ���� 
	    MyBuf[12] = 0x00;				// null �Է� 
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�
	    // time code ���� 
	    MyBuf[18]  = 0x00;				// null �Է� 
	    break;
            
	case 0x05:		// grand master card
	    DecodeHotelCode();        // Get Hotel Code
            DecodeStaffNumber();
            DecodeSecurityNumber();   // Get Scurity Number
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time ���α׷� ���� ���� 
	    temp = CARD_DATA[6];
	    temp = temp & 0x1f ;
	    MyBuf[11] = temp;
	    // staff ���� ���� 
	    MyBuf[12] = 0x00;				// null �Է� 
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�
	    // time code ���� 
	    MyBuf[18]  = 0x00;				// null �Է� 
	    break;
            
	case 0x06:		// master card
	    DecodeHotelCode();        // Get Hotel Code
            DecodeStaffNumber();
            DecodeSecurityNumber();   // Get Scurity Number
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time Code 
	    temp = CARD_DATA[6];
	    temp = temp & 0x1f ;
	    MyBuf[11] = temp;
            DecodeStaffArea();
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�
	    // time code ���� 
	    MyBuf[18]  = 0x00;				// null �Է� 
	    break;
            
	case 0x07:			// maid card
	    DecodeHotelCode();        // Get Hotel Code
            DecodeStaffNumber();
            DecodeSecurityNumber();   // Get Scurity Number 
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time ���α׷� ���� ���� 
	    temp = CARD_DATA[6];
	    temp = temp & 0x1f ;
	    MyBuf[11] = temp;
            DecodeStaffArea();
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ�
	    // time code ���� 
	    MyBuf[18]  = 0x00;				// null �Է� 
            break;
            
	case 0x08:		// minibar card
	    DecodeHotelCode();        // Get Hotel Code
            DecodeStaffNumber();
            DecodeSecurityNumber();   // Get Scurity Number  
            DecodeValidData();        // Decode Valid Date
	    // index ���� 
	    temp = CARD_DATA[6];
	    temp = (temp & 0xe0)/32;			// index ����  , ex) 1110 0000�� 0000 0111�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
	    MyBuf[10] = temp;
	    // time ���α׷� ���� ���� 
	    temp = CARD_DATA[6];
	    temp = temp & 0x1f ;
	    MyBuf[11] = temp;
            DecodeStaffArea();
            DecodeSpecialArea();    // special area ����  ->5 Byte �ҿ� 
	    // time code ���� 
	    MyBuf[18]  = 0x00;				// null �Է� 
            break;
            
        case 0x0f:			// setting card
            DecodeHotelCode();        // Get Hotel Code
	    // ������ ���� 
	    //temp = CARD_DATA[2];
	    //temp1 = (temp & 0xe0)/32;
	    //temp = temp1;
	    //setting_class = temp;
            setting_class = DecodeExtentionClass();
	    setting_plus = card_class+setting_class;
	    MyBuf[0] = setting_plus;	
	    //	MyBuf[3] = temp;					// ������ ����  ->20110513 �����ڸ� ī�� Ŭ������ ���ļ� ������ ���� 
	    if (setting_class == 0x00)			// reset card
	    {
	        // security number ���� 
		temp = CARD_DATA[2];
		temp1 = temp & 0x1f;
		temp1 = temp1*16;			// 0001 1111 �� 1 1111 0000�� ����� ����.��������� left�� 4ĭ shift ��Ų ��� 
		temp = CARD_DATA[3];
		temp2 = temp & 0xf0;
		temp2 = temp2 /16;			// 1111 0000 �� 0000 1111�� ����� ����.��������� right�� 4ĭ shift ��Ų ��� 
		temp3 = temp1 + temp2;
		temp = (temp3 & 0x0f00 ) /256;
		MyBuf[3] = temp;				// scurity number ���� byte ���� 
		temp = temp3 & 0x00ff;
		MyBuf[4] = temp;				// scurity number ���� byte ���� 
		// null �� �Է� �� 5byte
		MyBuf[5] = 0x00;
		MyBuf[6] = 0x00;
		MyBuf[7] = 0x00;
		MyBuf[8] = 0x00;
		MyBuf[9] = 0x00;
		MyBuf[10] = 0x00;
	    }
	    if (setting_class == 0x01)			// time card
	    {
		// scurity number ���� 
		temp = CARD_DATA[2];
		temp1 = temp & 0x1f;
		temp1 = temp1*16;			// 0001 1111 �� 1 1111 0000�� ����� ����.��������� left�� 4ĭ shift ��Ų ��� 
		temp = CARD_DATA[3];
		temp2 = temp & 0xf0;
		temp2 = temp2 /16;			// 1111 0000 �� 0000 1111�� ����� ����.��������� right�� 4ĭ shift ��Ų ��� 
		temp3 = temp1 + temp2;
		temp = (temp3 & 0x0f00 ) /256;
		MyBuf[3] = temp;				// scurity number ���� byte ���� 
		temp = temp3 & 0x00ff;
		MyBuf[4] = temp;				// scurity number ���� byte ���� 
		// date ����
		temp = CARD_DATA[3];
		temp1 = (temp & 0x0f ) * 2;		// 0000 1111 ��  0001 1110�� ����� ����.��������� left�� 1ĭ shift ��Ų ��� 
		temp = CARD_DATA[4];
		temp2 = (temp & 0x80) /128;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 7ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[5] = temp;				//date ���� 
		// month ���� 
		temp = CARD_DATA[4];
		temp = (temp & 0x78) /8;		// 0111 1000 �� 0000 1111�� ����� ����.��������� right�� 3ĭ shift ��Ų ��� 
		MyBuf[6] = temp;				// month ���� 
		// year ���� 
		temp = CARD_DATA[4];
		temp1 = (temp & 0x07) * 2;		// 0000 0111 ��  0000 1110�� ����� ����.��������� left�� 1ĭ shift ��Ų ��� 
		temp = CARD_DATA[5];
		temp2 = (temp & 0x80) /128;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 7ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[7] = temp;				//year ���� 
		// ���� ���� 
		temp = CARD_DATA[5];
		temp = (temp & 0x70) /16;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 4ĭ shift ��Ų ��� 
		MyBuf[8] = temp;	
		//hour ���� 
		temp = CARD_DATA[5];
		temp1 = (temp & 0x0f ) * 2;		// 0000 1111 ��  0001 1110�� ����� ����.��������� left�� 1ĭ shift ��Ų ��� 
		temp = CARD_DATA[6];
		temp2 = (temp & 0x80) /128;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 7ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[9] = temp;				//hour ���� 
		// minute ���� 
		temp = CARD_DATA[6];
		temp = (temp & 0x7e) /2;		// 0111 1000 �� 0000 1111�� ����� ����.��������� right�� 1ĭ shift ��Ų ��� 
		MyBuf[10] = temp;				// month ���� 
	    }
	    if (setting_class == 0x02)			// initial card
	    {
		// room number ���� 
		temp = CARD_DATA[2];
		temp1 = (temp & 0x1f) *128; 	// 0001 1111 ��  1111 1000 0000�� ����� ����.��������� left�� 7ĭ shift ��Ų ��� 
		temp = CARD_DATA[3];
		temp2 = (temp & 0xfe)	/2;		// 1111 1110 ��  0111 1111�� ����� ����.��������� right�� 1ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = (temp3 & 0x0f00 ) /256;
		MyBuf[3] = temp;				// room number ���� byte ���� 
		temp = temp3 & 0x00ff;
		MyBuf[4] = temp;				// room number ���� byte ���� 
		// cb����: Master Area 
		temp = CARD_DATA[3];
		temp1 = (temp & 0x01) *32;		// 0000 0001 ��  0010 0000�� ����� ����.��������� left�� 5ĭ shift ��Ų ��� 
		temp = CARD_DATA[4];
		temp2 = (temp & 0xf8)	/8;		// 1111 1000 ��  0001 1111�� ����� ����.��������� right�� 3ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[5] = temp;
		// stb: Maid Area 
		temp = CARD_DATA[4];
		temp1 = (temp & 0x07) *8;		// 0000 0111 ��  0011 1000�� ����� ����.��������� left�� 3ĭ shift ��Ų ��� 
		temp = CARD_DATA[5];
		temp2 = (temp & 0xe0) /32;		// 1110 0000 ��  0000 0111�� ����� ����.��������� right�� 5ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[6] = temp;
		// mb: Minibar Area
		temp = CARD_DATA[5];
		temp1 = (temp & 0x1f) *2;		// 0001 1111 ��  0011 1110�� ����� ����.��������� left�� 1ĭ shift ��Ų ��� 
		temp = CARD_DATA[6];
		temp2 = (temp & 0x80)/128;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 7ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[7] = temp;
		// Reader Type
		temp = CARD_DATA[6];
		temp = (temp & 0x78) /8;		// 0111 1000 �� 0000 1111�� ����� ����.��������� right�� 3ĭ shift ��Ų ��� 
		MyBuf[8] = temp;
		// Special Area Number 
		temp = CARD_DATA[7];
		temp1 = (temp & 0x1f) *2;		// 0001 1111 ��  0011 1110�� ����� ����.��������� left�� 1ĭ shift ��Ų ��� 
		temp = CARD_DATA[8];
		temp2 = (temp & 0x80)/128;	// 1000 0000 �� 0000 0001�� ����� ����.��������� right�� 7ĭ shift ��Ų ��� 
		temp3 = temp1+temp2;
		temp = temp3;
		MyBuf[9] = temp;
		// Doorlock Address 
		temp = CARD_DATA[8];
		temp = temp & 0x7f;
		MyBuf[10] = temp;
	    }
            
	    if (setting_class == 21)	      // Hotel Code Set
	    {
                temp = CARD_DATA[4];
                MyBuf[5] = temp;
                temp = CARD_DATA[5];
                MyBuf[6] = temp;      
	    }    
	    if (setting_class == 22)          // Contact Time Set
	    {
                temp = CARD_DATA[4];          // Auto Time
                MyBuf[5] = temp;
                temp = CARD_DATA[5];          // Call Time
                MyBuf[6] = temp;                
	    }       
            
	    if (setting_class == 23)          // Time Set
	    {
                temp = CARD_DATA[4];          // Week
                MyBuf[5] = temp;
                temp = CARD_DATA[5];          // Year
                MyBuf[6] = temp;    
                temp = CARD_DATA[6];          // Month
                MyBuf[7] = temp;
                temp = CARD_DATA[7];          // Day
                MyBuf[8] = temp;   
                temp = CARD_DATA[8];          // Hour
                MyBuf[9] = temp;
                temp = CARD_DATA[9];          // Minute
                MyBuf[10] = temp;    
                temp = CARD_DATA[10];         // Second
                MyBuf[11] = temp;                
	    }  
	    if (setting_class == 24)          // Area Number Set
	    {
                temp = CARD_DATA[4];          // 
                MyBuf[5] = temp;  
	    }                 
	    // ���� 8byte�� null�� ä�� 
	    MyBuf[11] = 0x00;
	    MyBuf[12] = 0x00;
	    MyBuf[13] = 0x00;
	    MyBuf[14] = 0x00;
	    MyBuf[15] = 0x00;
	    MyBuf[16] = 0x00;
	    MyBuf[17] = 0x00;
	    MyBuf[18] = 0x00;		// �����ڸ� ī�� Ŭ������ ���ļ� �����Ƿ� �߰� ��.
	    break;
            
        default:
            break;
    }
}


//========================================================================//
void ISO15693(void)
{
}
void ISO15693_LOOP(void)
{
    BYTE i;
    ISO15693();
 
    if(DETECT_15693)             
    {
	DETECT_15693 = 0;
	DETECT_CARD = 1; 
        //Timer1_Sec_Start(20);

		Buzzer_On();
		delay_ms(50);
		Buzzer_Off();     
        //TransmitReadingCard();
	DecodeCardData();		// card data�� �з��ϱ� ���� 
        //TransmitDecodingCard();
	for (i=0 ; i<25 ; i++)
	{
	    CARD_DATA[i] = 0;
	}
        //TransmitDecodingCard();
	CheckCardClass();	
        DETECT_CARD = 0;
    }

    for (i=0 ; i<30 ; i++)
    {
	MyBuf[i] = 0;
    }  
    
    //Transmit_Test();
}

void CardDetect(void)
{
    //DETECT_CARD = 0;
    ISO15693_LOOP();
    
    if(!DETECT_CARD && !NEW_CARD)
    {  
        //WhiteLedOn();
        //Timer1_Sec_Stop();	
	//Timer1_Sec_Start(0);
        //T1IRQ = 1;
	NORMAL_RESET = 1;
        delay_ms(5);	
    }  
}
void CheckCardData(void)
{
	//BYTE i=0;
	//DecodeCardData();
	//NEW_CARD=1;
	
	
	CheckCardClass();
	
}

