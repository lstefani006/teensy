CXXFLAGS=-W2 -Os -I../TeensyLib -V

TARGET=Blink.hex
SRC=\
	Blink.cpp

include ../TeensyLib/Makefile.inc

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
