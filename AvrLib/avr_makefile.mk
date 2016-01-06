CFLAGS+=\
	-MMD \
	-Os -Wall -Wextra  \
	-ffunction-sections -fdata-sections \
	-mmcu=atmega328p -DF_CPU=8000000L \
	-DARDUINO=10607 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR \
	-I$(ARDUINO)/hardware/arduino/avr/cores/arduino \
	-I$(ARDUINO)/hardware/arduino/avr/variants/eightanaloginputs

CXXFLAGS+=$(CFLAGS) -std=gnu++11 -fno-threadsafe-statics \
	-fno-exceptions

obj/%.o : %.cpp | obj
	avr-g++ -c $(CXXFLAGS) $< -o $@

obj/%.o : %.c | obj
	avr-gcc -c $(CFLAGS) $< -o $@

%.s : %.cpp
	avr-g++ -g -Wa,-adhls -c $(CXXFLAGS) $< > $@

%.s : %.c
	avr-gcc -S -c $(CFLAGS) $< -o $@

VPATH+=$(ARDUINO)/hardware/arduino/avr/cores/arduino

LIB_SRC =$(wildcard $(ARDUINO)/hardware/arduino/avr/cores/arduino/*.c)
LIB_SRC+=$(wildcard $(ARDUINO)/hardware/arduino/avr/cores/arduino/*.cpp)
LIB_SRC:=$(notdir $(LIB_SRC))

LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_SRC))
LIB_OBJ:=$(patsubst %.c,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.o,obj/%.o,$(LIB_OBJ))

##############################

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,obj/%.o,$(notdir $(OBJ)))

TARGET:=$(TARGET:.hex=)

.PHONY: clean upload

all : $(TARGET).hex

$(TARGET).hex : libArduinoPro.a $(OBJ)
	avr-gcc -o$(TARGET).elf -Wl,--gc-sections -mmcu=atmega328p $(OBJ) libArduinoPro.a 
	avr-strip -g $(TARGET).elf
	avr-objdump -S -D $(TARGET).elf > $(TARGET).dis
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	avr-size $(TARGET).elf

libArduinoPro.a : $(LIB_OBJ)
	rm -f $@
	avr-ar -cvq $@ $^

obj:
	mkdir obj

clean:
	-rm -f libArduinoPro.a
	-rm -rf obj
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).dis

upload : $(TARGET).hex
	avrdude -C$(ARDUINO)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino \
		-P/dev/ttyUSB1 -b57600 -D -Uflash:w:$(TARGET).hex

-include $(OBJ:.o=.d)
