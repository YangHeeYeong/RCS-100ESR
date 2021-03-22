###############################################################################
# Makefile for the project RCS-800ER_Ver0_00
###############################################################################

## General Flags
PROJECT = RCS-800ER_Ver0_00
MCU = atmega128
TARGET = RCS_100ESR_ver1.00.00.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                   -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=RCS_100ESR_ver1.00.00.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Library Directories
LIBDIRS = -L"D:\last_file\RCS-100ESR\pro\lastesr\lib" 

## Libraries
LIBS = -lmifare 

## Objects that must be built in order to link
OBJECTS = 24lc256.o card.o doorlock.o e2prom.o iso14443.o iso14443a.o key_input.o main.o mcu.o mifare.o mifare_crypto.o reader.o rtc.o server.o system.o timer.o tmoney.o trh033m.o uart.o Online.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) RCS_100ESR_ver1.00.00.hex RCS_100ESR_ver1.00.00.eep RCS_100ESR_ver1.00.00.lss size

## Compile
24lc256.o: ../24lc256.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

card.o: ../card.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

doorlock.o: ../doorlock.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

e2prom.o: ../e2prom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

iso14443.o: ../iso14443.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

iso14443a.o: ../iso14443a.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

key_input.o: ../key_input.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mcu.o: ../mcu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mifare.o: ../mifare.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mifare_crypto.o: ../mifare_crypto.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

reader.o: ../reader.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtc.o: ../rtc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

server.o: ../server.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

system.o: ../system.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer.o: ../timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tmoney.o: ../tmoney.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

trh033m.o: ../trh033m.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Online.o: ../Online.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) RCS_100ESR_ver1.00.00.elf dep/* RCS_100ESR_ver1.00.00.hex RCS_100ESR_ver1.00.00.eep RCS_100ESR_ver1.00.00.lss RCS_100ESR_ver1.00.00.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

