V=1

ifeq ($(PLATFORM),STM32F103)
	CXXFLAGS=-I../StLib -I.
	VPATH=../StLib
else ifeq ($(PLATFORM),TEENSY)
	CXXFLAGS=-I../TeensyLib 
	VPATH=../TeensyLib
else ifeq ($(PLATFORM),ESP8266)
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else ifeq ($(PLATFORM),AVR)
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
else
	error
endif

VPATH+=../util
CXXFLAGS+=-I../util
TARGET=st_temp.hex
SRC+=\
	st_temp.cpp \
	DallasTemperature.cpp \
	OneWire.cpp \
	SPI.cpp \
	uprintf.cpp \
	AffineMatrix.cpp \
	Graph.cpp \
	fonts.cpp

#Font16.c \
#Font32.c \
#Font64.c \
#Font7s.c \
#glcdfont.c
#Adafruit_ILI9341_STM.cpp \
#Adafruit_GFX_AS.cpp

include ../util/makefile.mk
