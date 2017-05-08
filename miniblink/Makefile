#
# make 
# make clean
# make flash
#
BINARY = miniblink
OBJS = usart_irq.o systick_setup.o rtc_setup.o
CPPFLAGS = -O0 -Wall -std=c++17

LDSCRIPT = stm32-h103.ld

include ./Makefile.include

.PHONY: compile_commands.json

compile_commands.json :
	make clean
	bear make
	cat $(OPENCM3_DIR)/compile_commands.json >> compile_commands.json

.PHONY: lib.debug lib.clean lib.os debug

lib.debug:
	make -C ../LibStm/libopencm3 V=1 "DEBUG_FLAGS=-O0 -ggdb3"
lib.os:
	make -C ../LibStm/libopencm3 V=1
lib.clean:
	make -C ../LibStm/libopencm3 V=1 clean

debug:
	arm-none-eabi-gdb -tui -x leo.gdbinit miniblink.elf
