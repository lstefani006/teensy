V=1
#E=
G=-g
O=

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../LibTeensy
else  ifdef ESP8266
	CXXFLAGS=-I../LibEsp
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../LibSt -g
	VPATH=../StLib
else
	CXXFLAGS=-I../LibAvr
	VPATH=../AvrLib
endif

TARGET=Blink.hex
SRC+=\
	Blink.cpp

include ../util/makefile.mk
