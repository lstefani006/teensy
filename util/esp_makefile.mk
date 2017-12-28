include ../util/base_makefile.mk

L=$(HOME)/.arduino15/packages/esp8266/hardware/esp8266/2.3.0


PREFIX=xtensa-lx106-elf-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++


COMMON=
#COMMON=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ \
#	-mlongcalls \
#	-DF_CPU=80000000L -DLWIP_OPEN_SRC   -DARDUINO=10608 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD="ESP8266_ESP01"  -DESP8266 \
#	-MMD \
#	-I$L/cores/esp8266 -I$L/variants/generic -I$L/tools/sdk/include -I$L/tools/sdk/lwip/include \
#	-w -Os -g \
#	-mtext-section-literals \
#	-ffunction-sections -fdata-sections -falign-functions=4 

##################################################

LIB_ASFLAGS= \
			 -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ \
			 -I$L/tools/sdk/include -I$L/tools/sdk/lwip/include \
			 -g -x assembler-with-cpp  \
			 -mlongcalls -DF_CPU=80000000L -DLWIP_OPEN_SRC   -DARDUINO=10608 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP01  -DESP8266 \
			 -I$L/cores/esp8266 -I$L/variants/generic



LIB_CFLAGS= \
			-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ \
			-I$L/tools/sdk/include -I$L/tools/sdk/lwip/include \
			-w -Os -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL \
			-fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -falign-functions=4  \
			-std=gnu99 -ffunction-sections -fdata-sections \
			-DF_CPU=80000000L -DLWIP_OPEN_SRC   -DARDUINO=10608 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP01  -DESP8266 \
			-I$L/cores/esp8266 -I$L/variants/generic

LIB_CXXFLAGS= \
			  -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ \
			  -I$L/tools/sdk/include -I$L/tools/sdk/lwip/include -w -Os -g    \
			  -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11  -ffunction-sections -fdata-sections \
			  -DF_CPU=80000000L -DLWIP_OPEN_SRC   -DARDUINO=10608 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP01  -DESP8266 -I$L/cores/esp8266 -I$L/variants/generic 

CXXFLAGS+= $(COMMON) \
		   -DESP8266 -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -I$L/tools/sdk/include -I$L/tools/sdk/lwip/include \
		   -Icore -w -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11  \
		   -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC   -DARDUINO=10608 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=ESP8266_ESP01  -DESP8266 \
			-I$L/cores/esp8266 -I$L/variants/generic



########################################################
# sorgenti programma da compilare
#
.esp_obj/%.o : %.cpp | .esp_obj
	@echo leo $<
	@$(CXX) -c $(CXXFLAGS) $< -o $@

.esp_obj/%.o : %.c | .esp_obj
	@echo $<
	@$(CC) -c $(CXXFLAGS) $< -o $@


########################################################
# libreria
#
.esp_lib/%.o : %.cpp | .esp_lib
	@echo $<
	@$(CXX) -c $(LIB_CXXFLAGS) $< -o $@

.esp_lib/%.o : %.c | .esp_lib
	@echo $<
	@$(CC) -c $(LIB_CFLAGS) $< -o $@

.esp_lib/%.o : %.S | .esp_lib
	@echo $<
	@$(CC) -c $(LIB_ASFLAGS) $< -o $@

########################################################
%.s : %.cpp
	@echo $<
	$(CXX) -Wa,-adhls -c $(CXXFLAGS) $< > $@

%.s : %.c
	@echo $<
	$(CC) -S -c $(CFLAGS) $< -o $@

#VPATH+=$(shell find $(L)/cores/esp8266 -type d)
VPATH+=$(L)/cores/esp8266
VPATH+=$(L)/cores/esp8266/libb64
VPATH+=$(L)/cores/esp8266/spiffs
VPATH+=$(L)/cores/esp8266/umm_malloc


#LIB_SRC = $(shell cd $(L); ls "*.S")
#LIB_SRC+= $(shell cd $(L); ls "*.c")
#LIB_SRC+= $(shell cd $(L); ls "lib64/*.c")
#LIB_SRC+= $(shell cd $(L); ls "spiffs/*.c")
#LIB_SRC+= $(shell cd $(L); ls "*.cpp")
##LIB_SRC+= $(shell find $(L)/cores/esp8266 -name "*.cpp" | sort)


LIB_SRC=\
		cont.S \
		cont_util.c \
		core_esp8266_eboot_command.c \
		core_esp8266_flash_utils.c \
		core_esp8266_i2s.c \
		core_esp8266_main.cpp \
		core_esp8266_noniso.c \
		core_esp8266_phy.c \
		core_esp8266_postmortem.c \
		core_esp8266_si2c.c \
		core_esp8266_timer.c \
		core_esp8266_wiring_analog.c \
		core_esp8266_wiring.c \
		core_esp8266_wiring_digital.c \
		core_esp8266_wiring_pulse.c \
		core_esp8266_wiring_pwm.c \
		core_esp8266_wiring_shift.c \
		libc_replacements.c \
		heap.c \
		uart.c \
		libb64/cdecode.c \
		libb64/cencode.c \
		spiffs/spiffs_cache.c \
		spiffs/spiffs_check.c \
		spiffs/spiffs_gc.c \
		spiffs/spiffs_hydrogen.c \
		spiffs/spiffs_nucleus.c \
		umm_malloc.c \
		IPAddress.cpp \
		MD5Builder.cpp \
		pgmspace.cpp \
		abi.cpp \
		base64.cpp \
		cbuf.cpp \
		Print.cpp \
		Schedule.cpp \
		spiffs_api.cpp \
		spiffs_hal.cpp \
		debug.cpp \
		Esp.cpp \
		FS.cpp \
		HardwareSerial.cpp \
		Stream.cpp \
		StreamString.cpp \
		time.c \
		Tone.cpp \
		Updater.cpp \
		WMath.cpp \
		WString.cpp


LIB_SRC:=$(notdir $(LIB_SRC))

LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_SRC))
LIB_OBJ:=$(patsubst %.c,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.S,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.o,.esp_lib/%.o,$(LIB_OBJ))

##############################

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,.esp_obj/%.o,$(notdir $(OBJ)))

TARGET:=$(TARGET:.hex=)

.PHONY: clean upload all

all : $(TARGET).hex

$(TARGET).hex : libEsp.ar $(OBJ)
	xtensa-lx106-elf-gcc -g -w -Os -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static  \
		"-L$(L)/tools/sdk/lib" \
		"-L$(L)/tools/sdk/ld" \
		"-Teagle.flash.4m.ld" \
		-Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,register_chipv6_phy  \
		-o $(TARGET).elf \
		-Wl,--start-group $(OBJ) \
		libEsp.ar \
		-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 -llwip_gcc -lstdc++ -Wl,--end-group -L.
	echo ciao



#$(PREFIX)strip -g $(TARGET).elf
#$(PREFIX)objdump -S -D $(TARGET).elf > $(TARGET).dis
#$(PREFIX)objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
#$(PREFIX)objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
#$(PREFIX)size $(TARGET).elf

libEsp.ar : $(LIB_OBJ)
	rm -f $@
	$(PREFIX)ar cru $@ $^

.esp_lib:
	mkdir .esp_lib

.esp_obj:
	mkdir .esp_obj

clean:
	-rm -f libEsp.ar
	-rm -rf .esp_obj
	-rm -rf .esp_lib
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).dis

-include $(OBJ:.o=.d)
