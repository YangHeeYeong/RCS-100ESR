
#ifndef _READER_H
#define _READER_H


#ifdef _SET_GLOBAL_
		#define _GLOBAL_
#else
		#define _GLOBAL_   extern
#endif

_GLOBAL_ volatile BYTE DETECT_CARD, NEW_CARD, NORMAL_RESET;
_GLOBAL_ volatile BYTE MyBuf[50];
_GLOBAL_ volatile BYTE ACU_DATA_CARD; //2020.11.17 ���¼� ��� ����, ���̵� ACU ���� ���� 

void CardDetect(void);
void DecodeCardData(void);
void CheckCardData(void);
#endif