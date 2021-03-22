
#ifndef _RTC_H
#define _RTC_H

//------------------------------------------------------------------------------
//              DS1302 I/O Pin Definition
//------------------------------------------------------------------------------

#define RTC_CS_HIGH        	PORTC = PORTC | _BV(2); 
#define RTC_CS_LOW         	PORTC = PORTC & ~_BV(2);

#define RTC_IO_HIGH        	PORTC = PORTC | _BV(1); 
#define RTC_IO_LOW         	PORTC = PORTC & ~_BV(1);

#define RTC_CLK_HIGH		PORTC = PORTC | _BV(0);
#define RTC_CLK_LOW         PORTC = PORTC & ~_BV(0);


#define RTC_INIT_ON         0x00
#define RTC_INIT_OFF        0xff

#define W_SECOND	    0x80    // Write Second
#define R_SECOND	    0x81    // Read Second
#define W_MINUTE	    0x82    // Write Minute
#define R_MINUTE	    0x83    // Read Minute
#define W_HOUR		    0x84    // Write Hour
#define R_HOUR		    0x85    // Read Hour
#define W_DATE		    0x86    // Write Date
#define R_DATE		    0x87    // Read Date
#define W_MONTH		    0x88    // Write Month
#define R_MONTH		    0x89    // Read Month
#define W_DAY		    0x8A    // Write Day of Month
#define R_DAY		    0x8B    // Read Day of Month
#define W_YEAR		    0x8C    // Write Year
#define R_YEAR		    0x8D    // Read Year
#define W_PROTECT	    0x8E    // Write Write-Protec Bit
#define R_PROTECT	    0x8F    // Read Write-Protec Bit
#define W_CHARGE	    0x90    // Write Trickle-Charge
#define R_CHARGE	    0x91    // Read Trickle-Charge
#define W_CLK_BURST	    0xBE    // Write Clock Burst Mode
#define R_CLK_BURST	    0xBF    // Read Clock Burst Mode
#define W_RAM_BURST	    0xFE    // Write RAM Burst Mode
#define R_RAM_BURST	    0xFF    // Read RAM Burst Mode


#endif

extern volatile unsigned char rtc_sec;
extern volatile unsigned char rtc_min;
extern volatile unsigned char rtc_hour;
extern volatile unsigned char rtc_week;
extern volatile unsigned char rtc_date;
extern volatile unsigned char rtc_month;
extern volatile unsigned char rtc_year;
extern volatile unsigned char rtc_temp;

void Rtc_Init(void);
void Rtc_Write_Data(void);
void Rtc_Read_Data(void);
unsigned char bin2bcd(unsigned char binData);
