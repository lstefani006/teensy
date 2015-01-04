CXXFLAGS = -I. -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(TEENSYLIB)
CXXFLAGS+= -I$(ARDUINO)/libraries/SPI
CXXFLAGS+= -I$(ARDUINO)/libraries/TimerThree
CXXFLAGS+= -I$(ARDUINO)/libraries/OneWire
CXXFLAGS+= -W3
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/libraries/SPI
VPATH+=$(ARDUINO)/libraries/TimerThree
VPATH+=$(ARDUINO)/libraries/OneWire

SRC=main.cpp
SRC+=SPI.cpp
SRC+=TimerThree.cpp
SRC+=t_5110.cpp
SRC+=OneWire.cpp
SRC+=gpio.cpp

TARGET=5110.hex

include /home/leo/teensy/TeensyLib/Makefile.inc
