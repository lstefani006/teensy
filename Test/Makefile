CXXFLAGS=-W3 -Os
CXXFLAGS+= -I. -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(TEENSYLIB)

TARGET=Test.hex
SRC=\
	Test.cpp

include ../TeensyLib/Makefile.inc

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
