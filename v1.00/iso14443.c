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
* Filename : iso14443.c
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2015/08/21
* Description :
********************************************************************************
*/
#include "common.h"

u8 HCE_FAProcess(void)
{
  u8 i;
  
  TRH_WRITE(0x09, 0x01);  
  /*TRH_WRITE(0x02, 0xFA);  
  TRH_WRITE(0x02, 0x01);  
  TRH_WRITE(0x02, 0x00);  
  */
  for(i=0;i<3;i++) TRH_WRITE(0x02,TRH_RxBuf[i]);
    TRH_WRITE(0x01, 0x14);

  delay_ms(50);
  
  TRH_RxLen = 0;
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN != 0) && !(TRH_ERR&0x08)){
    for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
    #if nfc_debug
    //UART_STRING("\r\n HCE FA0001 = ");for(i=0;i<TRH_RxLen;i++) UART_H2A(TRH_RxBuf[i]);
    #endif
    if(((TRH_RxBuf[TRH_LEN-2] == 0x90)&&(TRH_RxBuf[TRH_LEN-1]==0x00))||(TRH_RxBuf[2] == 0xBD)){
      return 1;
    }
    else{
      if(TRH_RxBuf[0] == 0xFA) return 3;
      else 
	 {
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
	 	return 0;
      }
    }
  }
  else
  {
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
    return 0;
  }
}

//0xD4, 0x10, 0x52, 0x41, 0x43, 0x4F, 0x53
u8 HCE_SELECTDF(void)
{
  u8 i;
  u8 tmp;
  
  TRH_WRITE(0x09, 0x01);
  
  ISO14443_PCB ^= 0x01;
  TRH_WRITE(0x02, ISO14443_PCB);
  TRH_WRITE(0x02, 0x00);
  //#############################################
  TRH_WRITE(0x02, 0x00); //CLA
  TRH_WRITE(0x02, 0xA4); //INS
  TRH_WRITE(0x02, 0x04); //P1
  TRH_WRITE(0x02, 0x00); //P2
  TRH_WRITE(0x02, 0x07); //Lc
  //#############################################
  TRH_WRITE(0x02, 0xD4); //DATA1
  TRH_WRITE(0x02, 0x10); //DATA2
  TRH_WRITE(0x02, 0x52); //DATA3
  TRH_WRITE(0x02, 0x41); //DATA4
  TRH_WRITE(0x02, 0x43); //DATA5
  TRH_WRITE(0x02, 0x4F); //DATA6
  TRH_WRITE(0x02, 0x53); //DATA7
  //#############################################
  TRH_WRITE(0x02, 0x00); //Le
  
  TRH_WRITE(0x01, 0x14);
  
  delay_ms(50);
  
  TRH_RxLen = 0;
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN != 0) && !(TRH_ERR&0x08))
  {
    for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
    #if nfc_debug
    //UART_STRING("\r\n HCE SELECT DF = ");for(i=0;i<TRH_RxLen;i++) UART_H2A(TRH_RxBuf[i]);UART_STRING("\n\r");;UART_STRING("\n\r");
    #endif
    if((TRH_RxBuf[TRH_LEN-2] == 0x90)&&(TRH_RxBuf[TRH_LEN-1]==0x00))
    {
      if(TRH_RxBuf[2] == 0x00) return 1;
      else return 2;
    }
    else{
      if(TRH_RxBuf[0] == 0xFA)
      {
        for(i=0;i<3;i++){
          tmp = HCE_FAProcess();
          if(tmp == 0) return 0;
          else if(tmp == 1){
            if(TRH_RxBuf[2] == 0x00) return 1;
            else return 2;
          }
        }
	
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
        return 0;
      }
      else 
	{
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
	return 0;
      }
    }
  }
  else
  {
    return 0;
  }
}
/*
u8 HCE_READBINARY(void)
{
  u8 i;
  
  TRH_WRITE(0x09, 0x01);
  
  ISO14443_PCB ^= 0x01;
  TRH_WRITE(0x02, ISO14443_PCB);
  TRH_WRITE(0x02, 0x00);
  //#############################################
  TRH_WRITE(0x02, 0x00); //CLA
  TRH_WRITE(0x02, 0xB0); //INS
  TRH_WRITE(0x02, 0x80); //P1
  TRH_WRITE(0x02, 0x01); //P2
  //#############################################
  TRH_WRITE(0x02, 0x00); //Lc
  
  TRH_WRITE(0x07, 0x7F); //
  TRH_WRITE(0x01, 0x14);
  
  Timer0_mSec_Start(500);
  while((TRH_READ(0x07)&0x10) == 0){
    WDT ^= 1;
    if(T0IRQ) return 0;
  }
  Timer0_mSec_Stop();
  
  TRH_RxLen = 0;
  Timer0_mSec_Start(500);
  while(1){
    WDT ^= 1;
    if(TRH_READ(0x07)&0x08){
      Timer0_mSec_Stop();
      TRH_LEN = TRH_READ(0x04);
      TRH_ERR = TRH_READ(0x0A);
      TRH_WRITE(0x01, 0x00);
      TRH_WRITE(0x2E, 0x00);
      if((TRH_LEN != 0) && !(TRH_ERR&0x08)){
        for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
        #if nfc_debug
        UART_STRING("\r\n HCE READBINARY = ");for(i=0;i<TRH_RxLen;i++) UART_H2A(TRH_RxBuf[i]);UART_STRING("\n\r");;UART_STRING("\n\r");
        #endif
        if((TRH_RxBuf[TRH_LEN-2] == 0x90)&&(TRH_RxBuf[TRH_LEN-1]==0x00)){
          return 1;
        }
        else return 0;
      }
      else
      {
        return 0;
      }
    }
    else{
      if(T0IRQ){
        Timer0_mSec_Stop();
        return 0;
      }
      else{
        if(TRH_READ(0x04) > 64){
          for(i=0;i<60;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
        }
      }
    }
  }
}  
*/


u8 HCE_READBINARY(void)
{
  u8 i;
  u8 tmp;
  
  TRH_WRITE(0x09, 0x01);
  
  ISO14443_PCB ^= 0x01;
  TRH_WRITE(0x02, ISO14443_PCB);
  TRH_WRITE(0x02, 0x00);
  //#############################################
  TRH_WRITE(0x02, 0x00); //CLA
  TRH_WRITE(0x02, 0xB0); //INS
  TRH_WRITE(0x02, 0x80); //P1
  TRH_WRITE(0x02, 0x01); //P2
  //#############################################
  TRH_WRITE(0x02, 0x00); //Lc
  
  TRH_WRITE(0x01, 0x14);
  
  delay_ms(50);
  
  TRH_RxLen = 0;
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN != 0) && !(TRH_ERR&0x08)){
    for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
    #if nfc_debug
    UART_STRING("\r\n HCE READBINARY = ");for(i=0;i<TRH_RxLen;i++) UART_H2A(TRH_RxBuf[i]);UART_STRING("\n\r");;UART_STRING("\n\r");
    #endif
    if((TRH_RxBuf[TRH_LEN-2] == 0x90)&&(TRH_RxBuf[TRH_LEN-1]==0x00))
	{
		
      		return 1;
	}
    else{
      if(TRH_RxBuf[2] == 0xBD)
      {
	if(TRH_RxBuf[8] == 0x81)		//Check Switching DFU Mode Command
	{
		//if(TRH_RxBuf[10] == GetChecksum(&TRH_RxBuf[4],10))	//Check DFU CheckSum
		//{
			//BootloaderAckSend();			//Respond Switching Command
			//EEPROM_write(0x0005, 0x01);		//New F/W Flag
			delay_ms(100);
			asm("jmp 57344");		//Enter Bootliader Section
		//}
	}
      }
      else if(TRH_RxBuf[0] == 0xFA){
        for(i=0;i<3;i++){
          tmp = HCE_FAProcess();
          if(tmp == 0) return 0;
          else if(tmp == 1){
            if(TRH_RxBuf[2] == 0x00) 
		{
			
				return 1;
            	}
            else 
		{
			
				return 2;
            	}
          }
        }
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
        return 0;
      }
      else
	{
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
		return 0;
      }
    }
  }
  else
  {
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
    return 0;
  }
}

u8 HCE_WRITEBINARY(u8 len, u8 *data)
{
  u8 i;
  u8 tmp;
  #if nfc_debug
  UART_STRING("\r\n HCE_WRITEBINARY DATA = ");for(i=0;i<len;i++) UART_H2A(data[i]);
  #endif
  
  TRH_WRITE(0x09, 0x01);
  
  ISO14443_PCB ^= 0x01;
  TRH_WRITE(0x02, ISO14443_PCB);
  TRH_WRITE(0x02, 0x00);
  //#############################################
  TRH_WRITE(0x02, 0x00); //CLA
  TRH_WRITE(0x02, 0xD0); //INS
  TRH_WRITE(0x02, 0x80); //P1
  TRH_WRITE(0x02, 0x01); //P2
  TRH_WRITE(0x02, len); //Le
  for(i=0;i<len;i++) TRH_WRITE(0x02, data[i]);
  //#############################################
  TRH_WRITE(0x02, 0x00); //Lc
  
  TRH_WRITE(0x01, 0x14);
  delay_ms(50);
  
  TRH_RxLen = 0;
  TRH_LEN = TRH_READ(0x04);
  TRH_ERR = TRH_READ(0x0A);
  TRH_WRITE(0x01, 0x00);
  TRH_WRITE(0x2E, 0x00);
  
  if((TRH_LEN != 0) && !(TRH_ERR&0x08)){
    for(i=0;i<TRH_LEN;i++) TRH_RxBuf[TRH_RxLen++] = TRH_READ(0x02);
    #if nfc_debug
    UART_STRING("\r\n HCE WRITEBINARY = ");for(i=0;i<TRH_RxLen;i++) UART_H2A(TRH_RxBuf[i]);UART_STRING("\n\r");;UART_STRING("\n\r");
    #endif
    if(TRH_RxBuf[0] == 0xFA){
      for(i=0;i<3;i++){
        tmp = HCE_FAProcess();
        if(tmp == 0) return 0;
        else if(tmp == 1){
          if(TRH_RxBuf[2] == 0x00) return 1;
          else return 2;
        }
      }
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
      return 0;
    }
    else return 1;
  }
  else
  {
  	TRH_WRITE(0x11,0x58);
	delay_ms(50);
	TRH_WRITE(0x11,0x5B);
    return 0;
  }
}

BYTE None_RE(void)
{//전송 파트.
  	u8 tmp;
 	u16 index,i;
    if(TRH_NFCTxEn){
      #if nfc_debug
      UART_STRING("\r\nHCE Tx Setp!!");
      #endif
      TRH_NFCRxPtr = 0;
      index = TRH_NFCTxLen/100;
      if(TRH_NFCTxLen%100) index++;
      
    
      while(1){
        TRH_TxLen = 0;
        TRH_TxBuf[TRH_TxLen++] = (u8)(index>>8);
        TRH_TxBuf[TRH_TxLen++] = (u8)(index);
        TRH_TxBuf[TRH_TxLen++] = 0;
		
        for(i=0;i<100;i++)
        {
            TRH_TxBuf[TRH_TxLen++] = TRH_NFCTxBuf[TRH_NFCRxPtr++];
			
			
            //if(TRH_NFCRxPtr == TRH_NFCRxLen) break;
            if(TRH_NFCTxLen == TRH_NFCRxPtr) break;
        }
	 #if ksy_debug
	UART0_TX_EN;
	UART_STRING("\r\n TRH_TxBuf = ");
	delay_ms(5);
	for(i=0;i<TRH_TxLen;i++) UART_H2A(TRH_TxBuf[i]);
	
	delay_ms(10);
	UART0_RX_EN;
	#endif
        //TRH_TxBuf[2] = (TRH_TxLen - 3);
        TRH_TxBuf[2] = TRH_TxLen;
        tmp = HCE_WRITEBINARY(TRH_TxLen, TRH_TxBuf);

	
        //tmp = HCE_WRITEBINARY(tx_data0[3], tx_data0);
        if(tmp == 0) return 0;
        if(index != 0){
          index--;
          if(index == 0){
            TRH_NFCTxEn = 0;
	   
            break;
          }
        }
      }
    }
  }

BYTE Send_RE(void)
{//전송 파트.
  	u8 tmp,flag,acnt,length;
 	u16 index,i,ckReadAddr,ecnt;
	
	//RE_flag=0;
    if(TRH_NFCTxEn){
      #if nfc_debug
      UART_STRING("\r\nHCE Tx Setp!!");
      #endif
      TRH_NFCRxPtr = 0;
     
      while(1){
        TRH_TxLen = 0;
        TRH_TxBuf[TRH_TxLen++] = (u8)(RE_index>>8);
        TRH_TxBuf[TRH_TxLen++] = (u8)(RE_index);
        TRH_TxBuf[TRH_TxLen++] = 0;
	if(RE_flag==0)
		{
			if(RE_cnt>8)
				length=0xff;
			else
				length=RE_cnt*30+11;
			TRH_TxBuf[TRH_TxLen++] = STX_ACU;
			TRH_TxBuf[TRH_TxLen++] = 0;
			TRH_TxBuf[TRH_TxLen++] = 0;
			TRH_TxBuf[TRH_TxLen++] = length;
			TRH_TxBuf[TRH_TxLen++] = RE_Com;
			TRH_TxBuf[TRH_TxLen++] = RE_SCom;
			TRH_TxBuf[TRH_TxLen++] = 0;
			TRH_TxBuf[TRH_TxLen++] = (BYTE)(RE_cnt>>8);
			TRH_TxBuf[TRH_TxLen++] = (BYTE)RE_cnt;
			
		}
	else if(RE_flag==1)
		{
			acnt=0;
			   for(i=RE_SA;i<=RE_EA;i++)
			        {
			        	RE_ecnt++;
					acnt++;
					
					if(RE_mode==0)
						{
							if(i>400)
							pEnterSend=i-400;
							else
							pEnterSend=i;
							if(i>400)
								{
									//pEnterSend=1;
									OverwriteState=0;
									ckReadAddr = eSETSTART + eOVERWRITE;
		  						       Save_Byte_Data(ckReadAddr, OverwriteState); 
								}
							
							
						}
					if(i>400)
			          	 ckReadAddr = (i-400) * 32;
					else
					ckReadAddr = i * 32;
				        ReadExtMemoryPageData(ckReadAddr, 30, &TRH_TxBuf[TRH_TxLen]);
					TRH_TxBuf[TRH_TxLen+28]=(BYTE)(RE_ecnt>>8);
					TRH_TxBuf[TRH_TxLen+29]=(BYTE)RE_ecnt;
				        TRH_TxLen = TRH_TxLen + 30;   
						
					if(acnt==3)
						{
							RE_SA=i+1;
							i=0xffff;
							break;
							
						}
			        }
			
		}
	if(RE_flag==2)
		{
			TRH_TxBuf[TRH_TxLen++]=0;
			TRH_TxBuf[TRH_TxLen++]=ETX_ACU;
		}
     
	
	
        //TRH_TxBuf[2] = (TRH_TxLen - 3);
        TRH_TxBuf[2] = TRH_TxLen;
	//if(!RE_end)
	
			
        tmp = HCE_WRITEBINARY(TRH_TxLen, TRH_TxBuf);

	
        //tmp = HCE_WRITEBINARY(tx_data0[3], tx_data0);
       if(RE_index > 0)
	{
	          RE_index--;
	          if(RE_index == 0){
		     //LED_GRE=1;
	            TRH_NFCTxEn = 0;
		    
	            break;
	          }
        }
	else
		{
			TRH_NFCTxEn = 0;
	  		 break;
		}
        if(tmp == 0) 
			{
						RE_index++;
						RE_SA=RE_SA-acnt;
							RE_ecnt=RE_ecnt-acnt;
							//RE_ecnt=RE_ecnt-acnt;
							return 0;
				
			}
	   if(RE_ecnt>=RE_cnt)
			   	RE_flag=2;	
	if(RE_flag==0)
		RE_flag=1;
	
      }
    }
  }


u8 ISO14443_RunFunction(void)
{
  u16 i;
  u8 tmp,j;
  u16 index,ckReadAddr;
  
  tmp = HCE_SELECTDF();
  //UART_STRING("\r\n temp return = ");UART_H2A(tmp);
    		
  if(tmp == 0) return 0;
  else if(tmp == 1)
  {
  	if(!RE_flagm)
  		{
  			
  			j=None_RE();
  		}
  		
	else
		{
			
			j=Send_RE();
			if(RE_mode==0)
			{
				ckReadAddr = eSETSTART + eENTERSEND;
		            Save_Word_Data(ckReadAddr, pEnterSend); 
			}
		}
		
  }
  else if(tmp == 2)
  {//수신 파트.
 
    TRH_NFCRxLen = 0;
    index = 0;
    while(1)
    {
      
      tmp = HCE_READBINARY();
      if(tmp == 0) return 0;
      index = (u16)TRH_RxBuf[2];
      index <<= 8;
      index |= (u16)TRH_RxBuf[3];
	
      for(i=0;i<TRH_RxBuf[4]-3;i++) 
	  	{
	  		TRH_NFCRxBuf[TRH_NFCRxLen++] = TRH_RxBuf[5+i];
			
	
	  	}     
   
	 #if ksy_debug
	UART0_TX_EN;
	UART_STRING("\r\n TRH_NFCRxBuf = ");
	delay_ms(5);
	for(i=0;i<TRH_NFCRxLen;i++) UART_H2A(TRH_NFCRxBuf[i]);
	
	delay_ms(10);
	UART0_RX_EN;
	#endif
      if(index == 0x0001)
      {
        TRH_NFCRxEn = 1;
        break;
      }
      
    }
  }
 
  
  return 1;
}


u8 HCE_RxRunFunction(void)
{
  u16 i;
  if(TRH_NFCRxEn)
  {
  	
  	TRH_WRITE(0x11,0x58);
    TRH_NFCRxEn = 0;
	    //=====RACOS Protocol====//
	    NewData0 = 1; 
//	    for(i=0;i<TRH_RxBuf[4]-3;i++) TRH_NFCRxBuf[TRH_NFCRxLen++] = TRH_RxBuf[5+i];  
	    for(i=0;i<TRH_NFCRxLen;i++) RxData0[i] = TRH_NFCRxBuf[i];
	    //UART_STRING("\r\n RX Data = ");for(i=0;i<TRH_NFCRxLen;i++) UART_H2A(RxData0[i]);
	    //UART_STRING("\r\n TRH_NFCRxLen = ");UART_H2A(TRH_NFCRxLen);
	    #if racos_debug

	    #endif
	    TRH_NFCTxLen = 0;
	    //Check_Main_Data();
	    Check_NFC_Data();
	    #if nfc_debug
	    UART_STRING("\r\n HCE Rx Data = ");for(i=0;i<TRH_NFCRxLen;i++) UART_H2A(TRH_NFCRxBuf[i]);
	    
	    #endif

	    TRH_NFCTxEn = 1;
	TRH_WRITE(0x11,0x5B);
  }
       
    
  return 1;
}





