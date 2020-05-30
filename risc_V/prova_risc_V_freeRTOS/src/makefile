T=riscv-nuclei-elf

CC=$(T)-gcc
CXX=$(T)-g++
OBJDUMP=$(T)-objdump
OBJCOPY=$(T)-objcopy
OBJSIZE=$(T)-objsize

TARGET=leo
OBJ_DIR=obj

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

C_SOURCES =  \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/GD32VF103_standard_peripheral/*.c} \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/GD32VF103_standard_peripheral/Source/*.c} \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/RISCV/drivers/*.c} \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/RISCV/env_Eclipse/*.c} \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/RISCV/stubs/*.c} \
		${wildcard $(TOP_DIR)/freertos/*.c} \
		${wildcard $(TOP_DIR)/freertos/portable/GCC/N200/*.c} \
		${wildcard $(TOP_DIR)/freertos/portable/MemMang/heap_4.c} \
		${wildcard $(TOP_DIR)/SEGGER/*.c} \
		${wildcard $(TOP_DIR)/SEGGER/FreeRTOSV10/*.c} \
		${wildcard $(TOP_DIR)/SEGGER/FreeRTOSV10/Config/*.c} \
		${wildcard $(TOP_DIR)/SEGGER/port/*.c} \
		${wildcard $(TOP_DIR)/Application/*.c} \

# ASM sources
ASM_SOURCES =  \
		${wildcard $(TOP_DIR)/GD32VF103_Firmware_Library/RISCV/env_Eclipse/*.S} \
		${wildcard $(TOP_DIR)/freertos/portable/GCC/N200/*.S}

#######################################
# CFLAGS
#######################################
# cpu
#      rv32i[m][a][f[d]][c]
#      rv32g[c]
#  	   rv64i[m][a][f[d]][c]
#      rv64g[c]
# GD32
CPU = -march=rv32imac
# abi
#      ilp32
#      ilp32f
#      ilp32d
#      lp64
#      lp64f
#      lp64d
ABI = -mabi=ilp32

# mcu
MCU = $(CPU) $(ABI) -mcmodel=medlow -msmall-data-limit=8 -fmessage-length=0 -fsigned-char

AS_DEFS = \
		-DGD32VF103C_START \
		-DUSE_STDPERIPH_DRIVER

# C defines
C_DEFS = \
		-DGD32VF103C_START \
		-DUSE_STDPERIPH_DRIVER

# AS includes
AS_INCLUDES = \
		-I $(TOP_DIR)/GD32VF103_Firmware_Library/RISCV/drivers

TOP_DIR=/home/leo/.platformio/packages/framework-gd32vf103-sdk
# C includes
C_INCLUDES = \
		-I $(TOP_DIR)/GD32VF103_standard_peripheral  \
		-I $(TOP_DIR)/GD32VF103_standard_peripheral/Include  \
		-I $(TOP_DIR)/RISCV/drivers  \
		-I $(TOP_DIR)/freertos/include \
		-I freertos/include \
		-I freertos/portable/GCC/N200 \
		-I .



# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CXXFLAGS=$(CFLAGS)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#######################################
# LDFLAGS
#######################################
# link script
LD_FILE = $(TOP_DIR)/RISCV/env_Eclipse/GD32VF103xB.lds
LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

# libraries
LIBS = -lm 
LIBDIR =
LDFLAGS = $(MCU) -nostartfiles -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(TARGET).map -Wl,--gc-sections

# default action: build all

$(TARGET).elf : $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET).elf
	#$(CC) --static -nostartfiles -T $(LD_FILE) \
	#	-Wl,-Map=$(TARGET).map -Wl,--gc-sections \
	#	-L$(OPENCM3_DIR)/lib $(OBJ) -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o $(TARGET).elf
	$(OBJCOPY)-O binary  "$(TARGET).elf" "$(TARGET).bin"
	$(OBJDUMP) -h -S $(TARGET).elf > $(TARGET).dis
	$(OBJSIZE) $(TARGET).elf

$(OBJ_DIR)/%.o : %.cpp
	@echo $<
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o : %.c
	@echo $<
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

