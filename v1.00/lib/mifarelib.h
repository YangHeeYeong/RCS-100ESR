/*
********************************************************************************
* 3ALogics Inc.
* 7th Floor, Hyundai-office Bldg., 9-4, Sunae-dong. Bundang-gu, Seongnam-si,
* Gyeonggi-do, 463-783, Korea
*
* (c) Copyright 2011, 3ALogics, Korea
*
* Hardware : RSK-300
* Filename : mifarelib.h
* Version  : 1.0
* Programmer(s):
* Created : 2011/05/24
* Description : 
********************************************************************************
*/
#ifndef _MIFARELIB_H
#define _MIFARELIB_H


volatile unsigned char SECTORKEY[6];

unsigned char MifareCrypto1Function(unsigned char len, unsigned char *trdata, unsigned char *trbit, unsigned char status);

#endif


