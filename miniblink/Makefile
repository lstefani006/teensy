TARGET=miniblink
SRC=miniblink.cpp 
SRC+=rtc_setup.cpp usart_setup.cpp uprintf.cpp spi_setup.cpp ring.cpp systick_setup.cpp Timer.cpp Arduino.cpp
OBJ_DIR=.obj

VPATH+=../util
VPATH+=../LibStm/LeoStm

CPPFLAGS+=-I../util
CPPFLAGS+=-I../LibStm/LeoStm

CPPFLAGS+=-I.

E=@
W=4
O=-Os
G=-g
############################################

include ../LibStm/makefile.cm3
