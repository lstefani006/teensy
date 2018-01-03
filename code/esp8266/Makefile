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
SRCDIR   = src
BUILDDIR = build
TARGET   = $(BUILDDIR)/leo
LIBS     = ESP8266WiFi ESP8266WebServer


CC=xtensa-lx106-elf-gcc
CXX=xtensa-lx106-elf-g++
AR=xtensa-lx106-elf-ar

ESPBASE=/home/leo/.arduino15/packages/esp8266
H=$(ESPBASE)/hardware/esp8266/2.3.0

COREDIR=$(H)/cores/esp8266
LIBDIR=$(H)/libraries

INCDIRS  =	include \
			$(H)/cores/esp8266 \
			$(H)/tools/sdk/include \
			$(H)/variants/generic \
			$(H)/tools/sdk/lwip/include \
            $(wildcard $(foreach lib, $(LIBS), $(LIBDIR)/$(lib)/src))

#Tutti
B=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -DF_CPU=80000000L -DLWIP_OPEN_SRC -DDEBUG_ESP_PORT=Serial -DARDUINO=10805 \
  -DARDUINO_ESP8266_ESP12 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP12 -DESP8266

C=-Os -MMD -Wall -Wextra 
E=-mlongcalls -mtext-section-literals
F=-falign-functions=4 -ffunction-sections -fdata-sections

#C++
L=-std=c++11 -fno-exceptions -fno-rtti

#C
M=-std=gnu99 -Wpointer-arith -Wno-implicit-function-declaration -fno-inline-functions -nostdlib -Wl,-EL

CPPFLAGS=$B $G $C $E $F $(INCDIRS:%=-I%)

CXXFLAGS += $L
CFLAGS   += $M

define find_source
    $(shell find $(1) -name '*.c' -or -name '*.cpp' -or -name '*.S')
endef

OBJS       = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(call find_source, $(SRCDIR)))))
OBJS      += $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(foreach lib, $(LIBS), $(call find_source, $(LIBDIR)/$(lib)/src)))))
CORE_OBJS += $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(call find_source, $(COREDIR)))))

VPATH += $(shell find $(SRCDIR) -type d)
VPATH += $(shell find $(COREDIR) -type d)
VPATH += $(foreach lib, $(LIBS), $(shell find $(LIBDIR)/$(lib)/src -type d))

.PHONY: all clean upload

all : $(TARGET).bin

$(BUILDDIR)/%.cpp.o : %.cpp | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.cc) $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.c.o : %.c | $(BUILDDIR)
	@echo cc $<
	$(ECHO)$(COMPILE.c) -Wl,-EL $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.c) -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR)/%.S.o : %.S | $(BUILDDIR)
	@echo CC $<
	$(ECHO)$(COMPILE.cc) -x assembler-with-cpp $< $(OUTPUT_OPTION)
	$(ECHO)$(COMPILE.cc) -x assembler-with-cpp -MM -MP -MT $@ $< -o $(BUILDDIR)/$*.d

$(BUILDDIR):
	$(ECHO)mkdir $(BUILDDIR)

$(BUILDDIR)/libEsp.ar : $(CORE_OBJS)
	@echo AR $@
	$(ECHO)rm -f $@
	$(ECHO)$(AR) cru $@ $^

$(TARGET).bin: $(OBJS) $(BUILDDIR)/libEsp.ar
	@echo LINK $@
	$(ECHO)$(CC) -Os -nostdlib \
		-Wl,--no-check-sections -u call_user_start -Wl,-static \
		-L$H/tools/sdk/lib -L$H/tools/sdk/ld \
		-Teagle.flash.512k64.ld -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,register_chipv6_phy  \
		-o $(TARGET).elf -Wl,--start-group \
		$(OBJS) $(BUILDDIR)/libEsp.ar \
		-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 -llwip_gcc -lstdc++ -Wl,--end-group
	$(ECHO)$(ESPBASE)/tools/esptool/0.4.9/esptool \
		-eo $(ESPBASE)/hardware/esp8266/2.3.0/bootloaders/eboot/eboot.elf \
		-bo $(TARGET).bin -bm dio -bf 40 -bz 4M -bs .text -bp 4096 -ec \
		-eo $(TARGET).elf -bs .irom0.text -bs .text -bs .data -bs .rodata -bc -ec

upload : $(TARGET).bin
	@echo UPLOAD $<
	$(ECHO)ArduinoSerialMonitor.exe -stop
	$(ECHO)$(ESPBASE)/tools/esptool/0.4.9/esptool -vv -cd ck -cb 115200 -cp /dev/ttyUSB0 -ca 0x00000 -cf $(TARGET).bin
	$(ECHO)ArduinoSerialMonitor.exe -run

clean:
	$(ECHO)$(RM) $(BUILDDIR)/*.d $(BUILDDIR)/*.o $(TARGET).* $(BUILDDIR)/libEsp.ar

ifneq ($(MAKECMDGOALS),clean)
	-include $(BUILDDIR)/*.d
endif
