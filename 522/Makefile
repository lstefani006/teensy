V=1

VPATH+=./crypto

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib  -I.
	CXXFLAGS= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI -I.
	VPATH=../TeensyLib
	VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib -I.
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../StLib -I.
	VPATH+=../StLib
	VPATH+=../util
	VPATH+=$(HOME)/Arduino/hardware/Arduino_STM32/STM32F1/libraries/SPI/src
	CXXFLAGS+=-I../util
	CXXFLAGS+=-I$(HOME)/Arduino/hardware/Arduino_STM32/STM32F1/libraries/SPI/src
else
	CXXFLAGS=-I. -I../util -I../rfid/src
	CXXFLAGS+=-I$(ARDUINO)/hardware/arduino/avr/libraries/SPI/src -I.
	VPATH+=../util
	VPATH+=$(ARDUINO)/hardware/arduino/avr/libraries/SPI/src
	VPATH+=../rfid/src
endif

TARGET=522.hex
SRC+=\
	MFRC522.cpp \
	MFRC522Debug.cpp \
	522_main.cpp \
	SPI.cpp \
	uprintf.cpp 

#ifndef ARDUINO
SRC += des.cpp des3.cpp rsa.cpp aes.cpp endian.cpp debug.cpp asn1.cpp
#endif

include ../util/makefile.mk

tags : 
	hdrtag *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
