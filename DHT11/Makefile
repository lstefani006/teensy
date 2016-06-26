ifdef TEENSY
CXXFLAGS=-W2 -Os -I../TeensyLib -V -I.
VPATH=../TeensyLib
else
CXXFLAGS=-I../AvrLib -I.
VPATH=../AvrLib
endif

TARGET=DHT11.hex
SRC=\
	DHT11.cpp \
	idDHTLib.cpp
#	t_io.cpp

ifdef TEENSY
else
#	SRC+= timer.cpp 
endif

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
