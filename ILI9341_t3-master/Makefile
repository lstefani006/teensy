CXXFLAGS = -I.
CXXFLAGS+= -I$(TEENSYLIB)

ifdef TEENSY
CXXFLAGS+= -W3 -Os
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I. -I../TeensyLib -V
VPATH+=../TeensyLib
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
else
CXXFLAGS+= -Os
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
CXXFLAGS+= -I$(ARDUINO)/hardware/arduino/avr/libraries/SPI
CXXFLAGS+= -I../AvrLib -I.
VPATH+=../AvrLib
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
endif


SRC=gmain.cpp
SRC+=SPI.cpp
SRC+=ILI9341_t3.cpp

TARGET=9341.hex

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif
