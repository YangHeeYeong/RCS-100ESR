 /*
********************************************************************************
* 3ALogics Inc.
* 7th Floor, Hyundai-office Bldg., 9-4, Sunae-dong. Bundang-gu, Seongnam-si,
* Gyeonggi-do, 463-783, Korea
*
* (c) Copyright 2015, 3ALogics, Korea
*
* Hardware : RACOS Sytem RFID Reader
* Complier : IAR AVR
* Filename : mifare_crytpo.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2015/08/21
* Description :
********************************************************************************
*/
#include "common.h"

#define rev8(x)			((((x)>>7)&1)^((((x)>>6)&1)<<1)^((((x)>>5)&1)<<2)^((((x)>>4)&1)<<3)^((((x)>>3)&1)<<4)^((((x)>>2)&1)<<5)^((((x)>>1)&1)<<6)^(((x)&1)<<7))
#define rev16(x)		(rev8 (x)^(rev8 (x>> 8)<< 8))
#define rev32(x)		(rev16(x)^(rev16(x>>16)<<16))

#define bit(x,n)		(((x)>>(n))&1)


#define mf1_f4a		0x9E98
#define mf1_f4b		0xB48E
#define mf1_f5c		0xEC57E80A


void MifarePRNG32(unsigned char *num, unsigned char n)
{
  unsigned char i;
  unsigned long x = 0;
  
  x = ((((unsigned long)num[0])<<24) | (((unsigned long)num[1])<<16) | 
  (((unsigned long)num[2])<<8) | ((unsigned long)num[3]));
  
  x = rev32(x);
  for (i = 0; i < n; i++) x=(x<<1)+(((x>>15)^(x>>13)^(x>>12)^(x>>10))&1);
  x = rev32(x);
  
  num[0] = (unsigned char)(x >> 24);
  num[1] = (unsigned char)(x >> 16);
  num[2] = (unsigned char)(x >> 8);
  num[3] = (unsigned char)x;
}

unsigned char KeyLeftShiftPosision(unsigned char *x, unsigned char shift)
{
  unsigned char bytecnt;
  unsigned char bitcnt;
  unsigned char temp;
  
  if(shift == 0){
    if(x[5]&0x01) return 1;
    else return 0;
  }
  else{
    bytecnt = shift/8;
    bitcnt = shift%8;
    
    if(bytecnt == 0){
      temp = ((x[5] >> bitcnt)&0x01);
      return temp;
    }
    else if(bytecnt == 1){
      temp = ((x[4] >> bitcnt)&0x01);
      return temp;
    }
    else if(bytecnt == 2){
      temp = ((x[3] >> bitcnt)&0x01);
      return temp;
    }
    else if(bytecnt == 3){
      temp = ((x[2] >> bitcnt)&0x01);
      return temp;
    }
    else if(bytecnt == 4){
      temp = ((x[1] >> bitcnt)&0x01);
      return temp;
    }
    else if(bytecnt == 5){
      temp = ((x[0] >> bitcnt)&0x01);
      return temp;
    }
    else return 0;
  }
}


unsigned char MifareIFilter(unsigned char *x, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
  unsigned char i;
  unsigned char caltemp[4];
  unsigned char calresult = 0;
  
  caltemp[0] = KeyLeftShiftPosision(x, a);
  caltemp[1] = KeyLeftShiftPosision(x, b)*2;
  caltemp[2] = KeyLeftShiftPosision(x, c)*4;
  caltemp[3] = KeyLeftShiftPosision(x, d)*8;
  
  calresult = 0;
  for(i=0;i<4;i++) calresult += caltemp[i];
  
  return calresult;
}

// Nonlinear output/feedback function
unsigned char MifareNonlinearFeedback20(unsigned char *x)
{
  unsigned char i;
  unsigned char caltemp[5];
  unsigned char calresult = 0;
  
  caltemp[0] = MifareIFilter(x, 9,11,13,15);
  caltemp[1] = MifareIFilter(x,17,19,21,23);
  caltemp[2] = MifareIFilter(x,25,27,29,31);
  caltemp[3] = MifareIFilter(x,33,35,37,39);
  caltemp[4] = MifareIFilter(x,41,43,45,47);
  
  
  caltemp[0] = (unsigned char)((mf1_f4b >> caltemp[0])&1);
  caltemp[1] = (unsigned char)(((mf1_f4a >> caltemp[1])&1)*2);
  caltemp[2] = (unsigned char)(((mf1_f4a >> caltemp[2])&1)*4);
  caltemp[3] = (unsigned char)(((mf1_f4b >> caltemp[3])&1)*8);
  caltemp[4] = (unsigned char)(((mf1_f4a >> caltemp[4])&1)*16);
  
  calresult = 0;
  for(i=0;i<5;i++) calresult += caltemp[i];
  calresult = (unsigned char)((mf1_f5c >> calresult)&1);
  
  return calresult;
}

// Linear output/feedback function
unsigned char MifareLinearFeedback18(unsigned char *x)
{
  unsigned char i;
  unsigned char caltemp[18];
  unsigned char calresult = 0;
  
  caltemp[0]  = KeyLeftShiftPosision(x , 0);
  caltemp[1]  = KeyLeftShiftPosision(x , 5);
  caltemp[2]  = KeyLeftShiftPosision(x , 9);
  caltemp[3]  = KeyLeftShiftPosision(x , 10);
  caltemp[4]  = KeyLeftShiftPosision(x , 12);
  caltemp[5]  = KeyLeftShiftPosision(x , 14);
  caltemp[6]  = KeyLeftShiftPosision(x , 15);
  caltemp[7]  = KeyLeftShiftPosision(x , 17);
  caltemp[8]  = KeyLeftShiftPosision(x , 19);
  caltemp[9]  = KeyLeftShiftPosision(x , 24);
  caltemp[10] = KeyLeftShiftPosision(x , 25);
  caltemp[11] = KeyLeftShiftPosision(x , 27);
  caltemp[12] = KeyLeftShiftPosision(x , 29);
  caltemp[13] = KeyLeftShiftPosision(x , 35);
  caltemp[14] = KeyLeftShiftPosision(x , 39);
  caltemp[15] = KeyLeftShiftPosision(x , 41);
  caltemp[16] = KeyLeftShiftPosision(x , 42);
  caltemp[17] = KeyLeftShiftPosision(x , 43);
  
  calresult = 0;
  for(i=0;i<18;i++) calresult ^= caltemp[i];
  
  return calresult;
}


void KeyLeftShift(unsigned char *x, unsigned char shiftbit)
{
  unsigned char i;
  unsigned char temp[6];
  
  for(i=0;i<6;i++){
    if(x[i] & 0x01) temp[i] = 0x80;
    else temp[i] = 0x00;
  }
  for(i=0;i<6;i++){
    x[i] >>= 1;
  }
  x[0] |= shiftbit;
  x[1] |= temp[0];
  x[2] |= temp[1];
  x[3] |= temp[2];
  x[4] |= temp[3];
  x[5] |= temp[4];
}

// Single CRYPTO-1 round
// 0 - no feedback
// 1 - linear feedback
// 2 - nonlinear feedback
// 3 - both
unsigned char MifareCrypto1Round(unsigned char in_bit, unsigned char fb)
{
  unsigned char i;
  unsigned char lf, nf;
  unsigned char temp;
  unsigned char key[6];
  
  for(i=0;i<6;i++) key[i] = SECTORKEYBUFF[i];
  
  lf = MifareLinearFeedback18(key);
  nf = MifareNonlinearFeedback20(key);
  
  temp = (in_bit ^ (fb&1?lf:0) ^ (fb&2?nf:0));
  
  if(temp == 1) temp = 0x80;
  else temp = 0;
  
  KeyLeftShift(key, temp);
  for(i=0;i<6;i++) SECTORKEYBUFF[i] = key[i];
  
  return nf;
}


unsigned char MifareCrypto1Byte(unsigned char in_byte, unsigned char fb)
{
  unsigned char i, o;
  
  for (i = 0, o = 0; i < 8; i++) o += MifareCrypto1Round(bit(in_byte,i), fb) << (i);
  return o;
}

unsigned char MifareCrypto1Nibble(unsigned char in_byte, unsigned char fb)
{
  unsigned char i, o;
  
  for (i = 0, o = 0; i < 4; i++) o += MifareCrypto1Round(bit(in_byte,i), fb) << (i);
  return o;
}


unsigned char MifareCrypto1Parity(void)
{
  unsigned char i;
  unsigned char lf, nf;
  unsigned char key[6];
  
  for(i=0;i<6;i++) key[i] = SECTORKEYBUFF[i];
  
  lf = MifareLinearFeedback18(key);
  nf = MifareNonlinearFeedback20(key);
  
  return nf;
}

//==============================================================================
//Mifare Parity&CRC Functions
//==============================================================================
unsigned char MifareParity(unsigned char x)
{
  unsigned char i;
  unsigned char parity = 0;
  
  for(i=0;i<8;i++){
    if(x&0x80) parity ^= 1;
    else parity ^= 0;
    x <<= 1;
  }
  
  if(parity == 1) return 0;
  else return 1;
}

unsigned int MifareCrc(unsigned char len, unsigned char *caldata)
{
  unsigned char i, j;
  unsigned int temp;
  
  temp = 0x6363;
  
  for(i=0;i<len;i++){
    temp ^= caldata[i];
    for(j=0;j<8;j++){
      if(temp&0x0001) temp = (temp>>1) ^ 0x8408;
      else temp = (temp>>1);
    }
  }	
  return temp;
}


