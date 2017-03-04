RESULT ?= ftp_client_demo

DEFINES = \
	-DSTM32F4XX \
	-DUSE_STM32F4_DISCOVERY \
	-DUSE_STDPERIPH_DRIVER \
	-D_WINSOCK_H

INCLUDES += \
	-I../src \
	-I../../../../common/cmsis/include \
	-I../../../../common/st/devices/stm32f4xx \
	-I../../../../common/st/boards \
	-I../../../../common/st/boards/common \
	-I../../../../common/st/boards/stm32f4_discovery \
	-I../../../../common/st/stm32f4xx_std_periph_lib/inc \
	-I../../../../common/freertos/include \
	-I../../../../common/freertos/portable/gcc/arm_cm4f \
	-I../../../../../common \
	-I../../../../../cyclone_tcp \
	-I../../../../../cyclone_ssl \
	-I../../../../../cyclone_crypto

ASM_SOURCES = \
	./startup_stm32f4xx.S \

C_SOURCES = \
	../src/system_stm32f4xx.c \
	../src/stm32f4xx_it.c \
	./syscalls.c \
	../src/main.c \
	../src/debug.c \
	../../../../../common/endian.c \
	../../../../../common/os_port_freertos.c \
	../../../../../common/date_time.c \
	../../../../../common/str.c \
	../../../../../cyclone_tcp/core/net.c \
	../../../../../cyclone_tcp/core/net_mem.c \
	../../../../../cyclone_tcp/drivers/stm32f4x7_eth.c \
	../../../../../cyclone_tcp/drivers/lan8720.c \
	../../../../../cyclone_tcp/core/nic.c \
	../../../../../cyclone_tcp/core/ethernet.c \
	../../../../../cyclone_tcp/ipv4/arp.c \
	../../../../../cyclone_tcp/ipv4/ipv4.c \
	../../../../../cyclone_tcp/ipv4/ipv4_frag.c \
	../../../../../cyclone_tcp/ipv4/icmp.c \
	../../../../../cyclone_tcp/ipv4/igmp.c \
	../../../../../cyclone_tcp/ipv6/ipv6.c \
	../../../../../cyclone_tcp/ipv6/ipv6_frag.c \
	../../../../../cyclone_tcp/ipv6/ipv6_misc.c \
	../../../../../cyclone_tcp/ipv6/ipv6_pmtu.c \
	../../../../../cyclone_tcp/ipv6/icmpv6.c \
	../../../../../cyclone_tcp/ipv6/mld.c \
	../../../../../cyclone_tcp/ipv6/ndp.c \
	../../../../../cyclone_tcp/ipv6/ndp_cache.c \
	../../../../../cyclone_tcp/ipv6/ndp_misc.c \
	../../../../../cyclone_tcp/ipv6/slaac.c \
	../../../../../cyclone_tcp/core/ip.c \
	../../../../../cyclone_tcp/core/tcp.c \
	../../../../../cyclone_tcp/core/tcp_fsm.c \
	../../../../../cyclone_tcp/core/tcp_misc.c \
	../../../../../cyclone_tcp/core/tcp_timer.c \
	../../../../../cyclone_tcp/core/udp.c \
	../../../../../cyclone_tcp/core/socket.c \
	../../../../../cyclone_tcp/core/bsd_socket.c \
	../../../../../cyclone_tcp/core/raw_socket.c \
	../../../../../cyclone_tcp/dns/dns_cache.c \
	../../../../../cyclone_tcp/dns/dns_client.c \
	../../../../../cyclone_tcp/dns/dns_common.c \
	../../../../../cyclone_tcp/dns/dns_debug.c \
	../../../../../cyclone_tcp/mdns/mdns_client.c \
	../../../../../cyclone_tcp/mdns/mdns_responder.c \
	../../../../../cyclone_tcp/mdns/mdns_common.c \
	../../../../../cyclone_tcp/netbios/nbns_client.c \
	../../../../../cyclone_tcp/netbios/nbns_responder.c \
	../../../../../cyclone_tcp/netbios/nbns_common.c \
	../../../../../cyclone_tcp/dhcp/dhcp_client.c \
	../../../../../cyclone_tcp/dhcp/dhcp_common.c \
	../../../../../cyclone_tcp/dhcp/dhcp_debug.c \
	../../../../../cyclone_tcp/ftp/ftp_client.c \
	../../../../common/freertos/portable/gcc/arm_cm4f/port.c \
	../../../../common/freertos/croutine.c \
	../../../../common/freertos/list.c \
	../../../../common/freertos/queue.c \
	../../../../common/freertos/tasks.c \
	../../../../common/freertos/timers.c \
	../../../../common/freertos/portable/memmang/heap_3.c \
	../../../../common/st/boards/stm32f4_discovery/stm32f4_discovery.c \
	../../../../common/st/boards/stm32f4_discovery/stm32f4_discovery_lcd.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/misc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_adc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_can.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_crc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_aes.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_des.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_tdes.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dac.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dbgmcu.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dcmi.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dma.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_exti.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_flash.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_fsmc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_gpio.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash_md5.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash_sha1.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_i2c.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_iwdg.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_pwr.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rcc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rng.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rtc.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_sdio.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_spi.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_syscfg.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_tim.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_usart.c \
	../../../../common/st/stm32f4xx_std_periph_lib/src/stm32f4xx_wwdg.c

HEADERS = \
	../src/os_port_config.h \
	../src/net_config.h \
	../src/FreeRTOSConfig.h \
	../src/stm32f4xx_conf.h \
	../src/stm32f4xx_it.h \
	../../../../../common/endian.h \
	../../../../../common/os_port.h \
	../../../../../common/os_port_freertos.h \
	../../../../../common/date_time.h \
	../../../../../common/str.h \
	../../../../../common/error.h \
	../../../../../common/debug.h \
	../../../../../cyclone_tcp/core/net.h \
	../../../../../cyclone_tcp/core/net_mem.h \
	../../../../../cyclone_tcp/drivers/stm32f4x7_eth.h \
	../../../../../cyclone_tcp/drivers/lan8720.h \
	../../../../../cyclone_tcp/core/nic.h \
	../../../../../cyclone_tcp/core/ethernet.h \
	../../../../../cyclone_tcp/ipv4/arp.h \
	../../../../../cyclone_tcp/ipv4/ipv4.h \
	../../../../../cyclone_tcp/ipv4/ipv4_frag.h \
	../../../../../cyclone_tcp/ipv4/icmp.h \
	../../../../../cyclone_tcp/ipv4/igmp.h \
	../../../../../cyclone_tcp/ipv6/ipv6.h \
	../../../../../cyclone_tcp/ipv6/ipv6_frag.h \
	../../../../../cyclone_tcp/ipv6/ipv6_misc.h \
	../../../../../cyclone_tcp/ipv6/ipv6_pmtu.h \
	../../../../../cyclone_tcp/ipv6/icmpv6.h \
	../../../../../cyclone_tcp/ipv6/mld.h \
	../../../../../cyclone_tcp/ipv6/ndp.h \
	../../../../../cyclone_tcp/ipv6/ndp_cache.h \
	../../../../../cyclone_tcp/ipv6/ndp_misc.h \
	../../../../../cyclone_tcp/ipv6/slaac.h \
	../../../../../cyclone_tcp/core/ip.h \
	../../../../../cyclone_tcp/core/tcp.h \
	../../../../../cyclone_tcp/core/tcp_fsm.h \
	../../../../../cyclone_tcp/core/tcp_misc.h \
	../../../../../cyclone_tcp/core/tcp_timer.h \
	../../../../../cyclone_tcp/core/udp.h \
	../../../../../cyclone_tcp/core/socket.h \
	../../../../../cyclone_tcp/core/bsd_socket.h \
	../../../../../cyclone_tcp/core/raw_socket.h \
	../../../../../cyclone_tcp/dns/dns_cache.h \
	../../../../../cyclone_tcp/dns/dns_client.h \
	../../../../../cyclone_tcp/dns/dns_common.h \
	../../../../../cyclone_tcp/dns/dns_debug.h \
	../../../../../cyclone_tcp/mdns/mdns_client.h \
	../../../../../cyclone_tcp/mdns/mdns_responder.h \
	../../../../../cyclone_tcp/mdns/mdns_common.h \
	../../../../../cyclone_tcp/netbios/nbns_client.h \
	../../../../../cyclone_tcp/netbios/nbns_responder.h \
	../../../../../cyclone_tcp/netbios/nbns_common.h \
	../../../../../cyclone_tcp/dhcp/dhcp_client.h \
	../../../../../cyclone_tcp/dhcp/dhcp_common.h \
	../../../../../cyclone_tcp/dhcp/dhcp_debug.h \
	../../../../../cyclone_tcp/ftp/ftp_client.h \
	../../../../common/freertos/portable/gcc/arm_cm4f/portmacro.h \
	../../../../common/freertos/include/croutine.h \
	../../../../common/freertos/include/FreeRTOS.h \
	../../../../common/freertos/include/list.h \
	../../../../common/freertos/include/mpu_wrappers.h \
	../../../../common/freertos/include/portable.h \
	../../../../common/freertos/include/projdefs.h \
	../../../../common/freertos/include/queue.h \
	../../../../common/freertos/include/semphr.h \
	../../../../common/freertos/include/StackMacros.h \
	../../../../common/freertos/include/task.h \
	../../../../common/freertos/include/timers.h \
	../../../../common/st/boards/stm32f4_discovery/stm32f4_discovery.h \
	../../../../common/st/boards/stm32f4_discovery/stm32f4_discovery_lcd.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/misc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_adc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_can.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_crc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_cryp.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dac.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dbgmcu.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dcmi.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dma.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_exti.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_flash.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_fsmc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_gpio.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_hash.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_i2c.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_iwdg.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_pwr.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rcc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rng.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rtc.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_sdio.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_spi.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_syscfg.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_tim.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_usart.h \
	../../../../common/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_wwdg.h

ASM_OBJECTS = $(patsubst %.S, %.o, $(ASM_SOURCES))

C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

OBJ_DIR = obj

LINKER_SCRIPT = stm32f407_flash.ld

CFLAGS += -fno-common -Wall -Os -g3
CFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)

CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))

all: build size

install: build size program

build: $(RESULT).elf $(RESULT).lst $(RESULT).bin $(RESULT).hex
	
$(RESULT).elf: $(ASM_OBJECTS) $(C_OBJECTS) $(HEADERS) $(LINKER_SCRIPT) $(THIS_MAKEFILE)
	$(CC) -Wl,-M=$(RESULT).map -Wl,-T$(LINKER_SCRIPT) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(notdir $(ASM_OBJECTS))) $(addprefix $(OBJ_DIR)/, $(notdir $(C_OBJECTS))) -o $@

$(ASM_OBJECTS): | $(OBJ_DIR)

$(C_OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@

%.o: %.c $(HEADERS) $(THIS_MAKEFILE)
	$(CC) $(CFLAGS) -c $< -o $(addprefix $(OBJ_DIR)/, $(notdir $@))

%.o: %.S $(HEADERS) $(THIS_MAKEFILE)
	$(CC) $(CFLAGS) -c $< -o $(addprefix $(OBJ_DIR)/, $(notdir $@))

%.lst: %.elf
	$(OBJDUMP) -x -S $(RESULT).elf > $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

size: $(RESULT).elf
	$(SIZE) $(RESULT).elf

program:
	openocd -f board/stm32f4discovery.cfg -c "init; reset halt; flash write_image erase $(RESULT).bin 0x08000000; reset run; shutdown"

clean:
	rm -f $(RESULT).elf
	rm -f $(RESULT).bin
	rm -f $(RESULT).map
	rm -f $(RESULT).hex
	rm -f $(RESULT).lst
	rm -f $(OBJ_DIR)/*.o
