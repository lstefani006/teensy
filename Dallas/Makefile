ifdef TEENSY
	CXXFLAGS=-W3 -Os -I../TeensyLib -V
	VPATH=../TeensyLib
	CXXFLAGS+= -I$(TEENSYLIB)
else ifdef STM32F103
	CXXFLAGS=-I../LibStm -Os -I. -I../util
	VPATH=../LibStm
else
	CXXFLAGS=-I../LibAvr -Os
	VPATH=../LibAvr
endif

CXXFLAGS+=  -DONEWIRE_CRC8_TABLE=0
#CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
#VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire

SRC=dallas.cpp
SRC+=OneWire.cpp
SRC+=DallasTemperature.cpp

ifndef TEENSY
    SRC += t_io.cpp
endif

TARGET=Dallas.hex

ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else ifdef STM32F103
	include ../util/st_makefile.mk
else
	include ../util/avr_makefile.mk
endif

