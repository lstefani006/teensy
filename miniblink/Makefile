
TARGET=miniblink
SRC=miniblink.cpp rtc_setup.cpp usart_setup.cpp uprintf.cpp spi_setup.cpp ring.cpp systick_setup.cpp
OBJ_DIR=.obj

VPATH+=../util
CPPFLAGS+=-I../util

CPPFLAGS+=-I.

E=
W=3
O=-Os -g
############################################

include ../LibStm/makefile.cm3
