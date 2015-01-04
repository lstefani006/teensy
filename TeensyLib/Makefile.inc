TEENSYLIB:=$(dir $(lastword $(MAKEFILE_LIST)))
include $(TEENSYLIB)/Makefile.base

CFLAGS+=-MMD
CXXFLAGS+=-MMD

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
OBJ:=$(patsubst %.c,%.o,$(OBJ))
OBJ:=$(patsubst %.o,obj/%.o,$(OBJ))

FTARGET=$(basename $(TARGET))

all : $(TARGET)

#	-Wl,-v   --> verboso
$(FTARGET).hex : $(OBJ) $(TEENSYLIB)/mk20dx128.o $(TEENSYLIB)/libTeensy.a
	@echo link $(FTARGET)
	@teensy-g++ $(OBJ) -lm -o $(FTARGET).elf
	@arm-none-eabi-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load \
		--no-change-warnings --change-section-lma .eeprom=0 \
		$(FTARGET).elf $(FTARGET).eep 
	@arm-none-eabi-strip -g $(FTARGET).elf
	@arm-none-eabi-objcopy -O ihex -R .eeprom $(FTARGET).elf $(FTARGET).hex 
	@arm-none-eabi-objdump -S -D $(FTARGET).elf > $(FTARGET).dis 
	@arm-none-eabi-size  $(FTARGET).elf

$(FTARGET).a : $(OBJ)
	@echo $(notdir $<)
	@rm -f $@
	@$(AR) -cvq $@ $^

PHONY: clean upload distclean

$(OBJ): | obj

obj :
	echo $(OBJ)
	mkdir -p obj

$(TEENSYLIB)/libTeensy.a $(TEENSYLIB)/mk20dx128.o $(TEENSYLIB)/teensy_loader_cli : 
	make -C $(TEENSYLIB)

upload : $(FTARGET).hex $(TEENSYLIB)/teensy_loader_cli
	echo $(TEENSYLIB)/teensy_loader_cli -mmcu=mk20dx256 -v -w $(FTARGET).hex
	$(TEENSYLIB)/teensy_loader_cli -mmcu=mk20dx256 -v -w $(FTARGET).hex

-include $(OBJ:.o=.d)

clean : 
	-rm -f $(FTARGET).a
	-rm -f $(FTARGET).elf
	-rm -f $(FTARGET).eep
	-rm -f $(FTARGET).hex
	-rm -f $(FTARGET).map
	-rm -f $(FTARGET).dis
	-rm -f $(FTARGET).d
	-rm -f $(FTARGET).o
	-rm -f $(FTARGET).s
	-rm -rf obj

distclean: clean
	make -C $(TEENSYLIB) clean
