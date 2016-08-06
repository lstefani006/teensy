W?=4

ifeq ($(shell test $(W) -ge 1; echo $$?),0)
	WCOMMON=-Wreturn-type
	WC+=$(WCOMMON)
	WCXX+=$(WCOMMON)
endif

ifeq ($(shell test $(W) -ge 2; echo $$?),0)
	WCOMMON= -Wall \
			 -Wdouble-promotion \
			 -Wformat \
			 -Winit-self \
			 -Wmissing-include-dirs \
			 -Wparentheses \
			 -Wswitch-enum \
			 -Wuninitialized \
			 -Wmaybe-uninitialized \
			 -Wstrict-overflow \
			 -Wshadow \
			 -Wcast-qual \
			 -Wwrite-strings \
			 -Wconversion \
			 -Wlogical-op \
			 -Waggregate-return \
			 -Wwrite-strings \
			 -Wcast-align \
			 -Wcast-qual \
			 -Wpointer-arith \
			 -Wstrict-aliasing \
			 -Wformat \
			 -Wmissing-include-dirs \
			 -Wno-unused-parameter \
			 -Wuninitialized 
	WC+= $(WCOMMON) \
		 -Wstrict-prototypes 
	WCXX+= $(WCOMMON) \
		 -Wuseless-cast \
		 -Wzero-as-null-pointer-constant
endif

ifeq ($(shell test $(W) -ge 3; echo $$?),0)
	WCOMMON= -Wextra \
			 -Wempty-body
	WC+=$(WCOMMON)
	WCXX+=$(WCOMMON)
endif

ifeq ($(shell test $(W) -ge 4; echo $$?),0)
	WCOMMON= -Wpedantic
	WC+=$(WCOMMON)
	WCXX+=$(WCOMMON)
endif

##################################################
CCOMMON= \
	-MMD \
	-Os \
	-ffunction-sections -fdata-sections \
	-mmcu=atmega328p -DF_CPU=8000000L \
	-DARDUINO=10608 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR \
	-I$(ARDUINO)/hardware/arduino/avr/cores/arduino \
	-I$(ARDUINO)/hardware/arduino/avr/variants/eightanaloginputs

CFLAGS+=$(CCOMMON) $(WC)
CXXFLAGS+=$(CCOMMON) $(WCXX) -std=gnu++11 -fno-threadsafe-statics \
	-fno-exceptions

.obj/%.o : %.cpp | .obj
	@echo $<
	avr-g++ -c $(CXXFLAGS) $< -o $@

.obj/%.o : %.c | .obj
	@echo $<
	avr-gcc -c $(CFLAGS) $< -o $@

.lib/%.o : %.cpp | .lib
	@echo $<
	avr-g++ -c $(CXXFLAGS) $< -o $@

.lib/%.o : %.c | .lib
	@echo $<
	avr-gcc -c $(CFLAGS) $< -o $@

%.s : %.cpp
	@echo $<
	avr-g++ -g -Wa,-adhls -c $(CXXFLAGS) $< > $@

%.s : %.c
	@echo $<
	avr-gcc -S -c $(CFLAGS) $< -o $@

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
	avr-g++ -o$(TARGET).elf -Wl,--gc-sections -mmcu=atmega328p $(OBJ) libArduinoPro.a 
	avr-strip -g $(TARGET).elf
	avr-objdump -S -D $(TARGET).elf > $(TARGET).dis
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	avr-size $(TARGET).elf
	avr-size --mcu=atmega328 --format=avr $(TARGET).elf

libArduinoPro.a : $(LIB_OBJ)
	rm -f $@
	avr-ar -cvq $@ $^

.lib:
	mkdir .lib

.obj:
	mkdir .obj

clean:
	-rm -f libArduinoPro.a
	-rm -rf .obj
	-rm -rf .lib
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).dis

upload : $(TARGET).hex
	ArduinoSerialMonitor.exe -stop
	avrdude -C$(ARDUINO)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino \
		-P/dev/ttyUSB0 -b57600 -D -Uflash:w:$(TARGET).hex
	ArduinoSerialMonitor.exe -run

-include $(OBJ:.o=.d)
