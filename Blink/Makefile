CXXFLAGS=-W3 -Os

TARGET=Blink.hex
SRC=\
	Blink.cpp

include ../TeensyLib/Makefile.inc

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
