#
# make 
# make clean
# make flash
#
BINARY = miniblink
OBJS = usart_irq.o

LDSCRIPT = stm32-h103.ld

include ./Makefile.include

.PHONY: compile_commands.json

compile_commands.json :
	make clean
	bear make
	cat $(OPENCM3_DIR)/compile_commands.json >> compile_commands.json
