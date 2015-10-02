HOME=/home/leo/teensy/SerialIP

CFLAGS+= -I$(HOME)
CFLAGS+= -I$(HOME)/utility
CFLAGS+= -I$(TEENSYLIB)
CFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CFLAGS+= -W3
CFLAGS+= -Os -V

CXXFLAGS+= -I$(HOME)
CXXFLAGS+= -I$(HOME)/utility
CXXFLAGS+= -I$(TEENSYLIB)
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -I$(HOME)/../ILI9341_t3-master
CXXFLAGS+= -W3
CXXFLAGS+= -Os -V


VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(HOME)/utility
VPATH+=$(HOME)/../ILI9341_t3-master

SRC+=SerialIP.cpp
SRC+=clock-arch.c
SRC+=HelloWorldServer.cpp
SRC+=psock.c
SRC+=slipdev.c
SRC+=timer.c
SRC+=uip_arp.c
SRC+=uip.c
SRC+=uip-fw.c
SRC+=uiplib.c
SRC+=uip-neighbor.c
SRC+=uip-split.c
SRC+=ILI9341_t3.cpp
SRC+=SPI.cpp
#SRC+=leo.cpp

TARGET=slip.hex

include /home/leo/teensy/TeensyLib/Makefile.inc
