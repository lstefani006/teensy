
TARGET=522_ST
SRC=522_ST.cpp Arduino.cpp rtc_setup.cpp usart_setup.cpp uprintf.cpp spi_setup.cpp ring.cpp systick_setup.cpp MFRC522.cpp MFRC522Debug.cpp leo.c
OBJ_DIR=.obj

VPATH+=../util
VPATH+=../rfid/src
VPATH+=../LibStm/LeoStm

CPPFLAGS+=-I../rfid/src
CPPFLAGS+=-I../LibStm/LeoStm 
CPPFLAGS+=-I../miniblink 
CPPFLAGS+=-I../util

CPPFLAGS+=-I.

E=
W=3

############################################
include ../LibStm/makefile.cm3
