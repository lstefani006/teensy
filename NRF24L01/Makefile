CXXFLAGS += -I RF24-master
CXXFLAGS += -I $(ARDUINO)/hardware/arduino/avr/libraries/SPI
CXXFLAGS += -W3
CXXFLAGS += -Os

VPATH    += $(ARDUINO)/hardware/arduino/avr/libraries/SPI
VPATH    += ./RF24-master

SRC = RF24.cpp main.cpp

TARGET=RF24_Test.hex

include ../TeensyLib/Makefile.inc
