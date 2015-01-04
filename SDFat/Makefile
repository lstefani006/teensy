CXXFLAGS += -I SdFat-master/SdFat
CXXFLAGS += -I $(ARDUINO)/libraries/SPI
CXXFLAGS += -W1 -MMD -Os

VPATH    += $(ARDUINO)/libraries/SPI
VPATH    += SdFat-master/SdFat

TARGET=SdFatTest.hex

SRC = \
	  main.cpp \
	  istream.cpp \
	  MinimumSerial.cpp \
	  ostream.cpp \
	  Sd2Card.cpp \
	  SdBaseFile.cpp \
	  SdBaseFilePrint.cpp \
	  SdFat.cpp \
	  SdFatErrorPrint.cpp \
	  SdFatUtil.cpp \
	  SdFile.cpp \
	  SdSpiArduino.cpp \
	  SdSpiAVR.cpp \
	  SdSpiSAM3X.cpp \
	  SdSpiSoft.cpp \
	  SdSpiTeensy3.cpp \
	  SdStream.cpp \
	  SdVolume.cpp \
	  StdioStream.cpp

include ../TeensyLib/Makefile.inc
