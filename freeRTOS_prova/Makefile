V=1

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../TeensyLib
else  ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif

TARGET=freeRTOS_prova.hex
SRC+=\
	Prova.cpp


VPATH+=/home/leo/Arduino/libraries/FreeRTOS/src
CXXFLAGS+=-I/home/leo/Arduino/libraries/FreeRTOS/src
SRC+= \
	  croutine.c \
	  event_groups.c \
	  heap_3.c \
	  list.c \
	  port.c \
	  queue.c \
	  tasks.c \
	  timers.c \
	  variantHooks.cpp




ifdef TEENSY
	include ../util/teensy_makefile.mk
else ifdef ESP8266
	include ../util/esp_makefile.mk
else
	include ../util/avr_makefile.mk
endif
