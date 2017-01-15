include ../util/base_makefile.mk

ARDUINO_PRIV=$(HOME)/Arduino

INCLUDE=\
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple/include \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple/stm32f1/include \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple/usb/stm32f1 \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple/usb/usb_lib \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/system/libmaple/stm32f1/include/series \
		-I$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c


CF_COMMON=\
		  $(INCLUDE) \
		  -mthumb  \
		  -march=armv7-m \
		  -mcpu=cortex-m3 \
		  -DF_CPU=72000000L \
		  -DARDUINO=10611 \
		  -DARDUINO_GENERIC_STM32F103C \
		  -DARDUINO_ARCH_STM32F1   \
		  -DMCU_STM32F103CB  \
		  -D__STM32F1__ \
		  -DMCU_STM32F103CB  \
		  -D__STM32F1__ 

CF_S_LIB=\
		 $(CF_COMMON) \
		 -x assembler-with-cpp 

CF_C_LIB=\
		 $(CF_COMMON) \
		 -Os  \
		 -DDEBUG_LEVEL=DEBUG_ALL  \
		 -ffunction-sections  \
		 -fdata-sections  \
		 -nostdlib  \
		 --param max-inline-insns-single=500  \
		 -DBOARD_generic_stm32f103c  \
		 -DVECT_TAB_ADDR=0x8000000  \
		 -DERROR_LED_PORT=GPIOB  \
		 -DERROR_LED_PIN=1 

CF_CXX_LIB=\
		   $(CF_COMMON) \
		   $(CF_C_LIB) \
		   -fno-rtti \
		   -fno-exceptions 

CFLAGS+=$(CF_C_LIB) \
		-MMD \
		-Wall  \
		-Wextra
CXXFLAGS+=$(CF_CXX_LIB) \
		  -MMD \
		  -Wall  \
		  -Wextra \
		  -std=gnu++14

##########################

OBJ_DIR=.obj_st
LIB_DIR=.lib_st

E?=@

$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	@echo $<
	$(E)arm-none-eabi-g++ -c $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	@echo $<
	$(E)arm-none-eabi-gcc -c $(CFLAGS) $< -o $@

$(LIB_DIR)/%.o : %.S | $(LIB_DIR)
	@echo $<
	$(E)arm-none-eabi-gcc -c $(CF_S_LIB) $< -o $@

$(LIB_DIR)/%.o : %.cpp | $(LIB_DIR)
	@echo $<
	$(E)arm-none-eabi-g++ -c $(CF_CXX_LIB) $< -o $@

$(LIB_DIR)/%.o : %.c | $(LIB_DIR)
	@echo $<
	$(E)arm-none-eabi-gcc -c $(CF_C_LIB) $< -o $@



VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/libmaple/stm32f1/performance
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/libmaple
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/stm32f1
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/avr
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/libmaple/usb/stm32f1
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/libmaple/usb/usb_lib
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/cores/maple/stm32f1
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c/wirish
VPATH+=$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c

LIB_SRC = \
		  exc.S \
		  isrs.S \
		  vector_table.S \
		  hooks.c \
		  itoa.c \
		  dtostrf.c \
		  adc.c \
		  adc_f1.c \
		  bkp_f1.c \
		  dac.c \
		  dma.c \
		  dma_f1.c \
		  exti.c \
		  exti_f1.c \
		  flash.c \
		  fsmc_f1.c \
		  gpio.c \
		  gpio_f1.c \
		  i2c.c \
		  i2c_f1.c \
		  iwdg.c \
		  nvic.c \
		  pwr.c \
		  rcc.c \
		  rcc_f1.c \
		  spi.c \
		  spi_f1.c \
		  systick.c \
		  timer.c \
		  timer_f1.c \
		  usart.c \
		  usart_f1.c \
		  usart_private.c \
		  util.c \
		  usb.c \
		  usb_cdcacm.c \
		  usb_reg_map.c \
		  usb_core.c \
		  usb_init.c \
		  usb_mem.c \
		  usb_regs.c \
		  util_hooks.c \
		  HardwareSerial.cpp \
		  HardwareTimer.cpp \
		  IPAddress.cpp \
		  Print.cpp \
		  Stream.cpp \
		  WString.cpp \
		  cxxabi-compat.cpp \
		  ext_interrupts.cpp \
		  main.cpp \
		  pwm.cpp \
		  usb_serial.cpp \
		  wirish_analog.cpp \
		  wirish_digital.cpp \
		  wirish_math.cpp \
		  wirish_shift.cpp \
		  wirish_time.cpp \
		  wiring_pulse_f1.cpp \
		  wirish_debug.cpp \
		  wirish_digital_f1.cpp

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

all : $(TARGET).bin

$(TARGET).bin : libST.a $(OBJ) $(LIB_DIR)/start.o $(LIB_DIR)/start_c.o $(LIB_DIR)/syscalls.o $(LIB_DIR)/board.o $(LIB_DIR)/boards.o $(LIB_DIR)/boards_setup.o
	@echo link
	$(E)arm-none-eabi-gcc -Os -Wl,--gc-sections -mcpu=cortex-m3 \
		-T$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c/ld/jtag.ld \
		"-Wl,-Map,$(TARGET).map" \
		"-L$(ARDUINO_PRIV)/hardware/Arduino_STM32/STM32F1/variants/generic_stm32f103c/ld" \
		-o "$(TARGET).elf" \
		"-L." \
		-lm -lgcc -mthumb \
		-Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all \
		-Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols \
		-Wl,--start-group \
		$(OBJ) \
		$(LIB_DIR)/start.o \
		$(LIB_DIR)/start_c.o \
		$(LIB_DIR)/syscalls.o \
		$(LIB_DIR)/board.o \
		$(LIB_DIR)/boards.o \
		$(LIB_DIR)/boards_setup.o \
		libST.a \
		-Wl,--end-group -lstdc++
	$(E)arm-none-eabi-objcopy -O binary  "$(TARGET).elf" "$(TARGET).bin"
	$(E)arm-none-eabi-size $(TARGET).elf
	$(E)arm-none-eabi-objdump -h -S $(TARGET).elf > $(TARGET).dis

libST.a : $(LIB_OBJ)
	@rm -f $@
	@arm-none-eabi-ar rcs $@ $^

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	-rm -f libST.a
	-rm -rf $(OBJ_DIR)
	-rm -rf $(LIB_DIR)
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).bin
	-rm -f $(TARGET).dis
	-rm -f $(TARGET).map

#VERBOSE=-v
VERBOSE=
upload : $(TARGET).bin
	@ArduinoSerialMonitor.exe -stop
	/home/leo/Arduino/hardware/Arduino_STM32/tools/linux/serial_upload ttyUSB0 1 2 $(TARGET).bin
	@ArduinoSerialMonitor.exe -run

-include $(OBJ:.o=.d)
