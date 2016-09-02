CXXFLAGS= -I$(ARDUINO)/hardware/teensy/avr/libraries/SPI -I.
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=./crypto

ifdef TEENSY
	VPATH   +=../util
	CXXFLAGS+= -DUSE_NO_RTOS
	CXXFLAGS+=-I../util
else ifdef ESP8266
else
	CXXFLAGS+= -DUSE_NO_RTOS
	VPATH   +=../util
	CXXFLAGS+=-I../util
endif

TARGET=522.hex
SRC+=\
	MFRC522.cpp \
	522_main.cpp \
	SPI.cpp \
	des.cpp des3.cpp rsa.cpp aes.cpp endian.cpp debug.cpp asn1.cpp \
	uprintf.cpp

ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	hdrtag *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
