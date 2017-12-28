TARGET     = $(BUILDDIR)/leo
LIBS       = SPI Wire
SRCDIR     = src
INCLUDEDIR = include

##############################################################
ARDUINO=/home/leo/arduino-1.8.2
ECHO?=@

##############################################################
ifeq ($(BOARD),ARDUINO_PRO)
	DD=-DF_CPU=8000000L -DARDUINO_AVR_PRO -mmcu=atmega328p
	VR=eightanaloginputs
else ifeq ($(BOARD),ARDUINO_2009)
	DD=-DF_CPU=16000000L -DARDUINO_AVR_DUEMILANOVE -mmcu=atmega328p
	VR=standard
else ifeq ($(BOARD),ARDUINO_AVR_MEGA2560)
	DD=-DF_CPU==16000000L -DARDUINO_AVR_MEGA2560 -mmcu=atmega2560
	VR=mega
else
	$(error Please specify BOARD=ARDUINO_PRO or BOARD=ARDUINO_AVR_MEGA2560)
endif
##############################################################
COREDIR=$(ARDUINO)/hardware/arduino/avr/cores/arduino
LIBDIR=$(ARDUINO)/hardware/arduino/avr/libraries

##############################################################
CC=avr-gcc
CXX=avr-g++
AR=avr-ar

BUILDDIR = build
INCDIRS  =	$(INCLUDEDIR) \
			$(ARDUINO)/hardware/arduino/avr/cores/arduino \
			$(ARDUINO)/hardware/arduino/avr/variants/$(VR) \
            $(wildcard $(foreach lib, $(LIBS), $(LIBDIR)/$(lib)/src))

CPPFLAGS= -ffunction-sections -fdata-sections -Os \
		  $(DD) \
		-DARDUINO=10608 -DARDUINO_ARCH_AVR \
		  -Wall

CXXFLAGS += $(INCDIRS:%=-I%) -std=c++1y -g -fno-threadsafe-statics -fno-exceptions
CFLAGS   += $(INCDIRS:%=-I%)  -g
LDFLAGS  += -g -lm

define find_source
	$(shell find $(1) -name '*.c' -or -name '*.cpp')
endef

OBJS       = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(call find_source, $(SRCDIR)))))
OBJS      += $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(foreach lib, $(LIBS), $(call find_source, $(LIBDIR)/$(lib)/src)))))

CORE_OBJS += $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(call find_source, $(COREDIR)))))

VPATH += $(shell find $(SRCDIR) -type d)
VPATH += $(foreach lib, $(LIBS), $(shell find $(LIBDIR)/$(lib)/src -type d))
VPATH += $(shell find $(COREDIR) -type d)

.PHONY: all clean run

all: $(TARGET).hex

# COMPILE.c		==> $(CC) $(CFLAGS) $(CPPFLAGS) -c
# LINK.c		==> $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
# COMPILE.cc	==> $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
# LINK.cc		==> $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
#
# CFLAGS		==> Options for the cc  command; none by default.
# CXXFLAGS		==> Options for the c++ command; none by default.
# CPPFLAGS		==> Options for cpp; none by default. (preprocessor)
#
$(BUILDDIR)/%.cpp.o : %.cpp | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.c.o : %.c | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.c) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.c) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR):
	$(ECHO)mkdir $(BUILDDIR)

$(TARGET).hex: $(OBJS) $(BUILDDIR)/libAvr.ar
	@echo LINK $@
	$(ECHO)$(CXX) $(CXXFLAGS) -o $(TARGET).elf -Wl,--gc-sections -mmcu=atmega328p $^ $(LDFLAGS)
	$(ECHO)avr-objdump -h -S $(TARGET).elf > $(TARGET).dis
	$(ECHO)avr-strip -g $(TARGET).elf
	$(ECHO)avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	$(ECHO)avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	$(ECHO)avr-size $(TARGET).elf
	$(ECHO)avr-size --mcu=atmega328p --format=avr $(TARGET).elf

$(BUILDDIR)/libAvr.ar : $(CORE_OBJS)
	@echo AR $@
	$(ECHO)rm -f $@
	$(ECHO)$(AR) cru $@ $^

upload : $(TARGET).hex
	$(ECHO)ArduinoSerialMonitor.exe -stop
	$(ECHO)avrdude -C$(ARDUINO)/hardware/tools/avr/etc/avrdude.conf $(VERBOSE) -patmega328p -carduino \
		-P/dev/ttyUSB0 -b57600 -D -Uflash:w:$(TARGET).hex
	$(ECHO)ArduinoSerialMonitor.exe -run

clean:
	$(ECHO)$(RM) $(BUILDDIR)/*.d $(BUILDDIR)/*.o $(TARGET).* $(BUILDDIR)/libAvr.ar

ifneq ($(MAKECMDGOALS),clean)
	-include $(BUILDDIR)/*.d
endif
