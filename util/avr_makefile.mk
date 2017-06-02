include ../util/base_makefile.mk

##################################################
CCOMMON= \
		 -g \
		 -Os \
		 -MMD \
		 -ffunction-sections -fdata-sections \
		 -mmcu=atmega328p -DF_CPU=8000000L \
		 -DARDUINO=10608 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR \
		 -I$(ARDUINO)/hardware/arduino/avr/cores/arduino \
		 -I$(ARDUINO)/hardware/arduino/avr/variants/eightanaloginputs

CFLAGS+=$(CCOMMON) $(WC)
CXXFLAGS+=$(CCOMMON) $(WCXX) -std=gnu++14 -fno-threadsafe-statics \
		  -fno-exceptions


ifndef ECHO
	ECHO=@
endif



.obj/%.o : %.cpp | .obj
	@echo $<
	$(ECHO)avr-g++ -c $(CXXFLAGS) $< -o $@

.obj/%.o : %.c | .obj
	@echo $<
	$(ECHO)avr-gcc -c $(CFLAGS) $< -o $@

.lib/%.o : %.cpp | .lib
	@echo $<
	$(ECHO)avr-g++ -c $(CXXFLAGS) $< -o $@

.lib/%.o : %.c | .lib
	@echo $<
	$(ECHO)avr-gcc -c $(CFLAGS) $< -o $@

%.s : %.cpp
	@echo $<
	$(ECHO)avr-g++ -g -Wa,-adhls -c $(CXXFLAGS) $< > $@

%.s : %.c
	@echo $<
	$(ECHO)avr-gcc -S -c $(CFLAGS) $< -o $@

VPATH+=$(ARDUINO)/hardware/arduino/avr/cores/arduino

LIB_SRC =$(wildcard $(ARDUINO)/hardware/arduino/avr/cores/arduino/*.c)
LIB_SRC+=$(wildcard $(ARDUINO)/hardware/arduino/avr/cores/arduino/*.cpp)
LIB_SRC:=$(notdir $(LIB_SRC))

LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_SRC))
LIB_OBJ:=$(patsubst %.c,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.o,.lib/%.o,$(LIB_OBJ))

##############################

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,.obj/%.o,$(notdir $(OBJ)))

TARGET:=$(TARGET:.hex=)

.PHONY: clean upload

all : $(TARGET).hex

$(TARGET).hex : libArduinoPro.a $(OBJ)
	$(ECHO)avr-g++ -o$(TARGET).elf -Wl,--gc-sections -mmcu=atmega328p $(OBJ) libArduinoPro.a 
	$(ECHO)avr-objdump -h -S $(TARGET).elf > $(TARGET).dis
	$(ECHO)avr-strip -g $(TARGET).elf
	$(ECHO)avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	$(ECHO)avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	$(ECHO)avr-size $(TARGET).elf
	$(ECHO)avr-size --mcu=atmega328p --format=avr $(TARGET).elf

libArduinoPro.a : $(LIB_OBJ)
	@rm -f $@
	@avr-ar -cvq $@ $^

.lib:
	@mkdir .lib

.obj:
	@mkdir .obj

clean:
	-rm -f libArduinoPro.a
	-rm -rf .obj
	-rm -rf .lib
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).dis

#VERBOSE=-v
VERBOSE=
upload : $(TARGET).hex
	$(ECHO)ArduinoSerialMonitor.exe -stop
	$(ECHO)avrdude -C$(ARDUINO)/hardware/tools/avr/etc/avrdude.conf $(VERBOSE) -patmega328p -carduino \
		-P/dev/ttyUSB0 -b57600 -D -Uflash:w:$(TARGET).hex
	$(ECHO)ArduinoSerialMonitor.exe -run

-include $(OBJ:.o=.d)
