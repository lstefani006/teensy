CXXFLAGS = -I.
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+= -Os
VPATH+=./crypto

ifdef TEENSY
VPATH   +=../TeensyLib
CXXFLAGS+= -DUSE_NO_RTOS
CXXFLAGS+= -W3 -Os
CXXFLAGS+=-I../TeensyLib -V
endif

TARGET=522.hex
SRC=\
	MFRC522.cpp \
	main.cpp \
	SPI.cpp \
	des.cpp des3.cpp rsa.cpp aes.cpp endian.cpp debug.cpp asn1.cpp

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	hdrtag *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
