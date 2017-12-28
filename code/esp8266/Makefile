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

CC=xtensa-lx106-elf-gcc
CXX=xtensa-lx106-elf-g++

H=/home/leo/.arduino15/packages/esp8266/hardware/esp8266/2.3.0

# ARDUINO=/home/leo/arduino-1.8.2
LIBDIR=$(H)/cores/esp8266

TARGET   = $(BUILDDIR)/leo
LIBS     = ESP8266WiFi
SRCDIR   = src
BUILDDIR = build
INCDIRS  =	include \
			$(H)/cores/esp8266 \
			$(H)/tools/sdk/include \
			$(H)/variants/generic \
			$(H)/tools/sdk/lwip/include \
            $(wildcard $(foreach lib, $(LIBS), $(H)/libraries/$(lib)/src))

#Tutti
B=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -DF_CPU=80000000L -DLWIP_OPEN_SRC -DDEBUG_ESP_PORT=Serial -DARDUINO=10805 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP01 -DESP8266
#G=-I$H/tools/sdk/include -I$H/tools/sdk/lwip/include -Iinclude -I$H/cores/esp8266 -I$H/variants/generic
C=-Wall -Wextra -Os -g -MMD
E=-mlongcalls -mtext-section-literals
F=-falign-functions=4 -ffunction-sections -fdata-sections

#C++
L=-std=c++11 -fno-exceptions -fno-rtti

#C
M=-std=gnu99 -Wpointer-arith -Wno-implicit-function-declaration -fno-inline-functions -nostdlib -Wl,-EL

CPPFLAGS=$B $G $C $E $F $(INCDIRS:%=-I%)

CXXFLAGS += $L
CFLAGS   += $M
#LDFLAGS  += -g -lm

OBJS     = $(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.cpp.o, $(notdir $(wildcard $(SRCDIR)/*.cpp))))
OBJS    += $(addprefix $(BUILDDIR)/, $(patsubst %.cpp, %.cpp.o, $(notdir $(wildcard $(foreach lib, $(LIBS), $(H)/libraries/$(lib)/src/*.cpp)))))

A=$(notdir $(shell find $(LIBDIR) -name '*.c' -or -name '*.cpp' -or -name '*.S'))
CORE_OBJS    += $(addprefix $(BUILDDIR)/, $(patsubst %.S, %.S.o, $(patsubst %.c, %.c.o, $(patsubst %.cpp, %.cpp.o, $(A)))))

VPATH += $(shell find $(LIBDIR) -type d)
VPATH += $(foreach lib, $(LIBS), $(H)/libraries/$(lib)/src)
VPATH += $(SRCDIR)

.PHONY: all clean run

#lsslsl:
#	@echo
#	@echo $(A)
#	@echo
#	@echo $(OBJS)
#	@echo
#	@echo $(VPATH)

all: $(TARGET).elf

$(BUILDDIR)/%.cpp.o : %.cpp | $(BUILDDIR)
	@echo g++ $<
	$(ECHO)$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.c.o : %.c | $(BUILDDIR)
	@echo gcc $<
	$(ECHO)$(COMPILE.c) -Wl,-EL $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.c) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.S.o : %.S | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.cc) -x assembler-with-cpp $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -x assembler-with-cpp -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR):
	$(ECHO)mkdir $(BUILDDIR)

$(BUILDDIR)/libEsp.ar : $(CORE_OBJS)
	rm -f $@
	xtensa-lx106-elf-ar cru $@ $^


#		"/tmp/arduino_build_217266/sketch/Blink.ino.cpp.o" "/tmp/arduino_build_217266/arduino.ar" 
#
$(TARGET).elf: $(OBJS) $(BUILDDIR)/libEsp.ar
	@echo LINK $<
	xtensa-lx106-elf-gcc -g -Wall -Wextra -Os -nostdlib \
		-Wl,--no-check-sections -u call_user_start -Wl,-static "-L$H/tools/sdk/lib" "-L$H/tools/sdk/ld" \
		"-Teagle.flash.512k64.ld" -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,register_chipv6_phy  \
		-o $(TARGET).elf -Wl,--start-group \
		$(OBJS) $(BUILDDIR)/libEsp.ar\
		-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 -llwip_gcc -lstdc++ -Wl,--end-group

#	xtensa-lx106-elf-gcc -g -w -Os -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static  \
#		"-L$(H)/tools/sdk/lib" \
#		"-Teagle.flash.4m.ld" \
#		-Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,register_chipv6_phy  \
#		-o $(TARGET).elf \
#		-Wl,--start-group $(OBJS) \
#		-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 -llwip_gcc -lstdc++ -Wl,--end-group -L.

run:
	$(TARGET)

clean:
	$(ECHO)$(RM) $(BUILDDIR)/*.d $(BUILDDIR)/*.o $(TARGET).* $(BUILDDIR)/libEsp.ar

ifneq ($(MAKECMDGOALS),clean)
	-include $(BUILDDIR)/*.d
endif
