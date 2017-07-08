
SRC=522_ST.cpp Arduino.cpp rtc_setup.cpp usart_setup.cpp uprintf.cpp spi_setup.cpp ring.cpp systick_setup.cpp MFRC522.cpp MFRC522Debug.cpp
TARGET=522_ST
OBJ_DIR=.obj


VPATH+=../miniblink
VPATH+=../util
VPATH+=../rfid/src

CPPFLAGS+=-I../rfid/src 
CPPFLAGS+=-I../miniblink 
CPPFLAGS+=-I../util

CPPFLAGS+=-I.
############################################
E?=@
O?=-Os -g
W?=-Wextra -Wshadow -Wredundant-decls -Weffc++ -Wall
CPPFLAGS+=$(O)
CPPFLAGS+=-mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd
CPPFLAGS+=$(W)
CPPFLAGS+=-fno-common -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions
CPPFLAGS+=-MD -Wundef
CPPFLAGS+=-DSTM32F1
CPPFLAGS+=-I/home/leo/teensy/LibStm/libopencm3/include 
CPPFLAGS+=-std=c++17

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

$(TARGET).elf : $(OBJ) $(OBJ_DIR)/lib.os
	$(E)arm-none-eabi-gcc --static -nostartfiles -Tstm32-h103.ld -mthumb \
		-mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd \
		-Wl,-Map=$(TARGET).map -Wl,--gc-sections \
		-L/home/leo/teensy/LibStm/libopencm3/lib $(OBJ) -lopencm3_stm32f1 -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o $(TARGET).elf
	$(E)arm-none-eabi-objcopy -O binary  "$(TARGET).elf" "$(TARGET).bin"
	$(E)arm-none-eabi-objdump -h -S $(TARGET).elf > $(TARGET).dis
	$(E)arm-none-eabi-size $(TARGET).elf


$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	@echo $<
	$(E)arm-none-eabi-g++ $(G) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	@echo $<
	$(E)arm-none-eabi-g++ $(G) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	-rm -rf $(OBJ_DIR)
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).bin
	-rm -f $(TARGET).dis
	-rm -f $(TARGET).map
	-rm -f compile_commands.json

compile_commands.json :
	make clean
	bear make
	cat $(OPENCM3_DIR)/compile_commands.json >> compile_commands.json

.PHONY: lib.debug lib.clean lib.os gdb map upload

$(OBJ_DIR)/lib.debug:
	touch $(OBJ_DIR)/lib.debug
	make -C ../LibStm/libopencm3 V=1 "DEBUG_FLAGS=-O0 -ggdb3"

$(OBJ_DIR)/lib.os:
	touch $(OBJ_DIR)/lib.os
	make -C ../LibStm/libopencm3 V=1

lib.clean:
	rm -f $(OBJ_DIR)/lib.debug
	rm -f $(OBJ_DIR)/lib.os
	make -C ../LibStm/libopencm3 V=1 clean

gdb: leo.gdbinit
	arm-none-eabi-gdb -tui -x leo.gdbinit $(TARGET).elf

leo.gdbinit:
	echo "target extended-remote /dev/ttyBmpGdb" >  leo.gdbinit
	echo "mon swdp_scan                          >> leo.gdbinit
	echo "attach 1                               >> leo.gdbinit
	echo "set mem inaccessible-by-default off    >> leo.gdbinit
	echo "monitor vector_catch disable hard      >> leo.gdbinit
	echo "monitor option erase                   >> leo.gdbinit
	echo "set print pretty                       >> leo.gdbinit
	echo "load                                   >> leo.gdbinit
	echo "b main                                 >> leo.gdbinit


upload : $(TARGET).bin
	@ArduinoSerialMonitor.exe -stop
	/home/leo/Arduino/hardware/Arduino_STM32/tools/linux/serial_upload ttyUSB0 1 2 $(TARGET).bin
	@ArduinoSerialMonitor.exe -run

flash:
	arm-none-eabi-gdb --batch \
		-ex 'target extended-remote /dev/ttyBmpGdb' \
		-x /home/leo/teensy/LibStm/libopencm3/scripts/black_magic_probe_flash.scr \
		$(TARGET).elf

info:
	~/Arduino/hardware/Arduino_STM32/tools/linux64/stm32flash/stm32flash /dev/ttyUSB0

disable_write_protection:
	~/Arduino/hardware/Arduino_STM32/tools/linux64/stm32flash/stm32flash -k /dev/ttyUSB0

-include $(OBJ:.o=.d)
