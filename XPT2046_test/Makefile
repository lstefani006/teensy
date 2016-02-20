ifdef TEENSY
CXXFLAGS=-W2 -Os -I../TeensyLib -V
VPATH=../TeensyLib
else
CXXFLAGS=-I../AvrLib -I.  -I/home/leo/arduino-1.6.7/hardware/arduino/avr/libraries/SPI
VPATH=../AvrLib:/home/leo/arduino-1.6.7/hardware/arduino/avr/libraries/SPI
endif


ifdef TEENSY
else
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_ILI9341
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Adafruit_GFX
endif

TARGET=TouchTest.hex
SRC=\
	TouchTest.cpp \
	t_io.cpp \
	SPI.cpp \
	XPT2046.cpp \
	XPT2046_Touchscreen.cpp \
	calibrate.cpp

SRC+=Adafruit_ILI9341.cpp Adafruit_GFX.cpp

ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else
	include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
