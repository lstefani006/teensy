CXXFLAGS = -I. -DONEWIRE_CRC8_TABLE=0
CXXFLAGS+= -I$(TEENSYLIB)
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
CXXFLAGS+= -W3
CXXFLAGS+= -Os

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire

SRC=main.cpp
SRC+=SPI.cpp
SRC+=TimerThree.cpp
SRC+=t_5110.cpp
SRC+=OneWire.cpp
SRC+=gpio.cpp

TARGET=5110.hex

include /home/leo/teensy/TeensyLib/Makefile.inc
