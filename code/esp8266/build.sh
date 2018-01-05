#	VERBOSE=0                 \
make -f ../makeEspArduino/makeEspArduino.mk \
	BUILD_DIR=./build         \
	SKETCH=./leo.cpp          \
	FLASH_DEF=4M3M            \
	LWIP_VARIANT=v2mss536     \
	$*

#awk -f esp.awk /home/leo/.arduino15/packages/esp8266/hardware/esp8266/2.4.0/boards.txt
