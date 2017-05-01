CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
AR=arm-none-eabi-ar

PROJECT_NAME = Demo01
PROJECT_SRCS = main.c
PROJECT_OBJS = main.o

STMLIB_DIR   = /home/leo/teensy/LibStm/STM32F10x_StdPeriph_Lib_V3.5.0
DEVBOARD     = STM3210B_EVAL

#LD_SCRIPT = $(STMLIB_DIR)/Project/STM32F10x_StdPeriph_Template/TrueSTUDIO/STM3210B-EVAL/stm32_flash.ld
LD_SCRIPT = ./stm32-h103.ld
STM32_LIB = libstm32.a
TARGET    = $(PROJECT_NAME).elf

INCLUDES  = \
			-I $(STMLIB_DIR)/Libraries/CMSIS/CM3/CoreSupport \
			-I $(STMLIB_DIR)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
			-I $(STMLIB_DIR)/Libraries/STM32F10x_StdPeriph_Driver/inc \
			-I.

#COMMON_CFLAGS = -mthumb -mcpu=cortex-m3 -std=gnu90 -Wall -ffunction-sections -fdata-sections -Os
COMMON_CFLAGS = -mthumb -mcpu=cortex-m3 -Wall -ffunction-sections -fdata-sections -g -O0 -DUSE_FULL_ASSERT
DRVLIB_CFLAGS = -DUSE_$(DEVBOARD) -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

LD_SYS_LIBS=-L/home/leo/teensy/LibStm/libopencm3/lib -lopencm3_stm32f1

LFLAGS = -mthumb -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -T$(LD_SCRIPT) $(LD_SYS_LIBS) -static -Wl,-cref,-Map=$(PROJECT_NAME).map -Wl,--gc-sections

target : ./libstm32.a $(TARGET) 

./libstm32.a:
	make -f ./makefile_lib.mk

$(TARGET) : $(STM32_LIB) $(PROJECT_OBJS)
	#$(CC) $(LFLAGS) $(PROJECT_OBJS) $(STM32_LIB) -o $(TARGET) -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
	arm-none-eabi-gcc --static -nostartfiles -Tstm32-h103.ld -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd \
		-Wl,-Map=miniblink.map -Wl,--gc-sections \
		-L/home/leo/teensy/LibStm/libopencm3/lib \
		-L. \
		$(PROJECT_OBJS) \
		-lopencm3_stm32f1 -Wl,--start-group -lstm32 -lc -lgcc -lnosys -Wl,--end-group -o $(TARGET)


%.o: %.c
	$(CC) -c $(COMMON_CFLAGS) $(DRVLIB_CFLAGS) $(INCLUDES) $< -o $@

%.o: %.cpp
	$(CXX) -c $(COMMON_CFLAGS) $(DRVLIB_CFLAGS) $(INCLUDES) $< -o $@

clean:
	-rm -f *.a *.o  $(PROJECT_NAME).elf $(PROJECT_NAME).map
	make -f ./makefile_lib.mk clean

.PHONY: clean lib target
