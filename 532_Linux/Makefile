TARGET=532_HSU
OBJ_DIR=.obj
#SRC+=iso14443a_uid.cpp
SRC+=./android_hce.cpp

SRC+=emulatetag.cpp llcp.cpp mac_link.cpp PN532.cpp snep.cpp
SRC+=PN532_HSU.cpp
SRC+=Arduino.cpp

VPATH+=../PN532/PN532
VPATH+=../PN532/PN532_HSU

CPPFLAGS+=-I. #-DDEBUG
CPPFLAGS+=-I../PN532/PN532_HSU
CPPFLAGS+=-I../PN532/PN532

E=
W=3

############################################
E?=@
O?=-O0 -ggdb3
OBJ_DIR?=.obj
LIBOPENCM3_LIB?=os
############################################


CPPFLAGS+=$(O)
CPPFLAGS+=-MD -Wundef -Wall -Wextra
CXXFLAGS+=-std=c++17 

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(E)g++ $(CPPFLAGS) $(OBJ) -o $(TARGET)

$(OBJ_DIR)/%.o : %.cpp
	@echo $<
	@mkdir -p $(@D)
	$(E)g++ -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o : %.c
	@echo $<
	@mkdir -p $(@D)
	$(E)gcc -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	-rm -f $(OBJ_DIR)/*.o
	-rm -f $(OBJ_DIR)/*.d


-include $(OBJ:.o=.d)
