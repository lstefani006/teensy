W=2
ifdef TEENSY
	CXXFLAGS=-W2 -Os -I../TeensyLib -V -I.
	VPATH=../TeensyLib
else
	CXXFLAGS=-I../AvrLib -I.
	VPATH=../AvrLib
	SRC += t_io.cpp
endif

VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne

CXXFLAGS+=-I$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+=-I$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne

TARGET=DHT11.hex
SRC+=             \
	DHT11.cpp    \
	idDHTLib.cpp \
	t_5110.cpp   \
	SPI.cpp      \
	TimerOne.cpp \
	fonts.cpp    \
	Graph.cpp    \
	AffineMatrix.cpp \
	uprintf.cpp


#####################################################
CXXFLAGS+= -DONEWIRE_CRC8_TABLE=0 -DDALLAS=1
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
SRC+=OneWire.cpp
SRC+=DallasTemperature.cpp


ifdef TEENSY
include ../TeensyLib/teensy_makefile.mk
else
include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
