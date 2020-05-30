W=0
ECHO=
G=-g
O=-Os

ifdef TEENSY
	CXXFLAGS=-I../TeensyLib 
	VPATH=../LibTeensy
else  ifdef ESP8266
	CXXFLAGS=-I../LibEsp
	VPATH=../EspLib
else ifdef STM32F103
	CXXFLAGS=-I../LibSt -g
	VPATH=../StLib
else
	CXXFLAGS=-I../LibAvr
	VPATH=../AvrLib
endif

TARGET=Blink.hex
SRC+=\
	Blink.cpp

include ../util/makefile.mk



gdb: leo.gdbinit
	arm-none-eabi-gdb -tui -x leo.gdbinit $(TARGET).elf

leo.gdbinit:
	echo "target extended-remote /dev/ttyBmpGdb"  >  leo.gdbinit
	echo "mon swdp_scan"                          >> leo.gdbinit
	echo "attach 1"                               >> leo.gdbinit
	echo "set mem inaccessible-by-default off"    >> leo.gdbinit
	echo "monitor vector_catch disable hard"      >> leo.gdbinit
	echo "monitor option erase"                   >> leo.gdbinit
	echo "set print pretty"                       >> leo.gdbinit
	echo "load"                                   >> leo.gdbinit
	echo "b main"                                 >> leo.gdbinit


upload : all
	@ArduinoSerialMonitor.exe -stop
	/home/leo/Arduino/hardware/Arduino_STM32/tools/linux/serial_upload ttyUSB0 1 2 $(TARGET).bin
	@ArduinoSerialMonitor.exe -run

flash: all
	arm-none-eabi-gdb --batch \
		-ex 'target extended-remote /dev/ttyBmpGdb' \
		-x $(OPENCM3_DIR)/scripts/black_magic_probe_flash.scr \
		$(TARGET).elf
