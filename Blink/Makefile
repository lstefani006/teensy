CXXFLAGS=-W3 -Os -I../TeensyLib

TARGET=Blink.hex
SRC=\
	Blink.cpp

include ../TeensyLib/Makefile.inc

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
