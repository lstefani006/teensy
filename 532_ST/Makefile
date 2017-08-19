TARGET=532_SPI
OBJ_DIR=.obj
SRC+=iso14443a_uid.cpp

SRC+=emulatetag.cpp llcp.cpp mac_link.cpp PN532.cpp snep.cpp
SRC+=PN532_SPI.cpp

VPATH+=../PN532/PN532
VPATH+=../PN532/PN532_SPI

CPPFLAGS+=-I.
CPPFLAGS+=-I../PN532/PN532_SPI
CPPFLAGS+=-I../PN532/PN532

TAGS+=$(HOME)/teensy/PN532/PN532_SPI
TAGS+=$(HOME)/teensy/PN532/PN532

E=@
W=3

include ../LibStm/LeoStm/Makefile

############################################
include ../LibStm/makefile.cm3
