W=1

ifdef TEENSY
	CXXFLAGS=-I../util -I.
	VPATH=../util
else
	CXXFLAGS=-I../util -I.
	VPATH=../util
endif

ifdef TEENSY
	VPATH     +=$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
	CXXFLAGS+=-I$(ARDUINO)/hardware/teensy/avr/libraries/TimerThree
else
	VPATH     +=$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne
	CXXFLAGS+=-I$(ARDUINO)/hardware/teensy/avr/libraries/TimerOne
endif

VPATH     +=$(ARDUINO)/hardware/teensy/avr/libraries/SPI
CXXFLAGS+=-I$(ARDUINO)/hardware/teensy/avr/libraries/SPI

TARGET=DHT11.hex
SRC+= \
	  t_5110.cpp   \
	  SPI.cpp      \
	  RTC_DS1302.cpp \
	  idDHTLib.cpp \
	  uprintf.cpp \
	  fonts.cpp    \
	  AffineMatrix.cpp \
	  Graph.cpp    \
	  DHT11.cpp   

ifdef TEENSY
	SRC+=TimerThree.cpp
else
	SRC+=TimerOne.cpp
endif

#####################################################
CXXFLAGS+= -DONEWIRE_CRC8_TABLE=0 -DDALLAS=1
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/OneWire
SRC+=OneWire.cpp
SRC+=DallasTemperature.cpp

#####################################################
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/EEPROM
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/EEPROM
SRC+=EEPROM.cpp

#####################################################
CXXFLAGS+= -I$(ARDUINO)/hardware/teensy/avr/libraries/Time
VPATH+=$(ARDUINO)/hardware/teensy/avr/libraries/Time
SRC+=Time.cpp

ifdef TEENSY
	include ../TeensyLib/teensy_makefile.mk
else
	include ../AvrLib/avr_makefile.mk
endif

tags : 
	ctags *.c *.cpp *.h *.hpp \
		$(TEENSY)/*.cpp $(TEENSY)/*.c \
		$(TEENSY)/*.hpp $(TEENSY)/*.h
