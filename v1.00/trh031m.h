/*
********************************************************************************
* 3ALogics Inc.
* 7th Floor, Hyundai-office Bldg., 9-4, Sunae-dong. Bundang-gu, Seongnam-si,
* Gyeonggi-do, 463-783, Korea
*
* (c) Copyright 2007, 3ALogics, Korea
*
* Hardware : RSK-300
* Filename : TRH033M.h
* Version  : 1.0
* Programmer(s): Suk-Byoung Chea
* Created : 2007/12/10
* Description : This file define Registers of TRH033M.
********************************************************************************
*/

#ifndef _TRH031M_H
#define _TRH031M_H

#define REG_CLEAR	0x00

enum TRH031M_REG{
	//Page 0 register
	REG_PAGE0		= 0x00,	//Selects the register page0                                  
	REG_COMMAND		= 0x01, //Starts and stops the command execution          
	REG_FIFODATA	= 0x02, //Input and output of 64 byte FIFO buffer register
	REG_STATUS1		= 0x03, //Shows the Reader chip status                    
	REG_FIFOLENGTH	= 0x04,	//Number of bytes buffered in the FIFO            
	REG_STATUS2		= 0x05,	//Shows the Reader chip status                    
	REG_IEN			= 0x06,	//Interrupt enable register                       
	REG_IRQ			= 0x07,	//Interrupt request register

	//Page 1 register
	REG_PAGE1		= 0x08,	//Selects the page1                              
	REG_CONTROL		= 0x09,	//Control register                       
	REG_ERRFLAG		= 0x0A,	//Command error on last activity         
	REG_COLLPOS		= 0x0B,	//Location of bit on last collision error
	REG_TIMERVALUE		= 0x0C,	//Actual timer value                             
	REG_CRCRESULTLSB	= 0x0D,	//LSB of CRC result register                     
	REG_CRCRESULTMSB	= 0x0E,	//MSB of CRC result register                     
	REG_BITFRAME		= 0x0F,	//ISO 14443 A bit level transmission control    

	//Page 2 register
	REG_PAGE2		= 0x10,	//Selects the page2
	REG_TXCONTROL		= 0x11,	//Control TX1 and TX2                    
	REG_CWCONDUCTANCE	= 0x12,	//Adjust TX pin output power             
	REG_MODCONDUCTANCE	= 0x13,	//Adjust ASK Modulation index            
	REG_CODCONTROL		= 0x14,	//Set TX Encoding method                 
	REG_MODWIDTH		= 0x15,	//Modify modulation width for 14443A Type
	REG_TOFFWIDTH		= 0x16,	//Determine Off width for Tag-It protocol
	REG_BFRAMMING		= 0x17,	//Set framing method for ISO 14443 B 

	//Page 3 register
	REG_PAGE3		= 0x18,	//Selects the page3         
	REG_RXCONTROL1		= 0x19,	//Receiver control register
	REG_DECODCONTROL	= 0x1A,	//Decoder control register
	REG_TDIFF		= 0x1B, //Tag-it Receive Frequency Measure Value
	REG_RXTHRESHOLD		= 0x1C,	//Set demodulation standard
	REG_RXCONTROL2		= 0x1E,	//Select decoder input  
	REG_AWIDTHCUT		= 0x1F, //A Type Threshold Control resister

	//Page 4 register
	REG_PAGE4		= 0x20,	//Select page4                                     
	REG_RXWAIT		= 0x21,	//Set idle time between TX ending and RX beginning
	REG_REDUNDANCY		= 0x22,	//Set data integrity verifying method             
	REG_CRCPRESETLSB	= 0x23,	//LSB of CRC preset register                      
	REG_CRCPRESETMSB	= 0x24,	//MSB of CRC preset register    
	REG_RXTHRESHOLD2	= 0x25, //Reciver off state RX Threshold value resister
	REG_TESTOUTSEL		= 0x26,	//TESTOUT pin source selection register   

	//Page 5 register
	REG_PAGE5		= 0x28,	//Selects the page5                 
	REG_FIFOLEVEL		= 0x29,	//Reference value of FIFO interrupt
	REG_TIMERCLK		= 0x2A,	//Set timer speed                  
	REG_TCONTROL		= 0x2B,	//Set timer begin and end condition
	REG_TRELOADVALUE	= 0x2C,	//Set timer initial value          
	REG_IRQCONFIG		= 0x2D,	//IRQ pin configuration register   
	
};

//############################################################################################//

//---------------------------------------------------------------------------------------------
//PAGE 0 Registers
//---------------------------------------------------------------------------------------------

//	REG_COMMAND		= 0x01, //Starts and stops the command execution	(Read/Write) 
enum REG_COMMAND_BIT{

	INIT			= 0x3F,	//Initializing system.	
	IDLE			= 0x00,	//Stop command or maintain idle.
	TRANSMIT		= 0x1A,	//Transmit data from FIFO.
	RECEIVE			= 0x16,	//Receive data to FIFO.
	TRANSCEIVE		= 0x1E,	//Transmit data to FIFO and thereafter receive automatically.
};

//	REG_STATUS1		= 0x03, //Shows the Reader chip status	(Only Read) 
enum REG_STATUS1_BIT{
	MS_IDLE			= 0x00, //ready status
	MS_TXSOF		= 0x10, //transmitting the SOF(Start Of Flame).
	MS_TXDATA		= 0x20, //transmitting data of FIFO buffer.
	MS_TXEOF		= 0x30, //Transmitting EOF(End Of Flame).
	MS_GTR			= 0x40, //starting receive
	MS_PR			= 0x50, //waiting till selected period in the RxWait register is expired
	MS_AR			= 0x60, //waiting for the receiving signal
	MS_REV			= 0x70, //receiving the signal

	IRQ			= 0x08, //Displays interrupt occurrence. Set interrupt use by utilizing IEN register.
	ERR			= 0x04, //Showing an error state
	HiAlert			= 0x02, 
	LoAlert			= 0x01,
};

//	REG_STATUS2		= 0x05,	//Shows the Reader chip status	(Only Read) 
enum REG_STATUS2_BIT{
	TRunning		= 0x80, //Set is Running internal timer, clear is Stopping internal timer

	LockStatus1		= 0x40, //During operating Tag-It protocol, LockStatus of Memory Block save in GetBlock Response.
	LockStatus2		= 0x20, // 

	TerFlag			= 0x10, //During operating Tag-It protocol, Saving ErrorFlag from tag response
	TadFlag			= 0x08, //During operating Tag-It protocol, Saving Address Flag from tag response on Tag-It protocol

	RxLastBits2		= 0x04, //1 During operating Anti-Collision of 14443 A Binary-Tree type, 
	RxLastBits1		= 0x02, //saving a received split byte. Not used except A-type protocol.
	RxLastBits0		= 0x01, //
};

//	REG_IEN			= 0x06,	//Interrupt enable register		(Read/Write) 
enum REG_IEN_BIT{
	SetIEN			= 0x80, //Clear Bit Make ¡®0¡¯ marked flag from 6 to 0.Set Bit.Make ¡®1¡¯ marked flag from 6 to 0..
	TimerIEn		= 0x20, //Clear Bit Not transfer TimerIrq interrupt signal to IRQ pin. Set Bit Transfer TimerIrq interrupt signal to IRQ pin
	TxIEn			= 0x10, //Clear Bit Not transfer TxIrq interrupt signal to IRQ pin. Set Bit Transfer TxIrq interrupt signal to IRQ pin
	RxIEn			= 0x08, //Clear Bit Not transfer RxIrq interrupt signal to IRQ pin. Set Bit Transfer RxIrq interrupt signal to IRQ pin.
	IdleEn			= 0x04, //Clear Bit Not transfer IdleIrq interrupt signal to IRQ pin. Set Bit Transfer IdleIrq interrupt signal to IRQ pin
	HiAlertIEn		= 0x02, //Clear Bit Not transfer HiAlertIrq signal to IRQ pin. Set Bit Transfer HiAlertIrq signal to IRQ pin
	LoAlertIEn		= 0x01, //Clear Bit Not transfer LoAlertIrq interrupt signal to IRQ pin. Set Bit Transfer LoAlertIrq interrupt signal to IRQ pin
};

//	IRQ_REG_IRQ			= 0x07,	//Interrupt request register	(Read/Write) 
enum RE_IRQ_IRQ_BIT{
	IRQ_SetIRq		= 0x80, //Clear Bit Make marked flag to 0 from bit number 6~0. Set Bit Make marked flag to1 from bit number 6~0.
	IRQ_TimerIrq		= 0x20, //
	IRQ_TxIrq		= 0x10, //
	IRQ_RxIrq		= 0x08, //
	IRQ_IdleIrq		= 0x04, //
	HiAlertIrq		= 0x02, //
	LoAlertIrq		= 0x01, //
};

//---------------------------------------------------------------------------------------------
//PAGE 1 Registers
//---------------------------------------------------------------------------------------------
// REG_CONTROL			= 0x09,	//Control register   (Read/Write)             
enum REG_CONTROL_BIT{
	PowerDown		= 0x10, //Clear Bit Power down mode not enabled.Set Bit Use power down mode. Minimize TRH031M power consumption
	TStopNow		= 0x04, //This bit becomes ¡®0¡¯ automatically. Set Bit Stopping timer.
	TStartNow		= 0x02, //This bit becomes ¡®0¡¯ automatically. Starting timer.
	FlushFIFO		= 0x01, //This bit becomes ¡®0¡¯ automatically. Deleting all data of FIFO
};       
// REG_ERRFLAG			= 0x0A,	//Command error on last activity    (Only Read)     
enum REGERRFLAG__ERRFLAG_BIT{
	ERRFLAG_FIFOOvf		= 0x10, //Clear Bit FIFO buffer is not full for data saving. Set Bit FIFO buffer is fully filled. Can¡¯t write data into the FIFO buffer.
	ERRFLAG_CRCErr		= 0x08, //Claer Bit Received CRC value is correct. Set Bit CRC calculation is incorrect. Received data has single or multiple errors.
	ERRFLAG_ParityErr	= 0x02, //Clear Bit Parity value is correct. Set Bit Parity value and data get error
	CollErr			= 0x01, //Clear Bit No Collision occurred. Set Bit Collision is detected. There are more than one tag.
};        

// REG_BITFRAME			= 0x0F,	//ISO 14443 A bit level transmission control    (Read/Write)
enum REG_BITFRAME_BIT{
	RxAlign2		= 0x40, //Indicates the bit position that first received data shall be stored.
	RxAlign1		= 0x20, //If the RxAlign is equal to 1, The first received bit of frame shall be stored at a bit position 1
	RxAlign0		= 0x10, //and Next shall be 2
	
	TMaskFlag		= 0x08, //Used for Tag-It protocol. 
					//From SID Request command it is used when using Mask Bit and Mask Length value is 
					//stored in TxLastBits, then, selected Mask value from data stored in FIFO and 
					//transmitted to Tag-It tag.
	
	TxLastBits2		= 0x04, //Used for bit level transmission and for 8 bit or lower data transmission.
	TxLastBits1		= 0x02, //When selecting 7, top 7 bits will be transmitted. Used for 14443 A-Type and Tag-It.
	TxLastBits0		= 0x01, //

};       

//---------------------------------------------------------------------------------------------
//PAGE 2 Registers
//---------------------------------------------------------------------------------------------
//REG_TXCONTROL		= 0x11,	//Control TX1 and TX2    
enum REG_TXCONTROL_BIT{
	
	MS1			= 0x40, //00: Constant Low 
	MS0			= 0x20, //01: Constant High
					//10: Internal coder source
					//11: TESTIN pin source
	F100ASK			= 0x10, //Clear bit --> Modulation depends on MODCONDUCTANCE Register.
					//Set bit --> Forces a 100% ASK Modulation.
	
	TX2Inv			= 0x08, //Clear bit --> TX2 pin will drives a non-inverted energy carrier.
					//Set bit --> TX2 pin will drives an inverted energy carrier.
	
	TX2CW			= 0x04, //Clear bit --> TX2 pin will drives continuously the modulated energy carrier.
					//Set bit --> TX2 pin will drives continuously the un-modulated energy carrier.
	
	TX2RFEN			= 0x02, //Claer bit --> TX2 pin will drives a constant output level 
					//Set bit --> TX2 pin will drives the energy carrier modulated by the transmission data
	TX1RFEN			= 0x01, //Clear bit --> TX1 pin will drives a constant output level
					//Set bit --> TX1 pin will drives the energy carrier modulated by the transmission data
};                

//REG_CWCONDUCTANCE	= 0x12,	//Adjust TX pin output power
enum REG_CWCONDUCTANCE_BIT{
					//CwConductance	Rp [Ohm]  
	CwConinfR		= 0x00, //0  		¡Ä
	CwCon10R		= 0x01, //1		10.14     
	CwCon6R			= 0x02, //2		6.33 
	CwCon5R			= 0x04, //4		4.60
	CwCon3R			= 0x08, //8		2.98 
	CwCon2R			= 0x0F, //16		2.11 
	CwCon1R			= 0x3F, //32  		1.63 
};                                      

enum REG_MODCONDUCTANCE_BIT{                                        
//REG_MODCONDUCTANCE	= 0x13,	//Adjust ASK Modulation index      
					//ModConductance	Rp [Ohm]
	ModConinfR		= 0x00, //0			¡Ä
	ModCon12R		= 0x01, //1			12.60
	ModCon10R		= 0x02, //2			10.14
	ModCon9R		= 0x04, //4			8.47
	ModCon7R		= 0x08, //8			7.25
	ModCon6R		= 0x0F, //16			6.33
	ModCon5R		= 0x3F, //32			5.07
};

//REG_CODCONTROL	= 0x14,	//Set TX Encoding method                 
enum REG_CODCONTROL_BIT{                
	TI_Addr			= 0x08, //Used to select Address mode type during Tag-It protocol Request command
	ISO14443A_TxCoding	= 0x01, //Set coder to ISO 14443 A Type mode
	ISO14443B_TxCoding	= 0x00, //Set coder to ISO 14443 B type mode
	ISO15693_256TxCoding	= 0x06, //Set coder to ISO 15693 standard mode ( 1 out of 256 coding )
	ISO15693_4TxCoding	= 0x07, //Set coder to ISO 15693 standard mode( 1 out of 4 coding)
	Tag_it_TxCoding		= 0x02, //Set coder to Tag-It Protocol mode
};

//REG_BFRAMMING		= 0x17,	//Set framing method for ISO 14443 B 
enum REG_BFRAMMING_BIT{
	EOFWidth		= 0x20, //Clear bit the EOF to length of 10 ETU
					//Set bit the EOF to length of 11 ETU
	CharSpacing2		= 0x10, //Set the EGT length between 0 and 7 ETU.
	CharSpacing1		= 0x08, //TEGT = CharSpacing128x1/fc
	CharSpacing0		= 0x04, //
	
	SOFWidth1		= 0x02, //00: Set the SOF to length of 10 ETU Low 2 ETU High.
	SOFWidth0		= 0x01, //01: Set the SOF to length of 10 ETU Low 3 ETU High.
					//10: Set the SOF to length of 11 ETU Low 2 ETU High.
                                        //11: Set the SOF to length of 11 ETU Low 3 ETU High.
};
 
//---------------------------------------------------------------------------------------------
//PAGE 3 Registers
//--------------------------------------------------------------------------------------------- 

//REG_RXCONTROL1	= 0x19,	//Receiver control register
enum REG_RXCONTROL1_BIT{
	VGAGain6	= 0x00, //000--> 06.02dB
	VGAGain12	= 0x01, //001--> 12.04dB
	VGAGain15	= 0x02, //010--> 15.56dB
	VGAGain18	= 0x03, //011--> 18.06dB
        VGAGain20       = 0x04, //100--> 20.00dB
};                                        
//REG_DECODCONTROL	= 0x1A,	//Decoder 101 ? 21.58dBcontrol register
enum REG_DECODCONTROL_BIT{
	ZrAfColl		= 0x20, //Clear bit Save received data after collision.                                
                            //Set bit Received data after collision save as¡®0¡¯.                                     
	ISO14443A_DeCoding      = 0x08,
	ISO14443B_DeCoding      = 0x18,
	ISO15693_DeCoding       = 0x10,
	Tag_it_DeCoding         = 0x00,
};

//REG_RXTHRESHOLD	= 0x1C,	//Set demodulation standard
enum REG_RXTHRESHOLD_BIT{
	CRO			= 0x40, //Confirm reference input voltage on comparator.
	
	HYR68mv			= 0x00, //Hysteresis Range
	HYR106mv		= 0x08, //Set hysteresis range on RX part comparator.
	HYR144mv		= 0x10, //
	HYR180mv		= 0x18, //
	HYR216mv		= 0x20, //
	HYR256mv		= 0x28, //
	
	
	CRV1743mv		= 0x00, //Comparator Reference Voltage
	CRV1713mv		= 0x01, //Set reference input voltage level on comparator
	CRV1681mv		= 0x02, //
	CRV1621mv		= 0x03, //
	CRV1591mv		= 0x04, //
	CRV1560mv		= 0x05, //
};


//REG_RXCONTROL2	= 0x1E,	//Select decoder input  
enum REG_RXCONTROL2_BIT{
	ADcdMd			= 0x80, //ISO14443A Type Decoding Mode Edge Select.
	TDcdMd			= 0x04, //Tag-it Decoding Mode Selcet. Use to defualut('0')
	DcdSrc			= 0x02, //Decoding source Signal Select
	 				//Clear bit, Tag response signal use
	 				//Set bit, TESTIN PIN signal use.
};
/*
//REG_AWIDTHCUT		= 0x1F, //A Type Threshold Control resister
enum REG_AWIDTHCUT_BIT{

};
*/
//---------------------------------------------------------------------------------------------
//PAGE 4 Registers
//---------------------------------------------------------------------------------------------                                    
//REG_REDUNDANCY	= 0x22,	//Set data integrity verifying method    
enum REG_REDUNDANCY_BIT{
	CRCWr		= 0x40, //Store received CRC value on FIFO.
	CRCB		= 0x20, //Clear bit ISO 14443 A Type
	                        //Set bit ISO/IEC3309 (ISO 14443 B Type and ISO 15693)
	RxCRCE		= 0x08, //Processing CRC calculation for received data.	
	TxCRCEn		= 0x04, //Adding CRC calculation for transmission data.
	ParityOdd	= 0x02, //Setting Parity calculation. 14443 A-type only
	                        //Clear bit even parity                               
	                        //Set bit odd parity                                
	
	ParityEn	= 0x01, //Setting parity error detecting code. 14443 A-type only
};         

//REG_TESTOUTSEL		= 0x26,	//TESTOUT pin source selection register
enum REG_TESTOUTSEL_BIT{
	TestSel_LOW	= 0x00, //Constant Low-->out signal to TESTOUT pin 
	TestSel_HIGH	= 0x01, //Constant High-->out signal to TESTOUT pin 
	TestSel_DEMO	= 0x02, //Demodulated received signal-->out signal to TESTOUT pin 
	TestSel_MODU	= 0x04, //Modulation source signal-->out signal to TESTOUT pin 
};


#endif
