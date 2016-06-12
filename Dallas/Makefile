ifdef TEENSY
	CXXFLAGS=-W3 -Os -I../TeensyLib -V
	VPATH=../TeensyLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif



CXXFLAGS+= -I. -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(TEENSYLIB)
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
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

