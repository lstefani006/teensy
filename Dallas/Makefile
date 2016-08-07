ifdef TEENSY
	CXXFLAGS=-W3 -Os -I../TeensyLib -V
	VPATH=../TeensyLib
	CXXFLAGS+= -I$(TEENSYLIB)
else
	CXXFLAGS=-I../AvrLib -Os
	VPATH=../AvrLib
endif

CXXFLAGS+=  -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire

SRC=dallas.cpp
SRC+=OneWire.cpp
SRC+=DallasTemperature.cpp

ifndef TEENSY
    SRC += t_io.cpp
endif

TARGET=Dallas.hex

ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else
	include ../AvrLib/avr_makefile.mk
endif

