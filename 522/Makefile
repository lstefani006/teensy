CXXFLAGS = -I. -V
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI

ifdef TEENSY
CXXFLAGS+= -W3 -Os
endif
CXXFLAGS+= -Os

TARGET=522.hex
SRC=\
	MFRC522.cpp \
	522.cpp \
	SPI.cpp

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	hdrtag *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
