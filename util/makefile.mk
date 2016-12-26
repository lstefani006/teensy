
ifeq ($(PLATFORM),STM32F103)
	include ../util/st_makefile.mk
else ifeq ($(PLATFORM),TEENSY)
	include ../util/teensy_makefile.mk
else ifeq ($(PLATFORM),ESP8266)
	include ../util/esp_makefile.mk
else ifeq ($(PLATFORM),AVR)
	include ../util/avr_makefile.mk
else
	error
endif
