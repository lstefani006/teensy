CXXFLAGS = -I. #-V
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -W3
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI

TARGET=522.hex
SRC=\
	MFRC522.cpp \
	522.cpp \
	SPI.cpp

include ../TeensyLib/Makefile.inc

tags : 
	hdrtag *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
