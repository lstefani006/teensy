
# ho modificato il file
# /home/leo/.arduino15/packages/esp8266/hardware/esp8266/2.4.0/platform.txt
# nella sezione 
# compiler.warning_flags=-w
# ho tolto il -w ==> altrimenti il compilatore ignora il CPP_EXTRA

EMAKE=make -f ../makeEspArduino/makeEspArduino.mk \
	  ESP_ADDR=192.168.1.12 \
	  ESP_PORT=8266 \
	  ESP_PWD=spectrum \
	BUILD_DIR=./build         \
	SKETCH=./leo.cpp          \
	FLASH_DEF=4M1M            \
	LWIP_VARIANT=v2mss536     \
	CPP_EXTRA="-Wall -Wextra" \
#	VERBOSE=0 \

all:
	$(EMAKE)
help:
	$(EMAKE) help
help_opz:
	awk -f esp.awk ~/.arduino15/packages/esp8266/hardware/esp8266/2.4.0/boards.txt
clean:
	$(EMAKE) clean
flash :
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) flash
	ArduinoSerialMonitor.exe -run
flash_fs:
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) flash_fs
	ArduinoSerialMonitor.exe -run
ota  :
	$(EMAKE) ota
ota_fs:
	$(EMAKE) ota_fs
http:
	$(EMAKE) http
dump_flash:
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) dump_flash
	ArduinoSerialMonitor.exe -run
restore_flash:
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) restore_flash
	ArduinoSerialMonitor.exe -run
dump_fs:
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) dump_fs
	ArduinoSerialMonitor.exe -run
erase_flash:
	ArduinoSerialMonitor.exe -stop
	$(EMAKE) erase_flash
	ArduinoSerialMonitor.exe -run
list_lib: 
	$(EMAKE) list_lib
