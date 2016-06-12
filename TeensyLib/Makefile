include Makefile.base

# path dei $(LIB_SRC)
VPATH=$(TEENSY)

LIB_SRC= \
		 analog.c \
		 eeprom.c \
		 keylayouts.c \
		 math_helper.c \
		 nonstd.c \
		 pins_teensy.c \
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
		 usb_rawhid.c \
		 usb_seremu.c \
		 usb_serial.c \
		 yield.c \
		 AudioStream.cpp \
		 avr_emulation.cpp \
		 HardwareSerial1.cpp \
		 HardwareSerial2.cpp \
		 HardwareSerial3.cpp \
		 IntervalTimer.cpp \
		 IPAddress.cpp \
		 main.cpp \
		 new.cpp \
		 Print.cpp \
		 Stream.cpp \
		 Tone.cpp \
		 usb_flightsim.cpp \
		 usb_inst.cpp \
		 WMath.cpp \
		 WString.cpp


# si usa := per evitare la ricursione infinita che si avrebbe con =
LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_SRC))
LIB_OBJ:=$(patsubst %.c,%.o,$(LIB_OBJ))
LIB_OBJ:=$(patsubst %.o,obj/%.o,$(LIB_OBJ))

TARGET=libTeensy.a

all : \
	t_mk20dx128.o \
	t_io.o \
	$(TARGET) \
	teensy_loader_cli

all_old : .patched_itoa \
	t_mk20dx128.o \
	t_io.o \
	$(TARGET) \
	teensy_loader_cli

.patched_itoa:
	patch $(TEENSY)/avr_functions.h patch_atoi.txt
	echo non cancellare questo file dopo aver fatto questa patch >  .patched_itoa

mk20dx128.o : obj/mk20dx128.o
	cp obj/mk20dx128.o .
	t_io.o : obj/t_io.o
	cp obj/t_io.o .

t_mk20dx128.c : mk20dx128.c
	awk -f teensy_patch.awk $< > $@

obj/t_mk20dx128.o : | obj
obj/t_io.o : | obj
$(LIB_OBJ): | obj

obj :
	mkdir -p obj

$(TARGET) : $(LIB_OBJ)
	@echo $(notdir $<)
	@rm -f $@
	@$(AR) -cvq $@ $^

-include $(LIB_OBJ:.o=.d)

clean : 
	-rm -f $(TARGET)
	-rm -f t_mk20dx128.o
	-rm -f t_mk20dx128.c
	-rm -f t_io.o
	-rm -rf obj
	-rm -f teensy_loader_cli
	if [ -e .patched_itoa ]; then \
		patch -R $(TEENSY)/avr_functions.h patch_atoi.txt ; \
		rm .patched_itoa; \
	fi

ifeq ($(shell uname),Linux)
DD=-DUSE_LIBUSB=1
LL=-lusb
else
DD=-DUSE_WIN32=1
LL=-lsetupapi -lhid
endif

teensy_loader_cli : teensy_loader_cli.c
	gcc $(DD) teensy_loader_cli.c $(LL) -o teensy_loader_cli
