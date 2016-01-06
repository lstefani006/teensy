ifdef TEENSY
CXXFLAGS=-W2 -Os -I../TeensyLib -V
else
CXXFLAGS=-I../TeensyLib
endif

VPATH=../TeensyLib
TARGET=Blink.hex
SRC=\
	Blink.cpp \
	t_io.cpp


ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
