TARGET=532_HSU
#SRC+=iso14443a_uid.cpp
SRC+=./android_hce.cpp

SRC+=emulatetag.cpp llcp.cpp mac_link.cpp PN532.cpp snep.cpp
SRC+=PN532_HSU.cpp
SRC+=Arduino.cpp

VPATH+=../PN532/PN532
VPATH+=../PN532/PN532_HSU

CPPFLAGS+=-I. #-DDEBUG
CPPFLAGS+=-I../PN532/PN532_HSU
CPPFLAGS+=-I../PN532/PN532

############################################
E=
W=4
G=-g
O=-O0

############################################

include ../util/linux_makefile.mk
