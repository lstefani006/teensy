ifdef TEENSY
CXXFLAGS=-W2 -Os -I../TeensyLib -V
VPATH=../TeensyLib
else
CXXFLAGS=-I../AvrLib
VPATH=../AvrLib
endif

#SPI x touch e display
CXXFLAGS+=-I$(ARDUINO)/hardware/arduino/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/arduino/avr/libraries/SPI

# i2c per Radio
VPATH+=$(ARDUINO)/hardware/arduino/avr/libraries/Wire
VPATH+=$(ARDUINO)/hardware/arduino/avr/libraries/Wire/utility
CXXFLAGS+=-DI2C_LIB
CXXFLAGS+=-I$(ARDUINO)/hardware/arduino/avr/libraries/Wire
CXXFLAGS+=-I$(ARDUINO)/hardware/arduino/avr/libraries/Wire/utility

# RDA5807M e RDA
#VPATH+=./RDA5807M
#CXXFLAGS+=-I./RDA5807M
#VPATH+=./RDSDecoder
#CXXFLAGS+=-I./RDSDecoder

# Grafica
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX

TARGET=LeoRadio.hex
SRC=\
	\
	LeoRadio.cpp \
	t_io.cpp \
	\
	RDA5807M.cpp \
	RDSParser.cpp \
	\
	Wire.cpp \
	twi.c \
	\
	SPI.cpp \
	XPT2046_Touchscreen.cpp \
	calibrate.cpp \
	Adafruit_ILI9341.cpp \
	Adafruit_GFX.cpp \
	I2C.cpp


ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif
