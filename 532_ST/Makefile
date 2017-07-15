
TARGET=532_SP
OBJ_DIR=.obj
SRC+=Arduino.cpp rtc_setup.cpp usart_setup.cpp uprintf.cpp spi_setup.cpp ring.cpp systick_setup.cpp
SRC+=./iso14443a_uid.cpp

SRC+=emulatetag.cpp llcp.cpp mac_link.cpp PN532.cpp snep.cpp
SRC+=PN532_SPI.cpp

VPATH+=../miniblink
VPATH+=../util
VPATH+=../rfid/src
VPATH+=../PN532/PN532/PN532
VPATH+=../PN532/PN532/PN532_SPI


CPPFLAGS+=-I../miniblink 
CPPFLAGS+=-I../util
CPPFLAGS+=-I../rfid/src 

CPPFLAGS+=-I.
CPPFLAGS+=-I../PN532/PN532/PN532_SPI
CPPFLAGS+=-I../PN532/PN532/PN532

E=
W=3

############################################
include ../LibStm/makefile.cm3
