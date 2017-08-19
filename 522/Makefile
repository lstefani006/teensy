V=1

VPATH+=./crypto

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib
	CXXFLAGS= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI -I.
	VPATH=../TeensyLib
	VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../StLib
	VPATH+=../StLib
	VPATH+=$(HOME)/Arduino/hardware/Arduino_STM32/STM32F1/libraries/SPI/src
	CXXFLAGS+=-I$(HOME)/Arduino/hardware/Arduino_STM32/STM32F1/libraries/SPI/src
else
	CXXFLAGS+=-I$(ARDUINO)/hardware/arduino/avr/libraries/SPI/src -I.
	VPATH+=$(ARDUINO)/hardware/arduino/avr/libraries/SPI/src
	VPATH+=../rfid/src
endif

VPATH+=../util
VPATH+=../rfid/src
CXXFLAGS+=-I../util
CXXFLAGS+=-I. -I../rfid/src

ifdef STM32F103
	CXXFLAGS+=-DSTM32F1 -I/home/leo/teensy/LibStm/libopencm3/include
endif

G=-g
O=-O0
E=


TARGET=522.hex
SRC+=\
	MFRC522.cpp \
	MFRC522Debug.cpp \
	522_main.cpp \
	SPI.cpp \
	uprintf.cpp 

ifndef ARDUINO
SRC += des.cpp des3.cpp rsa.cpp aes.cpp endian.cpp debug.cpp asn1.cpp
endif

include ~/teensy/util/makefile.mk

eetags:
	ctags *.cpp *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h \
		../rfid/src/*.cpp
