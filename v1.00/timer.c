/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : timer.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/11
* Description : 
********************************************************************************
*/
#include "common.h"

volatile BYTE TOV0Count, TOV1Count, TOV2Count, TOV3Count;
volatile BYTE T0IRQ, T1IRQ, T2IRQ, T3IRQ;

volatile BYTE T3OVF;

ISR(TIMER0_OVF_vect)
{
    if(TOV0Count)
    {  
        // 1Sec:   1/4MHz=250n -> 250ns x 1024분주 = 10msec
        TOV0Count--;
        TCNT0 = 0xff - 71;      
        TIMSK |= 0x01;            //  Enable Timer0 interrupt 
    }
    else if(!TOV0Count)
    {
    	WDT;
    	Timer0_mSec_Stop();
	Timer0_mSec_Start(10);
    /*
        T0IRQ = 1;
        TIMSK &= 0xfe;    // Disable Timer0 interrupt
        */
    }  
} \

ISR(TIMER1_OVF_vect)
{
    if(TOV1Count)
    {  
        // 1Sec:   1/4MHz=250n -> 250ns x 256분주=64us -> 64u X 15625 = 1sec
        TOV1Count--;	
        //TCNT1H = 0xff - 0x4C;      
        //TCNT1L = 0xff - 0x4A;     // 15625 = 0x3D09
		TCNT1 = 0xffff-0x7a11;
		
        TIMSK |= 0x04;            //  Enable Timer1 interrupt 
    }	
    else if(!TOV1Count)
    {
        T1IRQ = 1;
        TIMSK &= 0xfb;    // Disable Timer1 interrupt
    } 	 
}

/*
//#pragma vector=TIMER2_OVF_vect          //8 bit Timer Overflow
//__interrupt void Timer2_ovf_isr(void)
ISR(TIMER2_OVF_vect)
{
    if(TOV2Count)
    {  
        // 1Sec:   1/4MHz=250n -> 250ns x 1024분주 = 10msec
        TOV2Count--;
        TCNT2 = 0xff - 125;      
        TIMSK |= 0x40;            //  Enable Timer2 interrupt 
    }
    else if(!TOV2Count)
    {
        T2IRQ = 1;
        TIMSK &= 0xbf;    // Disable Timer2 interrupt
    }  
}
*/

ISR(TIMER2_OVF_vect)
{
	//WDT;
    if(TOV2Count)
    {  
        // 1Sec:   1/4MHz=250n -> 250ns x 1024분주 = 10msec
        TOV2Count--;
        TCNT2 = 0xff - 248;      
        TIMSK |= 0x40;            //  Enable Timer2 interrupt 
    }
    else if(!TOV2Count)
    {
        T2IRQ = 1;
        TIMSK &= 0xbf;    // Disable Timer2 interrupt
    }  
}



/*
ISR(TIMER3_OVF_vect)            	// Timer3 ISR
{
	if(TOV3Count)
	{ 
     	TOV3Count--;	            // Decrease timeout counter
     	TCNT3H = 0xff - 0x3d;       // 
		TCNT3L = 0xff - 0x09;		// 15625 = 0x3D09
     	ETIMSK |= 0x04;				// Timer/Counter3 Overflow Interrupt Enable
 	}
   	if(!TOV3Count) 
   	{
		T3IRQ = 1;	               	// Set timeout state
   		ETIMSK &= 0xfb;           	// Timer/Counter3 Overflow Interrupt Disable
   	}
}
*/

void Timer0_Init(void)
{
    TIMSK &= 0xfe;    	// Disable Timer0 interrupt      
    TCCR0 = 0x07;    	// Normal Mode, Prescaling: 64        
    TCNT0 = 0x00;     	// Counter Value Clear
    Timer0_mSec_Stop();
	Timer0_mSec_Start(10);
//    TIFR &= 0x00;     // Timer Interrupt Flag Clear
//    TIMSK |= 0x04;     //  Enable Timer1 interrupt    
}

void Timer0_mSec_Start(WORD msecond)   
{
    TIMSK &= 0xfe; 	        // Disable Timer0 interrupt
    TCNT0 = 0xff - 71;     // 1 Sec에 해당되는 시정수 ;
    TOV0Count = msecond;
    T0IRQ = 0;	                // Reset timeout state
    TIMSK |= 0x01;              // timer0 start
} 

void Timer0_mSec_Stop(void)
{
    TIMSK &= 0xfe;              // disable Timer1 interrupt
    TOV0Count = 0;
//    TIFR |= 0x00;             // overflow flag clear
}

void Timer1_Init(void)
{
    TIMSK &= 0xfb;    // Disable Timer1 interrupt
    TCCR1A = 0x00;    // Normal Mode      
    TCCR1B = 0x04;    // Prescaling: 256         
    TCNT1 = 0x00;     // Counter Value Clear    
}

void Timer1_Sec_Start(BYTE second)   
{
    TIMSK &= 0xfb; 	        	// Disable Timer1 interrupt
    //TCNT1 = 0xffff - 15625;  	// 1 Sec에 해당되는 시정수 ;
	//TCNT1 = 0xffff-0x0c35;      // 100msec에 해당되는 시정수 ; 65536 - x = 3125 ->x= 65535 - 3125 -> 가 된다
	//TCNT1 = 0xffff-0x4C4A;      // 100msec에 해당되는 시정수 ; 65536 - x = 3125 ->x= 65535 - 3125 -> 가 된다

	TCNT1 = 0xffff-0xF422;      
    TOV1Count = second;
    T1IRQ = 0;	                // Reset timeout state
    TIMSK |= 0x04;              // timer1 start
} 

void Timer1_Sec_Stop(void)
{
    TIMSK &= 0xfb;              // disable Timer1 interrupt
    TOV1Count = 0;
//    TIFR |= 0x00;             // overflow flag clear
}

void Timer2_Init(void)
{
    TIMSK &= 0xbf;    // Disable Timer0 interrupt      
    TCCR2 = 0x07;    // Normal Mode, Prescaling: 1024         
    TCNT2 = 0x00;     // Counter Value Clear
    OCR2 = 0x00;
//    TIFR &= 0x00;     // Timer Interrupt Flag Clear
//    TIMSK |= 0x04;     //  Enable Timer1 interrupt    
}

void Timer2_mSec_Start(BYTE msecond)   
{
    TIMSK &= 0xbf; 	        	// Disable Timer2 interrupt
    TCCR2 = 0x07;    			// Normal Mode, Prescaling: 1024
    TCNT2 = 0xff - 71;          // 1 Sec에 해당되는 시정수 ;
    TOV2Count = msecond;
    T2IRQ = 0;	                // Reset timeout state
    TIMSK |= 0x40;              // timer2 start
} 

void Timer2_mSec_Stop(void)
{
    TIMSK &= 0xbf;              // disable Timer1 interrupt
    TOV2Count = 0;
//    TIFR |= 0x00;             // overflow flag clear
}

/*
//===========================================================================//
void Timer3_Init(void)
{
	// Timer/Counter 3 initialization
	TCCR3A=0x00;
	TCCR3B=0x05;                       // CK / 1024
	TCNT3H=0x00;
	TCNT3L=0x00;
}

void Timer3_1Sec_Start(BYTE sec)
{
  	ETIMSK &= 0xfb;				// Timer/Counter3 Overflow Interrupt Disable
  
	TCNT3H = 0xff - 0x3d;      	// 15625 = 0x3D09
	TCNT3L = 0xff - 0x09;
  	TOV3Count = sec;
  	T3IRQ = 0;              	// Reset timeout state
  	ETIMSK |= 0x04;				// Timer/Counter3 Overflow Interrupt Enable
} 

void Timer3_1Sec_Stop(void)
{
  	ETIMSK &= 0xfb;				// Timer/Counter3 Overflow Interrupt Disable
  	TOV3Count = 0;
}
*/

void delay_us(WORD time_us)  	// time delay for 16MHz
{
    register WORD i;

    for(i = 0; i < time_us; i++)      	        // 4 cycle +
    { 
		asm("nop");asm("nop"); asm("nop"); asm("nop"); 
		//	asm("nop");asm("nop"); 
 		//       asm("nop"); asm("nop"); 
		//	asm("nop"); asm("nop");
    }
}	

void delay_ms(WORD time_ms)	// time delay for ms 
{ 
    register WORD i;
	WDT;
    for(i = 0; i < time_ms; i++)
    { 
    	WDT;
		delay_us(250);
      	delay_us(250);
      	delay_us(250);
      	//delay_us(250);
    }
}

