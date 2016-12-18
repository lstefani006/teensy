V=1

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../TeensyLib
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../StLib
	VPATH=../StLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif

TARGET=Blink.hex
SRC+=\
	Blink.cpp


ifdef TEENSY
	include ../util/teensy_makefile.mk
else ifdef ESP8266
	include ../util/esp_makefile.mk
else ifdef STM32F103
	include ../util/st_makefile.mk
else
	include ../util/avr_makefile.mk
endif
