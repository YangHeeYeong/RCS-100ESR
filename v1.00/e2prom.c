

/*
********************************************************************************
* Messerschmitt Korea
* (c) Copyright 2009, Messerschmitt Korea
*
* Hardware : RST-500DL
* Filename : doorlock.c
* Version  : 1.0
* Programmer(s): Jong-min Kim
* Created : 2009/5/12
* Description : 
********************************************************************************
*/
#include "common.h"


void e2prom_write_byte(unsigned int uiAddress, unsigned char ucData)
{
    cli( );
      
    /* Wait for completion of previous write */
    while(EECR & (1<<1));

    /* Set up address and data register */
    EEAR = uiAddress;
    EEDR = ucData;

    /* Write logical one to EEMWE */
    EECR |= (1<< 2);  

    /* Start EEPROM write by setting EEWE */
    EECR |= (1<< 1);
    
    sei();
}

void e2prom_write_word(WORD uiAddress, WORD uiData)
{
    BYTE ucDataHigh, ucDataLow;
    
    ucDataLow = (BYTE)uiData;           // crc Low Byte
    ucDataHigh = (BYTE)(uiData >> 8);   // crc Hign Byte
    
    e2prom_write_byte(uiAddress, ucDataHigh);
    //delay_ms(5);
    e2prom_write_byte(uiAddress+1, ucDataLow);
}  

unsigned char e2prom_read_byte(unsigned int uiAddress)
{
    cli( );
    
    /* Wait for completion of previous write */
    while(EECR & (1<<1));

    /* Set up address register */
    EEAR = uiAddress;

    /* Start EEPROM read by setting EERE */
    EECR |= (1 << 0);

    sei();

    /* Return data from data register */
    return EEDR;
}



WORD e2prom_read_word(WORD uiAddress)
{
    BYTE HiByte, LoByte;
    WORD E2pBuf;
    

    HiByte = e2prom_read_byte(uiAddress);
    LoByte = e2prom_read_byte(uiAddress+1); 
    
    E2pBuf = HiByte;
    E2pBuf = (HiByte << 8);   
    E2pBuf |= LoByte; 
    
    return E2pBuf;     
}  


