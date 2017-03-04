RESULT ?= snmp_agent_demo

DEFINES = 

INCLUDES += \
	-I../src \
	-I../../../../common \
	-I../../../../cyclone_tcp \
	-I../../../../cyclone_ssl \
	-I../../../../cyclone_crypto

SOURCES = \
	../src/main.c \
	../src/private_mib_module.c \
	../src/private_mib_impl.c \
	../../../../common/endian.c \
	../../../../common/os_port_none.c \
	../../../../common/date_time.c \
	../../../../common/str.c \
	../../../../common/debug.c \
	../../../../cyclone_tcp/core/net.c \
	../../../../cyclone_tcp/core/net_mem.c \
	../../../../cyclone_tcp/drivers/pcap_driver.c \
	../../../../cyclone_tcp/core/nic.c \
	../../../../cyclone_tcp/core/ethernet.c \
	../../../../cyclone_tcp/ipv4/arp.c \
	../../../../cyclone_tcp/ipv4/ipv4.c \
	../../../../cyclone_tcp/ipv4/ipv4_frag.c \
	../../../../cyclone_tcp/ipv4/icmp.c \
	../../../../cyclone_tcp/ipv4/igmp.c \
	../../../../cyclone_tcp/ipv6/ipv6.c \
	../../../../cyclone_tcp/ipv6/ipv6_frag.c \
	../../../../cyclone_tcp/ipv6/ipv6_misc.c \
	../../../../cyclone_tcp/ipv6/ipv6_pmtu.c \
	../../../../cyclone_tcp/ipv6/icmpv6.c \
	../../../../cyclone_tcp/ipv6/mld.c \
	../../../../cyclone_tcp/ipv6/ndp.c \
	../../../../cyclone_tcp/ipv6/ndp_cache.c \
	../../../../cyclone_tcp/ipv6/ndp_misc.c \
	../../../../cyclone_tcp/ipv6/slaac.c \
	../../../../cyclone_tcp/core/ip.c \
	../../../../cyclone_tcp/core/tcp.c \
	../../../../cyclone_tcp/core/tcp_fsm.c \
	../../../../cyclone_tcp/core/tcp_misc.c \
	../../../../cyclone_tcp/core/tcp_timer.c \
	../../../../cyclone_tcp/core/udp.c \
	../../../../cyclone_tcp/core/socket.c \
	../../../../cyclone_tcp/core/raw_socket.c \
	../../../../cyclone_tcp/dns/dns_cache.c \
	../../../../cyclone_tcp/dns/dns_client.c \
	../../../../cyclone_tcp/dns/dns_common.c \
	../../../../cyclone_tcp/dns/dns_debug.c \
	../../../../cyclone_tcp/mdns/mdns_client.c \
	../../../../cyclone_tcp/mdns/mdns_responder.c \
	../../../../cyclone_tcp/mdns/mdns_common.c \
	../../../../cyclone_tcp/netbios/nbns_client.c \
	../../../../cyclone_tcp/netbios/nbns_responder.c \
	../../../../cyclone_tcp/netbios/nbns_common.c \
	../../../../cyclone_tcp/dhcp/dhcp_client.c \
	../../../../cyclone_tcp/dhcp/dhcp_common.c \
	../../../../cyclone_tcp/dhcp/dhcp_debug.c \
	../../../../cyclone_tcp/snmp/snmp_agent.c \
	../../../../cyclone_tcp/snmp/snmp_agent_dispatch.c \
	../../../../cyclone_tcp/snmp/snmp_agent_pdu.c \
	../../../../cyclone_tcp/snmp/snmp_agent_misc.c \
	../../../../cyclone_tcp/snmp/snmp_common.c \
	../../../../cyclone_tcp/snmp/snmp_usm.c \
	../../../../cyclone_tcp/mibs/mib2_module.c \
	../../../../cyclone_tcp/mibs/mib2_impl.c \
	../../../../cyclone_crypto/md5.c \
	../../../../cyclone_crypto/sha1.c \
	../../../../cyclone_crypto/hmac.c \
	../../../../cyclone_crypto/des.c \
	../../../../cyclone_crypto/aes.c \
	../../../../cyclone_crypto/cipher_mode_cbc.c \
	../../../../cyclone_crypto/cipher_mode_cfb.c \
	../../../../cyclone_crypto/asn1.c \
	../../../../cyclone_crypto/oid.c

HEADERS = \
	../src/os_port_config.h \
	../src/net_config.h \
	../src/crypto_config.h \
	../src/private_mib_module.h \
	../src/private_mib_impl.h \
	../../../../common/endian.h \
	../../../../common/os_port.h \
	../../../../common/os_port_none.h \
	../../../../common/date_time.h \
	../../../../common/str.h \
	../../../../common/error.h \
	../../../../common/debug.h \
	../../../../cyclone_tcp/core/net.h \
	../../../../cyclone_tcp/core/net_mem.h \
	../../../../cyclone_tcp/drivers/pcap_driver.h \
	../../../../cyclone_tcp/core/nic.h \
	../../../../cyclone_tcp/core/ethernet.h \
	../../../../cyclone_tcp/ipv4/arp.h \
	../../../../cyclone_tcp/ipv4/ipv4.h \
	../../../../cyclone_tcp/ipv4/ipv4_frag.h \
	../../../../cyclone_tcp/ipv4/icmp.h \
	../../../../cyclone_tcp/ipv4/igmp.h \
	../../../../cyclone_tcp/ipv6/ipv6.h \
	../../../../cyclone_tcp/ipv6/ipv6_frag.h \
	../../../../cyclone_tcp/ipv6/ipv6_misc.h \
	../../../../cyclone_tcp/ipv6/ipv6_pmtu.h \
	../../../../cyclone_tcp/ipv6/icmpv6.h \
	../../../../cyclone_tcp/ipv6/mld.h \
	../../../../cyclone_tcp/ipv6/ndp.h \
	../../../../cyclone_tcp/ipv6/ndp_cache.h \
	../../../../cyclone_tcp/ipv6/ndp_misc.h \
	../../../../cyclone_tcp/ipv6/slaac.h \
	../../../../cyclone_tcp/core/ip.h \
	../../../../cyclone_tcp/core/tcp.h \
	../../../../cyclone_tcp/core/tcp_fsm.h \
	../../../../cyclone_tcp/core/tcp_misc.h \
	../../../../cyclone_tcp/core/tcp_timer.h \
	../../../../cyclone_tcp/core/udp.h \
	../../../../cyclone_tcp/core/socket.h \
	../../../../cyclone_tcp/core/bsd_socket.h \
	../../../../cyclone_tcp/core/raw_socket.h \
	../../../../cyclone_tcp/dns/dns_cache.h \
	../../../../cyclone_tcp/dns/dns_client.h \
	../../../../cyclone_tcp/dns/dns_common.h \
	../../../../cyclone_tcp/dns/dns_debug.h \
	../../../../cyclone_tcp/mdns/mdns_client.h \
	../../../../cyclone_tcp/mdns/mdns_responder.h \
	../../../../cyclone_tcp/mdns/mdns_common.h \
	../../../../cyclone_tcp/netbios/nbns_client.h \
	../../../../cyclone_tcp/netbios/nbns_responder.h \
	../../../../cyclone_tcp/netbios/nbns_common.h \
	../../../../cyclone_tcp/dhcp/dhcp_client.h \
	../../../../cyclone_tcp/dhcp/dhcp_common.h \
	../../../../cyclone_tcp/dhcp/dhcp_debug.h \
	../../../../cyclone_tcp/snmp/snmp_agent.h \
	../../../../cyclone_tcp/snmp/snmp_agent_dispatch.h \
	../../../../cyclone_tcp/snmp/snmp_agent_pdu.h \
	../../../../cyclone_tcp/snmp/snmp_agent_misc.h \
	../../../../cyclone_tcp/snmp/snmp_common.h \
	../../../../cyclone_tcp/snmp/snmp_usm.h \
	../../../../cyclone_tcp/mibs/mib_common.h \
	../../../../cyclone_tcp/mibs/mib2_module.h \
	../../../../cyclone_tcp/mibs/mib2_impl.h \
	../../../../cyclone_crypto/crypto.h \
	../../../../cyclone_crypto/md5.h \
	../../../../cyclone_crypto/sha1.h \
	../../../../cyclone_crypto/hmac.h \
	../../../../cyclone_crypto/des.h \
	../../../../cyclone_crypto/aes.h \
	../../../../cyclone_crypto/cipher_mode_cbc.h \
	../../../../cyclone_crypto/cipher_mode_cfb.h \
	../../../../cyclone_crypto/asn1.h \
	../../../../cyclone_crypto/oid.h

LIBS = -lpcap

OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

OBJ_DIR = obj

CFLAGS += -Wall
CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)

CC = gcc
LD = ld
OBJDUMP = objdump
OBJCOPY = objcopy
SIZE = size

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))

all: build

build: $(RESULT)

$(RESULT): $(OBJECTS) $(HEADERS) $(THIS_MAKEFILE)
	$(CC) -Wl,-M=$(RESULT).map $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(notdir $(OBJECTS))) $(LIBS) -o $@

$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@

%.o: %.c $(HEADERS) $(THIS_MAKEFILE)
	$(CC) $(CFLAGS) -c $< -o $(addprefix $(OBJ_DIR)/, $(notdir $@))

clean:
	rm -f $(RESULT)
	rm -f $(RESULT).map
	rm -f $(OBJ_DIR)/*.o
