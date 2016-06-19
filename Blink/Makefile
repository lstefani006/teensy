ifdef TEENSY
	CXXFLAGS=-W2 -Os -I../TeensyLib -V
	VPATH=../TeensyLib
else  
ifdef ESP8266
	CXXFLAGS=-I../EspLib
	VPATH=../EspLib
else
	CXXFLAGS=-I../AvrLib
	VPATH=../AvrLib
endif
endif

TARGET=Blink.hex
SRC=\
	Blink.cpp
ifndef TEENSY
	SRC += t_io.cpp
endif


ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else ifdef ESP8266
	include ../EspLib/esp_makefile.mk
else
	include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
