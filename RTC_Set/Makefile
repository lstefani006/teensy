ifdef TEENSY
CXXFLAGS=-W2 -I../TeensyLib -V
VPATH=../TeensyLib
else
CXXFLAGS=-I../AvrLib
VPATH=../AvrLib
endif

TARGET=RTC_Set.hex
CXXFLAGS+=-Os
CXXFLAGS+= -I$(TEENSYLIB)

SRC=\
	main.cpp \
	RTC_DS1302.cpp \
	t_io.cpp


ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif
