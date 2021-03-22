

#ifndef _E2PROM_H
#define _E2PROM_H

//#define DOOR_SW_ST    0
//#define SECU_SW_ST    1

#define nHOTELCODE    0   // 2 BYTE
#define nFLOORTIME    2
#define nCALLTIME     3
#define nSaveCount    4   // 2 BYTE
#define nSaveAddr     6   // 2 BYTE
#define nOverWrite    8

#define nCheckHour0   11
#define nCheckHour1   12
#define nCheckHour2   13
#define nCheckHour3   14
#define nCheckHour4   15
#define nCheckHour5   16
#define nCheckHour6   17
#define nCheckHour7   18

#define nCheckMin0    19
#define nCheckMin1    20
#define nCheckMin2    21
#define nCheckMin3    22
#define nCheckMin4    23
#define nCheckMin5    24
#define nCheckMin6    25
#define nCheckMin7    26


#define nBuildNo		51
#define nAreaNo			52
#define nFloorNo		53

#define nFloorSet     100
#define nFloor01      101
#define nPublic01     171
#define nPublic02     172

void e2prom_write_byte(unsigned int uiAddress, unsigned char ucData);
void e2prom_write_word(unsigned int uiAddress, unsigned int uiData);
unsigned char e2prom_read_byte(unsigned int uiAddress);
unsigned int e2prom_read_word(unsigned int uiAddress);

#endif
