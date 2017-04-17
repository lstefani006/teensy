CXXFLAGS = -I.
CXXFLAGS+= -I$(TEENSYLIB)

ifdef TEENSY
CXXFLAGS+= -W3 -Os
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I. -I../util
VPATH+=../util
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=../ILI9341_t3-master
CXXFLAGS+= -I../ILI9341_t3-master
else
CXXFLAGS+= -Os
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I../util -I.
VPATH+=../AvrLib
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
endif


SRC=gmain.cpp
SRC+=SPI.cpp DallasTemperature.cpp OneWire.cpp
ifdef TEENSY
SRC+=ILI9341_t3.cpp
else
SRC+=Adafruit_ILI9341.cpp
SRC+=Adafruit_GFX.cpp
endif

TARGET=9341.hex

#ifdef TEENSY
#include ../TeensyLib/teensy_makefile.mk
#else
#include ../AvrLib/avr_makefile.mk
#endif
include ../util/makefile.mk
