ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../TeensyLib
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif

TARGET=Blink.hex
SRC+=\
	Blink.cpp


ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else ifdef ESP8266
	include ../EspLib/esp_makefile.mk
else
	include ../AvrLib/avr_makefile.mk
endif
