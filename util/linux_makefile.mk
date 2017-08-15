############################################
E?=@
O?=-O0 -ggdb3
G?=-g
OBJ_DIR?=.obj
############################################

include ../util/base_makefile.mk

CPPFLAGS+=$(O) $(G)
CPPFLAGS+=-MD
CXXFLAGS+=-std=c++17 

CFLAGS+=$(WC)
CXXFLAGS+=$(WCXX)

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(E)g++ $(CXXFLAGS) $(CPPFLAGS) $(OBJ) -o $(TARGET)

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
	-rm -f $(TARGET)
	-rm -f $(OBJ_DIR)/*.o
	-rm -f $(OBJ_DIR)/*.d
	-rm -rf $(OBJ_DIR)


-include $(OBJ:.o=.d)
