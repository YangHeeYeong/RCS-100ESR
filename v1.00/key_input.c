/*
********************************************************************************
* Messerschmitt 
* (c) Copyright 2011, Messerschmitt 
*
* Hardware : DL-100
* Filename : key_input.c
* Version  : 1.0
* Created : 2011/8
* Description : 
********************************************************************************
*/
#include "common.h"

volatile BYTE Sector_Key[6] = {0xb0, 0x94, 0x95, 0xf4, 0x86, 0x4f};
volatile BYTE Block_Address = 0x20;
