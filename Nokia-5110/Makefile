ifdef TEENSY
CXXFLAGS=-W2 -Os -I../TeensyLib -V
VPATH=../TeensyLib
else
CXXFLAGS=-I../AvrLib
VPATH=../AvrLib
endif

CXXFLAGS+= -I. -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
#CXXFLAGS+= -W3
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire

ifdef TEENSY
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
SRC+=TimerThree.cpp
else
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne
SRC+=TimerOne.cpp
endif

SRC+=nokia_temp.cpp
SRC+=SPI.cpp
SRC+=t_5110.cpp
SRC+=OneWire.cpp
SRC+=gpio.cpp
ifndef TEENSY
SRC+=t_io.cpp
endif

TARGET=5110.hex

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif
