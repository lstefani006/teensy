W?=3

ifeq ($(shell test $(W) -ge 1; echo $$?),0)
	WCOMMON_1=-Wreturn-type
	WC+=$(WCOMMON_1)
	WCXX+=$(WCOMMON_1)
endif

ifeq ($(shell test $(W) -ge 2; echo $$?),0)
	WCOMMON_2= -Wall \
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
			 -Wlogical-op \
			 -Wwrite-strings \
			 -Wcast-align \
			 -Wcast-qual \
			 -Wpointer-arith \
			 -Wstrict-aliasing \
			 -Wformat \
			 -Wmissing-include-dirs \
			 -Wno-unused-parameter \
			 -Wuninitialized 
	WC+= $(WCOMMON_2) \
		 -Wstrict-prototypes 
	WCXX+= $(WCOMMON_2) \
		 -Wuseless-cast \
		 -Wzero-as-null-pointer-constant
endif

ifeq ($(shell test $(W) -ge 3; echo $$?),0)
	WCOMMON_3= -Wextra \
			 -Wempty-body
	WC+=$(WCOMMON_3)
	WCXX+=$(WCOMMON_3)
endif

ifeq ($(shell test $(W) -ge 4; echo $$?),0)
	# -Wpedantic emette warnings per violazioni ISC C++... ma non aiuta molto
	WCOMMON_4= \
			   -Waggregate-return \
			   -Wconversion
	WC+=$(WCOMMON_4)
	WCXX+=$(WCOMMON_4)
endif

##################################################
CCOMMON=-Os \
		-g \
		-ffunction-sections -fdata-sections \
		-nostdlib \
		-MMD \
		-mthumb -mcpu=cortex-m4 -fsingle-precision-constant \
		-D__MK20DX256__ -DTEENSYDUINO=128 -DARDUINO=10608 -DF_CPU=72000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH \
		-I/home/leo/arduino-1.6.8/hardware/teensy/avr/cores/teensy3

CFLAGS+=$(CCOMMON) $(WC)
CXXFLAGS+=$(CCOMMON) $(WCXX) -std=gnu++11 -fno-exceptions -felide-constructors -fno-rtti

OBJ_DIR=.obj3
LIB_DIR=.lib3

$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	@echo $<
	@arm-none-eabi-g++ -c $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	@echo $<
	@arm-none-eabi-gcc -c $(CFLAGS) $< -o $@

$(LIB_DIR)/%.o : %.S | $(LIB_DIR)
	@echo $<
	@arm-none-eabi-gcc -c -x assembler-with-cpp $(CFLAGS) $< -o $@

$(LIB_DIR)/%.o : %.cpp | $(LIB_DIR)
	@echo $<
	@arm-none-eabi-g++ -c $(CXXFLAGS) $< -o $@

$(LIB_DIR)/%.o : %.c | $(LIB_DIR)
	@echo $<
	@arm-none-eabi-gcc -c $(CFLAGS) $< -o $@


VPATH+=$(TEENSY)
LIB_SRC = \
		  memcpy-armv7m.S \
		  memset.S \
		  analog.c \
		  eeprom.c \
		  keylayouts.c \
		  math_helper.c \
		  mk20dx128.c \
		  nonstd.c \
		  pins_teensy.c \
		  ser_print.c \
		  serial1.c \
		  serial2.c \
		  serial3.c \
		  touch.c \
		  usb_desc.c \
		  usb_dev.c \
		  usb_joystick.c \
		  usb_keyboard.c \
		  usb_mem.c \
		  usb_midi.c \
		  usb_mouse.c \
		  usb_mtp.c \
		  usb_rawhid.c \
		  usb_seremu.c \
		  usb_serial.c \
		  AudioStream.cpp \
		  DMAChannel.cpp \
		  HardwareSerial1.cpp \
		  HardwareSerial2.cpp \
		  HardwareSerial3.cpp \
		  IPAddress.cpp \
		  IntervalTimer.cpp \
		  Print.cpp \
		  Stream.cpp \
		  Tone.cpp \
		  WMath.cpp \
		  WString.cpp \
		  avr_emulation.cpp \
		  main.cpp \
		  new.cpp \
		  usb_flightsim.cpp \
		  usb_inst.cpp \
		  yield.cpp

LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_SRC))
LIB_OBJ:=$(patsubst %.c,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.S,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.o,$(LIB_DIR)/%.o,$(LIB_OBJ))

##############################

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

TARGET:=$(TARGET:.hex=)

.PHONY: clean upload

all : $(TARGET).hex

$(TARGET).hex : libTeensy.a $(OBJ)
	@arm-none-eabi-gcc -O -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=1472813328 "-T$(TEENSY)/mk20dx256.ld"  -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -o $(TARGET).elf $(OBJ) libTeensy.a -lm
	@arm-none-eabi-objdump -h -S $(TARGET).elf > $(TARGET).dis
	@arm-none-eabi-strip -g $(TARGET).elf
	@arm-none-eabi-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	@arm-none-eabi-objcopy -O ihex -R .eeprom "$(TARGET).elf" "$(TARGET).hex"
	@arm-none-eabi-size $(TARGET).elf

libTeensy.a : $(LIB_OBJ)
	@rm -f $@
	@arm-none-eabi-ar rcs $@ $^

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	-rm -f libTeensy.a
	-rm -rf $(OBJ_DIR)
	-rm -rf $(LIB_DIR)
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).dis

#VERBOSE=-v
VERBOSE=
upload : $(TARGET).hex
	@ArduinoSerialMonitor.exe -stop
	/home/leo/teensy/TeensyLib/teensy_loader_cli -mmcu=mk20dx256 -v -w $(TARGET).hex
	@ArduinoSerialMonitor.exe -run

-include $(OBJ:.o=.d)
