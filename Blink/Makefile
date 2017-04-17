V=1
#E=
G=-g
O=

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../TeensyLib
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../StLib -g
	VPATH=../StLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif

TARGET=Blink.hex
SRC+=\
	Blink.cpp

include ../util/makefile.mk
