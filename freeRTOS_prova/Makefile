W=1

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
	Prova.cpp \
	uprintf.cpp

CXXFLAGS+=-I../util
VPATH+=../util

CFLAGS+=-D__AVR_ATmega328P__ -DTIMSK=TIMSK0
CXXFLAGS+=-D__AVR_ATmega328P__


A=-DportUSE_WDTO=WDTO_15MS

VPATH+=./leo_freeRTOS/src
VAPTH+=/home/leo/teensy/leo_freeRTOS/src/portable/GCC/ATMega328_leo
VPATH+=./leo_freeRTOS/src/portable/MemMang

CXXFLAGS+=-I/home/leo/teensy/freeRTOS_prova/leo_freeRTOS/src/include
CXXFLAGS+=-I/home/leo/teensy/freeRTOS_prova/leo_freeRTOS/src/portable/GCC/ATMega328_leo
CXXFLAGS+=-I. $(A)

CFLAGS+=-I/home/leo/teensy/freeRTOS_prova/leo_freeRTOS/src/include
CFLAGS+=-I/home/leo/teensy/freeRTOS_prova/leo_freeRTOS/src/portable/GCC/ATMega328_leo
CFLAGS+=-I. $(A)

SRC+= \
	  croutine.c \
	  event_groups.c \
	  heap_3.c \
	  list.c \
	  queue.c \
	  tasks.c \
	  timers.c \
	  port.c \
	  variantHooks.cpp



ifdef TEENSY
	include ../util/teensy_makefile.mk
else ifdef ESP8266
	include ../util/esp_makefile.mk
else
	include ../util/avr_makefile.mk
endif

.obj/port.o : ./leo_freeRTOS/src/portable/GCC/ATMega328_leo/port.c
	@echo $<
	@avr-gcc -c $(CFLAGS) $< -o $@
.obj/variantHooks.o : ./leo_freeRTOS/src/portable/GCC/ATMega328_leo/variantHooks.cpp
	@echo $<
	@avr-g++ -c $(CXXFLAGS) $< -o $@
