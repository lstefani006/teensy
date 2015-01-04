# cpu =  cortex-m4
# arch = armv7e-m
#
# path dell'ambiente di sviluppo
ARDUINO=/home/leo/teensy/arduino
TEENSY=$(ARDUINO)/hardware/teensy/cores/teensy3

# TIME_T serve per inizializzare il RTC ad una data/ora piu' o meno esatta
CFLAGS=-Os -Wall \
	-ffunction-sections -fdata-sections \
	-mcpu=cortex-m4 \
	-mfloat-abi=soft \
	-mthumb \
	-Wall -Wextra -Wpedantic \
	-MMD \
	-DF_CPU=96000000 -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	-D__MK20DX256__ -DTEENSYDUINO=118 \
	-DTIME_T=1412425835 -DUSB_SERIAL \
	-DLAYOUT_US_ENGLISH \
	-I $(TEENSY) \
	-Wwrite-strings \
	-Winit-self \
	-Wcast-align \
	-Wcast-qual \
	-Wpointer-arith \
	-Wstrict-aliasing \
	-Wformat=2 \
	-Wmissing-include-dirs \
	-Wno-unused-parameter \
	-Wuninitialized \
	-Wzero-as-null-pointer-constant

CXXFLAGS = $(CFLAGS) \
	-fno-rtti -felide-constructors -std=gnu++11 -fno-exceptions 

CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
AR=arm-none-eabi-ar

obj/%.o : %.cpp
	@echo $(notdir $<)
	$(CXX) -c $(CXXFLAGS) $< -o $@
obj/%.o : %.c
	@echo $(notdir $<)
	@$(CC) -c $(CFLAGS) $< -o $@

%.s : %.cpp
	$(CXX) -S -c $(CXXFLAGS) $< -o $@
%.s : %.c
	$(CC) -S -c $(CFLAGS) $< -o $@
