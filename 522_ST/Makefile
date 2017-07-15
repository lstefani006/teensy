
TARGET=522_ST
SRC=522_ST.cpp MFRC522.cpp MFRC522Debug.cpp
OBJ_DIR=.obj

VPATH+=../util
VPATH+=../rfid/src
VPATH+=../LibStm/LeoStm

CPPFLAGS+=-I../rfid/src
CPPFLAGS+=-I../LibStm/LeoStm 
CPPFLAGS+=-I../miniblink 
CPPFLAGS+=-I../util

CPPFLAGS+=-I.

E=
W=3

############################################
include ../LibStm/LeoStm/Makefile
include ../LibStm/makefile.cm3
