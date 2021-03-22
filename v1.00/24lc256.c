
#include "common.h"

#define SLA_24LC256_WR  0xA0    // Control + Chip Select + Write
#define SLA_24LC256_RD  0xA1    // Control + Chip Select + Read

//#define SCL       GET_BITFIELD(&PORTD).bit0
//#define SDA       GET_BITFIELD(&PORTD).bit1

//volatile BYTE eep_buf;
//volatile BYTE Nack;
//BYTE ucHibyte,ucLobyte;

void i2c_nops(BYTE ucCnt)
{
    BYTE i;
    for(i=0 ; i<ucCnt ; i++)   
    {
        nop();
    }
}

void i2c_delay_us(BYTE ucTime)
{
    register BYTE i;
    for(i=0; i<ucTime; i++)
    {
        asm(" PUSH R0 ");
        asm(" POP R0 ");
        asm(" PUSH R0 ");
        asm(" POP R0 ");
        asm(" PUSH R0 ");
        asm(" POP R0 ");
    }
}

void i2c_Init(void)
{
    //SetBit(DDRG, DDG4);     // SDA Output Set
   DDRG=DDRG|0X10;
   //  SetBit(DDRG, DDG3);     // SCL Output Set
   DDRG=DDRG|0X08;
  
    //SetBit(PORTG, PORTG4);  // SDA Output Set
    PORTG=PORTG|0X10;
    //SetBit(PORTG, PORTG3);  // SCL Output Set
    PORTG=PORTG|0X08;
   
}  

void i2c_Start(void)
{
   // SetBit(DDRG, DDG3);     // SCL Output Set
    //SetBit(DDRG, DDG4);     // SDA Output Set
    DDRG=DDRG|0X10;
    DDRG=DDRG|0X08;
	
    SCL_H;//SCL = 1;  // SCL High
    SDA_H;//SDA = 1;  // SDA High 
    i2c_delay_us(1);
    
    SDA_L;//SDA = 0;  // SDA Low
    i2c_delay_us(4); 
    SCL_L;//SCL = 0;        // SCL=0 , SDA:1->0 after other 5 usecs 
}  

void i2c_Stop(void)
{
    //SetBit(DDRG, DDG4); // SDA Output Set
    DDRG=DDRG|0X10;
   	SCL_L;//SCL = 0;            // SDA=0, SCL=0, initial state of I2C  
    SDA_L;//SDA = 0;            // SDA=0,SCL=1, after 2 usecs
    i2c_delay_us(4); 
    
    SCL_H;//SCL = 1;            // SDA:0->1,SCL=1, after other 5 usecs
    i2c_delay_us(3); 
    SDA_H;//SDA = 1;            // SDA=0,SCL=1, after 2 usecs
    //ClrBit(DDRG, DDG4); // SDA Input Set
    DDRG=DDRG&0xEF;
	if(save_flag)
	{
		delay_ms(8);
	}
	else
	{
		delay_us(130);
	}
}   

void i2c_Clock(void)
{
   SCL_H;//SCL = 1;           // SCL High
   i2c_delay_us(2);   // Small Delay
   SCL_L;//SCL = 0;           // SCL Low
}


BYTE i2c_Acknak(void)
{
    BYTE Acknak = 0;    // Temp RAM for Ackn flag
   
    //ClrBit(DDRG, DDG4); // SDA Input Set
	DDRG=DDRG&0xEF;
    SCL_L;//SCL = 0;
 
    SCL_H;//SCL = 1;            // Clock the ACK bit
    i2c_nops(1);
    if(PORTG&0x10) Acknak = 1; // 1111 1111 0001 0000 = 0001 0000
    else Acknak = 0;
    i2c_nops(1);
   
    SCL_L;//SCL = 0;
   //SetBit(DDRG, DDG4); // SDA Output Set
	DDRG=DDRG|0x10;
    return Acknak;      // Return our ACK bit
}

void i2c_Byte_Write(BYTE ucData)
{
   BYTE i;

    //SetBit(DDRG, DDG4);  // SDA Output Set
    DDRG=DDRG|0x10;
   	SCL_L;//SCL = 0;             // Clear SCL Low

   for (i=0; i<8 ; i++)
   {     
      if(ucData & 0x80) SDA_H;//SDA = 1;  // Set SDA High //수정
      else SDA_L;//SDA = 0;               // Clear SDA Low
      i2c_nops(1);
      
      SCL_H;//SCL = 1;                  // Set SCL High, Clock data
      ucData = ucData << 1;     // Shift data in buffer right one
      i2c_nops(1);
      if(i==0)  i2c_nops(2);    
      SCL_L;//SCL = 0;                  // Clear SCL
   }
    //ClrBit(DDRG, DDG4);          // SDA Input Set
    DDRG=DDRG&0xEF;
   i2c_nops(45);                // Set SCL High, Clock data
   i2c_Acknak();
   //if(i2c_Acknak() == 1 )   Nack = 1;
}


BYTE i2c_Byte_Read(BYTE ucAck)
{
    BYTE i, ReadBuff = 0;
    
   //ClrBit(DDRG, DDG4);      // Set SDA to input
   DDRG=DDRG&0xEF;
    nop();
	//ClrBit(DDRG, DDG3); 
    for(i=0; i<8; i++)
    {
        ReadBuff <<= 1;
      
        SCL_H;//SCL = 1;              // Set SCL High, Clock bit out
        i2c_nops(3);          // Small Delay
        
        // Read data on SDA pin
        if ((PING & 0x10)==0x10) ReadBuff |= 0x01; //수정
        SCL_L;//SCL=0;                  // Clear SCL
        i2c_nops(3);            // Small Delay
    }
    i2c_nops(80);
    
    if(ucAck == 0)
    {   
        //SetBit(DDRG, DDG4);  // SDA Output Set
        DDRG=DDRG|0x10;
        SDA_L;//SDA = 0;
        i2c_Clock();
    }  
    else      // No Ack
    {
        PORTG |= 0x08;   // Clear SDA High
        i2c_Clock();
        i2c_nops(4);      // Small Delay
        PORTG &= 0xF7;   // Clear SDA Low
    }
    return ReadBuff;
}

void Save_Byte_Data(WORD uiAddr, BYTE ucData)
{
    BYTE ucAddrHigh, ucAddrLow;
    ucAddrLow = (BYTE)uiAddr;           // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);   // crc Hign Byte
    save_flag=1;
    i2c_Start();
    
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh); 
    i2c_delay_us(100);
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    i2c_Byte_Write(ucData);
    i2c_delay_us(70);
    
    i2c_Stop();
}  

void Save_Word_Data(WORD uiAddr, WORD uiData)
{
    BYTE ucAddrHigh, ucAddrLow;
    BYTE ucDataHigh, ucDataLow;
    save_flag=1;
    ucAddrLow = (BYTE)uiAddr;           // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);   // crc Hign Byte
    
    ucDataLow = (BYTE)uiData;           // crc Low Byte
    ucDataHigh = (BYTE)(uiData >> 8);   // crc Hign Byte
    
    i2c_Start();
    
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh); 
    i2c_delay_us(100);
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    i2c_Byte_Write(ucDataHigh);
    i2c_delay_us(70);
    i2c_Byte_Write(ucDataLow);
    i2c_delay_us(70); 
    
    i2c_Stop();
}

void Save_Page_Data(WORD uiAddr, BYTE nSize, BYTE *ucData)
{
    BYTE ucAddrHigh, ucAddrLow;
    BYTE i;
    save_flag=1;
    ucAddrLow = (BYTE)uiAddr;           // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);   // crc Hign Byte
    
    //ucDataLow = (BYTE)uiData;           // crc Low Byte
    //ucDataHigh = (BYTE)(uiData >> 8);   // crc Hign Byte
    
    i2c_Start();
    
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh); 
    i2c_delay_us(100);
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    for(i=0; i<nSize; i++)
    {  
        i2c_Byte_Write(*(ucData+i));
        //i2c_delay_us(70);
        i2c_delay_us(100);
    }
    
    i2c_Stop();
}

void Save_Page_Init(WORD uiAddr, BYTE nSize, BYTE ucData)
{
    BYTE ucAddrHigh, ucAddrLow;
    BYTE i;
    save_flag=1;
    ucAddrLow = (BYTE)uiAddr;           // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);   // crc Hign Byte
    
    i2c_Start();
    
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh); 
    i2c_delay_us(100);
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    for(i=0; i<nSize; i++)
    {  
        i2c_Byte_Write(ucData);
        //i2c_delay_us(70);
        i2c_delay_us(100);
    }
    
    i2c_Stop();
}

BYTE Read_Byte_Data(WORD uiAddr)
{
    BYTE ucAddrHigh, ucAddrLow;
    BYTE read_buf;
	save_flag=0;
    ucAddrHigh = (BYTE)(uiAddr >> 8);         // crc Hign Byte 
    ucAddrLow = (BYTE)uiAddr;
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh);
    i2c_delay_us(100);    
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_RD);
    i2c_delay_us(70);  

    read_buf = i2c_Byte_Read(1);
    i2c_delay_us(40);
    i2c_Stop();
    
    return read_buf;
}

WORD Read_Word_Data(WORD uiAddr)
{  
    BYTE ucAddrHigh, ucAddrLow; 
    BYTE HiByte, LoByte;
    WORD ReadBuff;
	save_flag=0;
   WDT;
    ucAddrLow = (BYTE)uiAddr;                // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);         // crc Hign Byte 
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh);
    i2c_delay_us(100);    
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_RD);
    i2c_delay_us(70);

    HiByte = i2c_Byte_Read(0);
    i2c_delay_us(40);
    LoByte = i2c_Byte_Read(1);
    i2c_delay_us(40);
    
    i2c_Stop();
    
    ReadBuff = HiByte;
    ReadBuff = (HiByte << 8);   
    ReadBuff |= LoByte; 
    
    return ReadBuff;    
}


void Read_Page_Data(WORD uiAddr, BYTE nSize, BYTE *pData)
{  
    BYTE ucAddrHigh, ucAddrLow; 
    //BYTE HiByte, LoByte;
    BYTE i, ReadBuff;
   save_flag=0;
    ucAddrLow = (BYTE)uiAddr;                // crc Low Byte
    ucAddrHigh = (BYTE)(uiAddr >> 8);         // crc Hign Byte 
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_WR);
    i2c_delay_us(40);
    
    i2c_Byte_Write(ucAddrHigh);
    i2c_delay_us(100);    
    i2c_Byte_Write(ucAddrLow);
    i2c_delay_us(100);
    
    i2c_Start();
    i2c_Byte_Write(SLA_24LC256_RD);
    i2c_delay_us(70);

    for(i=0; i<nSize-1; i++)
    {  
        ReadBuff = i2c_Byte_Read(0);
        *pData++ = ReadBuff;
        i2c_delay_us(40);
    }
    
    ReadBuff = i2c_Byte_Read(1);
    i2c_delay_us(40);
    *pData = ReadBuff;
    
    i2c_Stop();   
}

void ReadExtMemoryPageData(WORD uiAddr, BYTE nSize, BYTE *pData)
{
    BYTE i, temp;
    save_flag=0;
    for(i=0; i<nSize; i++)
    {  
        temp = Read_Byte_Data(uiAddr+i); 
        *pData++ = temp;
    }  
}  
