CXXFLAGS = -I. #-V
CXXFLAGS+= -I$(ARDUINO)/libraries/SPI
CXXFLAGS+= -W3
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/libraries/SPI

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
