CXXFLAGS = -I.
CXXFLAGS+= -I$(TEENSYLIB)
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -W3
CXXFLAGS+= -Os -V

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI

SRC=main.cpp
#SRC+=ILI9341_t3.cpp
SRC+=Adafruit_ILI9341.cpp
SRC+=Adafruit_GFX.cpp
SRC+=SPI.cpp

TARGET=9341.hex

include /home/leo/teensy/TeensyLib/Makefile.inc
