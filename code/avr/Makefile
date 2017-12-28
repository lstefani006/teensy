# COMPILE.c		==> $(CC) $(CFLAGS) $(CPPFLAGS) -c
# LINK.c		==> $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
# COMPILE.cc	==> $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
# LINK.cc		==> $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
#
# CFLAGS		==> Options for the cc  command; none by default.
# CXXFLAGS		==> Options for the c++ command; none by default.
# CPPFLAGS		==> Options for cpp; none by default. (preprocessor)
#
ECHO?=@

CC=avr-gcc
CXX=avr-g++
ARDUINO=/home/leo/arduino-1.8.2
LIBDIR=$(ARDUINO)/hardware/arduino/avr/cores/arduino

TARGET   = $(BUILDDIR)/leo
LIBS     = SPI
SRCDIR   = src
BUILDDIR = build
INCDIRS  =	include \
			$(ARDUINO)/hardware/arduino/avr/cores/arduino \
			$(ARDUINO)/hardware/arduino/avr/variants/eightanaloginputs \
            $(wildcard $(foreach lib, $(LIBS), $(ARDUINO)/hardware/arduino/avr/libraries/$(lib)/src))

CPPFLAGS= -ffunction-sections -fdata-sections \
          -Os -mmcu=atmega328p -DF_CPU=8000000L \
		  -DARDUINO=10608 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR \
		  -Wall

CXXFLAGS += $(INCDIRS:%=-I%) -std=c++1y -g -fno-threadsafe-statics -fno-exceptions
CFLAGS   += $(INCDIRS:%=-I%)  -g
LDFLAGS  += -g -lm

OBJS     = $(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.o, $(notdir $(wildcard $(SRCDIR)/*.cpp))))
OBJS    += $(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.o, $(notdir $(wildcard $(LIBDIR)/*.cpp))))
OBJS    += $(addprefix $(BUILDDIR)/, $(patsubst %.c, %.o,   $(notdir $(wildcard $(LIBDIR)/*.c))))
OBJS    += $(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.o, $(notdir $(wildcard $(foreach lib, $(LIBS), $(ARDUINO)/hardware/arduino/avr/libraries/$(lib)/src/*.cpp)))))

VPATH += $(SRCDIR)
VPATH += $(foreach lib, $(LIBS), $(ARDUINO)/hardware/arduino/avr/libraries/$(lib)/src)
VPATH += $(LIBDIR)

.PHONY: all clean run

all: $(TARGET).elf

$(BUILDDIR)/%.o : %.cpp | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.o : %.c | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.c) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.c) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR):
	$(ECHO)mkdir $(BUILDDIR)

$(TARGET).elf: $(OBJS)
	@echo LINK $<
	$(ECHO)$(CXX) $(CXXFLAGS) -o $(TARGET).elf -Wl,--gc-sections -mmcu=atmega328p $^ $(LDFLAGS)
	$(ECHO)avr-objdump -h -S $(TARGET).elf > $(TARGET).dis
	$(ECHO)avr-strip -g $(TARGET).elf
	$(ECHO)avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	$(ECHO)avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	$(ECHO)avr-size $(TARGET).elf
	$(ECHO)avr-size --mcu=atmega328p --format=avr $(TARGET).elf

run:
	$(TARGET)

clean:
	$(ECHO)$(RM) $(BUILDDIR)/*.d $(BUILDDIR)/*.o $(TARGET).*

ifneq ($(MAKECMDGOALS),clean)
	-include $(BUILDDIR)/*.d
endif
