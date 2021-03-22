
#include "common.h"

volatile BYTE rtc_sec = 0;
volatile BYTE rtc_min = 0;
volatile BYTE rtc_hour = 0;
volatile BYTE rtc_week = 0;
volatile BYTE rtc_date = 0;
volatile BYTE rtc_month = 0;
volatile BYTE rtc_year = 0;
volatile BYTE rtc_temp = 0;
//======================================================================//
//																		//
//                     DS1302 Function Definition                      	//
//																		//
//======================================================================//


BYTE bcd_to_binary(BYTE bcdData)
{
    BYTE temp1, temp2;
    temp1 = (bcdData & 0x0f);
    temp2 = (bcdData & 0xf0);
    temp2 = ((temp2 >> 4) * 10);
    bcdData = temp1 + temp2;
    return bcdData;	              // Return BCD Data
}

BYTE bin2bcd(BYTE binData)
{
    BYTE retbcd;
    retbcd = ((binData/10) << 4) + (binData % 10);
    return retbcd;
}

void RTC_Write_Byte(BYTE waddr)
{
    BYTE i, temp;
    temp = waddr;

	DDRC |= 0x02;			// RTC IO Direction = Output
    for(i=0; i<8; ++i)
    {
    	RTC_IO_LOW;
        if(temp & 0x01)
		{ 
	    	RTC_IO_HIGH;
		}
        //else RTC_IO_LOW;
        RTC_CLK_LOW;
        delay_us(10);
        RTC_CLK_HIGH;
        delay_us(10);
        temp >>= 1;
    }
}

BYTE RTC_Read_Byte()       // read one byte from the device
{
    BYTE i;
    BYTE rData = 0, ret;
    BYTE temp; 
	      
	DDRC &= 0xFD;			// RTC IO Direction = Input

    RTC_IO_HIGH;
    for(i=0; i<8; i++)
    {
    	RTC_CLK_HIGH;
        delay_us(10);
        RTC_CLK_LOW;
        delay_us(10);
        temp = (PINC & 0x02) << 6;
		rData >>= 1;
        rData |= temp; 
    }
    ret = bcd_to_binary(rData);

    return ret;						// Return Binary Data
}

void RTC_Reset() 
{
    RTC_CLK_LOW;
    RTC_CS_LOW;
    RTC_CS_HIGH;
}

void RTC_Write_Sec()
{
    RTC_Write_Byte(0x80);
    RTC_Write_Byte(0x07);
}

void RTC_Read_Sec()
{
    RTC_Write_Byte(0x81);
}

void Rtc_Init(void)
{ 
    //BYTE rtc_ini;
    BYTE init_sec, init_min, init_hour;
    BYTE init_date, init_mon, init_day, init_year;
    BYTE bcd_sec, bcd_min, bcd_hour;
    BYTE bcd_date, bcd_mon, bcd_day, bcd_year;	
	
	
	
	
	
	
    init_sec = Read_Byte_Data(eSETSTART+ertc_sec);
    init_min = Read_Byte_Data(eSETSTART+ertc_min);
    init_hour = Read_Byte_Data(eSETSTART+ertc_hour);
    init_date =Read_Byte_Data(eSETSTART+ertc_date);
    init_mon =Read_Byte_Data(eSETSTART+ertc_month);
    init_day = Read_Byte_Data(eSETSTART+ertc_week);      // 
    init_year = Read_Byte_Data(eSETSTART+ertc_year);
    bcd_sec = bin2bcd(init_sec);
    bcd_min = bin2bcd(init_min);
    bcd_hour = bin2bcd(init_hour);
    bcd_date = bin2bcd(init_date);
    bcd_mon = bin2bcd(init_mon);
    bcd_day = bin2bcd(init_day);
    bcd_year = bin2bcd(init_year);
/*
    RTC_Reset();

    RTC_Write_Byte(0x8e);		// Write Write-Protect
    RTC_Write_Byte(0);			// Disable Write-Protect
    RTC_Reset();

    RTC_Write_Byte(0x90);		// Write Trickle-Charge
    RTC_Write_Byte(0x00);		// Disable Trickle-Charge
*/
    RTC_Reset();

    RTC_Write_Byte(0xBE);		// Write Clock Burst
   RTC_Write_Byte(bcd_sec);	// Write Second
    RTC_Write_Byte(bcd_min);	// Write Minute
    RTC_Write_Byte(bcd_hour);	// Write Hour
    RTC_Write_Byte(bcd_date);	// Write Date
    RTC_Write_Byte(bcd_mon);	// Write Month
    RTC_Write_Byte(bcd_day);	// Write Day of Week
    RTC_Write_Byte(bcd_year);	// Write Year
    RTC_Write_Byte(0);		// Write Control Register

    RTC_CS_LOW; 
}


//======================================================================//
void Rtc_Write_Data(void)
{
    BYTE bcd_sec, bcd_min, bcd_hour;
    BYTE bcd_date, bcd_mon, bcd_day, bcd_year;	

    bcd_sec = bin2bcd(rtc_sec);
    bcd_min = bin2bcd(rtc_min);
    bcd_hour = bin2bcd(rtc_hour);
    bcd_date = bin2bcd(rtc_date);
    bcd_mon = bin2bcd(rtc_month);
    bcd_day = bin2bcd(rtc_week);
    bcd_year = bin2bcd(rtc_year);

    RTC_Reset();
    RTC_Write_Byte(0xBE);	// Write Clock Burst
    RTC_Write_Byte(bcd_sec);	// Write Second
    RTC_Write_Byte(bcd_min);	// Write Minute
    RTC_Write_Byte(bcd_hour);	// Write Hour
    RTC_Write_Byte(bcd_date);	// Write Date
    RTC_Write_Byte(bcd_mon);	// Write Month
    RTC_Write_Byte(bcd_day);	// Write Day of Week
    RTC_Write_Byte(bcd_year);	// Write Year
    RTC_Write_Byte(0);		// Write Control Register
    RTC_Reset(); 
    RTC_CS_LOW;
		Save_Byte_Data(eSETSTART+ertc_sec,rtc_sec);
    Save_Byte_Data(eSETSTART+ertc_min,rtc_min);
    Save_Byte_Data(eSETSTART+ertc_hour,rtc_hour);	
    Save_Byte_Data(eSETSTART+ertc_date,rtc_date);
    Save_Byte_Data(eSETSTART+ertc_month,rtc_month);
    Save_Byte_Data(eSETSTART+ertc_week,rtc_week);
    Save_Byte_Data(eSETSTART+ertc_year,rtc_year);
}

//======================================================================//
void Rtc_Read_Data(void)
{
   WDT;
    RTC_Reset();
    RTC_Write_Byte(0xBF);		        // Read Clock Burst
    rtc_sec = RTC_Read_Byte();			// Read Second
    rtc_sec &= 0x7f;
    rtc_min = RTC_Read_Byte();			// Read Minute
    rtc_hour = RTC_Read_Byte();			// Read Hour
    rtc_date = RTC_Read_Byte();			// Read Date
    rtc_month = RTC_Read_Byte();		// Read Month
    rtc_week = RTC_Read_Byte();			// Read Day of Week
    rtc_year = RTC_Read_Byte();			// Read Year
		
    RTC_CS_LOW;
	Save_Byte_Data(eSETSTART+ertc_sec,rtc_sec);
    Save_Byte_Data(eSETSTART+ertc_min,rtc_min);
    Save_Byte_Data(eSETSTART+ertc_hour,rtc_hour);	
    Save_Byte_Data(eSETSTART+ertc_date,rtc_date);
    Save_Byte_Data(eSETSTART+ertc_month,rtc_month);
    Save_Byte_Data(eSETSTART+ertc_week,rtc_week);
    Save_Byte_Data(eSETSTART+ertc_year,rtc_year);
}


