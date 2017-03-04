/**
 * @file main.c
 * @brief Main routine
 *
 * @section License
 *
 * Copyright (C) 2010-2016 Oryx Embedded SARL. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.2
 **/

//Dependencies
#include <stdlib.h>
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "pic32_usb_starter_kit_2.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mrf24wg_driver.h"
#include "dhcp/dhcp_client.h"
#include "dhcp/dhcp_server.h"
#include "ipv6/slaac.h"
#include "ipv6/ndp_router_adv.h"
#include "snmp/snmp_agent.h"
#include "mibs/mib2_module.h"
#include "mibs/mib2_impl.h"
#include "oid.h"
#include "private_mib_module.h"
#include "private_mib_impl.h"
#include "debug.h"

#ifndef _CP0_SET_CONFIG
    #define _CP0_SET_CONFIG(val) _mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, val)
#endif

//PIC32 onfiguration settings
#pragma config FSRSSEL = PRIORITY_7
#pragma config FMIIEN = OFF
#pragma config FETHIO = OFF
#pragma config FCANIO = ON
#pragma config FUSBIDIO = OFF
#pragma config FVBUSONIO = OFF
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config UPLLIDIV = DIV_1
#pragma config UPLLEN = OFF
#pragma config FPLLODIV = DIV_1
#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = ON
#pragma config IESO = ON
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_2
#pragma config FCKSM = CSDCMD
#pragma config WDTPS = PS1048576
#pragma config FWDTEN = OFF
//#pragma config DEBUG = ON
#pragma config ICESEL = ICS_PGx2
#pragma config PWP = OFF
#pragma config BWP = OFF
#pragma config CP = OFF

//Application configuration
#define APP_MAC_ADDR "00-00-00-00-00-00"

#define APP_USE_DHCP_CLIENT DISABLED
#define APP_USE_DHCP_SERVER ENABLED
#define APP_IPV4_HOST_ADDR "192.168.8.1"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "0.0.0.0"
#define APP_IPV4_PRIMARY_DNS "0.0.0.0"
#define APP_IPV4_SECONDARY_DNS "0.0.0.0"
#define APP_IPV4_ADDR_RANGE_MIN "192.168.8.10"
#define APP_IPV4_ADDR_RANGE_MAX "192.168.8.99"

#define APP_USE_SLAAC DISABLED
#define APP_USE_ROUTER_ADV ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::795"
#define APP_IPV6_PREFIX "fd00:1:2:3::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "fd00:1:2:3::795"

#define APP_SNMP_ENTERPRISE_OID "1.3.6.1.4.1.8072.9999.9999"
#define APP_SNMP_CONTEXT_ENGINE "\x80\x00\x00\x00\x01\x02\x03\x04"
#define APP_SNMP_TRAP_DEST_IP_ADDR "192.168.0.100"

//Global variables
bool_t ledState = FALSE;
systime_t ledTime = 0;
DhcpState dhcpPrevState = DHCP_STATE_INIT;

DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
DhcpServerSettings dhcpServerSettings;
DhcpServerContext dhcpServerContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
NdpRouterAdvSettings ndpRouterAdvSettings;
NdpRouterAdvPrefixInfo ndpRouterAdvPrefixInfo[1];
NdpRouterAdvContext ndpRouterAdvContext;
SnmpAgentSettings snmpAgentSettings;
SnmpAgentContext snmpAgentContext;

//Forward declaration of functions
void dhcpClientStateChangeCallback(DhcpClientCtx *context,
   NetInterface *interface, DhcpState state);

error_t snmpAgentRandCallback(uint8_t *data, size_t length);


/**
 * @brief System initialization
 **/

void systemInit(void)
{
   uint32_t temp;

   //Execute system unlock sequence
   SYSKEY = 0xAA996655;
   SYSKEY = 0x556699AA;

   //Configure PBCLK clock divisor (SYSCLK / 2);
   temp = OSCCON & ~_OSCCON_PBDIV_MASK;
   OSCCON = temp | (1 << _OSCCON_PBDIV_POSITION);

   //Configure RAM wait states (0)
   BMXCONCLR = _BMXCON_BMXWSDRM_MASK;

   //Configure FLASH wait states (2)
   temp = CHECON & ~_CHECON_PFMWS_MASK;
   CHECON = temp | (2 << _CHECON_PFMWS_POSITION);

   //Enable predictive prefetch for both cacheable and non-cacheable regions
   temp = CHECON & ~_CHECON_PREFEN_MASK;
   CHECON = temp | (3 << _CHECON_PREFEN_POSITION);

   //Enable data caching (4 lines)
   temp = CHECON & ~_CHECON_DCSZ_MASK;
   CHECON = temp | (3 << _CHECON_DCSZ_POSITION);

   //Enable KSEG0 cache
   temp = _CP0_GET_CONFIG() & ~_CP0_CONFIG_K0_MASK;
   temp |= (3 << _CP0_CONFIG_K0_POSITION);
   _CP0_SET_CONFIG(temp);

   //Relock the SYSKEY
   SYSKEY = 0;

   //Disable interrupts
   __builtin_disable_interrupts();

   //Set IV
   _CP0_BIS_CAUSE(_CP0_CAUSE_IV_MASK);
   //Enable multi-vectored mode
   INTCONSET = _INTCON_MVEC_MASK;
}


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Disable analog inputs
   AD1PCFG = 0xFFFF;

   //Configure RD0, RD1 and RD2 as outputs
   TRISDCLR = LED1_MASK | LED2_MASK | LED3_MASK;
   LATDCLR = LED1_MASK | LED2_MASK | LED3_MASK;

   //Configure RD6, RD7 and RD13 as inputs
   TRISDSET = SW1_MASK | SW2_MASK | SW3_MASK;
   //Enable pull-ups on CN15 (RD6), CN16 (RD7) and CN19 (RD13)
   CNPUESET = _CNPUE_CNPUE15_MASK | _CNPUE_CNPUE16_MASK | _CNPUE_CNPUE19_MASK;
}


/**
 * @brief LED blinking task
 **/

void ledTask(void)
{
   //Check current time
   if((int32_t)(systemTicks - ledTime) > 0)
   {
      //Toggle LED state
      if(ledState == 0)
      {
         LATDSET = LED3_MASK;
         ledState = 1;
         ledTime = systemTicks + 100;
      }
      else
      {
         LATDCLR = LED3_MASK;
         ledState = 0;
         ledTime = systemTicks + 900;
      }
   }
}


/**
 * @brief Main entry point
 * @return Unused value
 **/

int_t main(void)
{
   error_t error;
   size_t oidLen;
   uint8_t oid[SNMP_MAX_OID_SIZE];
   NetInterface *interface;
   MacAddr macAddr;
#if (APP_USE_DHCP_CLIENT == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //System initialization
   systemInit();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("*** CycloneTCP SNMP Agent Demo ***\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: PIC32MX795F512L\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //Standard MIB-II initialization
   error = mib2Init();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize MIB!\r\n");
   }

   //Private MIB initialization
   error = privateMibInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize MIB!\r\n");
   }

   //TCP/IP stack initialization
   error = netInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
   }

   //Configure the first Ethernet interface
   interface = &netInterface[0];

   //Set interface name
   netSetInterfaceName(interface, "eth0");
   //Set host name
   netSetHostname(interface, "SNMPAgentDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &mrf24wgDriver);
   //Set host MAC address
   macStringToAddr(APP_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
   }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_USE_DHCP_CLIENT == ENABLED)
   //Get default settings
   dhcpClientGetDefaultSettings(&dhcpClientSettings);
   //Set the network interface to be configured by DHCP
   dhcpClientSettings.interface = interface;
   //Disable rapid commit option
   dhcpClientSettings.rapidCommit = FALSE;
   //FSM state change event
   dhcpClientSettings.stateChangeEvent = dhcpClientStateChangeCallback;

   //DHCP client initialization
   error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP client!\r\n");
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP client!\r\n");
   }
#else
   //Set IPv4 host address
   ipv4StringToAddr(APP_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif

#if(APP_USE_DHCP_SERVER == ENABLED)
   //Get default settings
   dhcpServerGetDefaultSettings(&dhcpServerSettings);
   //Set the network interface to be configured by DHCP
   dhcpServerSettings.interface = interface;
   //Lease time, in seconds, assigned to the DHCP clients
   dhcpServerSettings.leaseTime = 3600;

   //Lowest and highest IP addresses in the pool that are available
   //for dynamic address assignment
   ipv4StringToAddr(APP_IPV4_ADDR_RANGE_MIN, &dhcpServerSettings.ipAddrRangeMin);
   ipv4StringToAddr(APP_IPV4_ADDR_RANGE_MAX, &dhcpServerSettings.ipAddrRangeMax);

   //Subnet mask
   ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &dhcpServerSettings.subnetMask);
   //Default gateway
   ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &dhcpServerSettings.defaultGateway);
   //DNS servers
   ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &dhcpServerSettings.dnsServer[0]);
   ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &dhcpServerSettings.dnsServer[1]);

   //DHCP server initialization
   error = dhcpServerInit(&dhcpServerContext, &dhcpServerSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP server!\r\n");
   }

   //Start DHCP server
   error = dhcpServerStart(&dhcpServerContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP server!\r\n");
   }
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_USE_SLAAC == ENABLED)
   //Get default settings
   slaacGetDefaultSettings(&slaacSettings);
   //Set the network interface to be configured
   slaacSettings.interface = interface;

   //SLAAC initialization
   error = slaacInit(&slaacContext, &slaacSettings);
   //Failed to initialize SLAAC?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize SLAAC!\r\n");
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext);
   //Failed to start SLAAC process?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SLAAC!\r\n");
   }
#else
   //Set link-local address
   ipv6StringToAddr(APP_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);

   //Set IPv6 prefix
   ipv6StringToAddr(APP_IPV6_PREFIX, &ipv6Addr);
   ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IPV6_PREFIX_LENGTH);

   //Set global address
   ipv6StringToAddr(APP_IPV6_GLOBAL_ADDR, &ipv6Addr);
   ipv6SetGlobalAddr(interface, 0, &ipv6Addr);
#endif

#if (APP_USE_ROUTER_ADV == ENABLED)
   //List of IPv6 prefixes to be advertised
   ipv6StringToAddr(APP_IPV6_PREFIX, &ndpRouterAdvPrefixInfo[0].prefix);
   ndpRouterAdvPrefixInfo[0].length = APP_IPV6_PREFIX_LENGTH;
   ndpRouterAdvPrefixInfo[0].onLinkFlag = TRUE;
   ndpRouterAdvPrefixInfo[0].autonomousFlag = TRUE;
   ndpRouterAdvPrefixInfo[0].validLifetime = 3600;
   ndpRouterAdvPrefixInfo[0].preferredLifetime = 1800;

   //Get default settings
   ndpRouterAdvGetDefaultSettings(&ndpRouterAdvSettings);
   //Set the underlying network interface
   ndpRouterAdvSettings.interface = interface;
   //Maximum time interval between unsolicited Router Advertisements
   ndpRouterAdvSettings.maxRtrAdvInterval = 60000;
   //Minimum time interval between unsolicited Router Advertisements
   ndpRouterAdvSettings.minRtrAdvInterval = 20000;
   //Router lifetime
   ndpRouterAdvSettings.defaultLifetime = 0;
   //List of IPv6 prefixes
   ndpRouterAdvSettings.prefixList = ndpRouterAdvPrefixInfo;
   ndpRouterAdvSettings.prefixListLength = arraysize(ndpRouterAdvPrefixInfo);

   //RA service initialization
   error = ndpRouterAdvInit(&ndpRouterAdvContext, &ndpRouterAdvSettings);
   //Failed to initialize DHCPv6 client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize RA service!\r\n");
      //Exit immediately
      return error;
   }

   //Start RA service
   error = ndpRouterAdvStart(&ndpRouterAdvContext);
   //Failed to start RA service?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start RA service!\r\n");
      //Exit immediately
      return error;
   }
#endif
#endif

   //Get default settings
   snmpAgentGetDefaultSettings(&snmpAgentSettings);
   //Bind SNMP agent to the desired network interface
   snmpAgentSettings.interface = interface;
   //Minimum version accepted by the SNMP agent
   snmpAgentSettings.versionMin = SNMP_VERSION_1;
   //Maximum version accepted by the SNMP agent
   snmpAgentSettings.versionMax = SNMP_VERSION_2C;

#if (SNMP_V3_SUPPORT == ENABLED)
   //Maximum version accepted by the SNMP agent
   snmpAgentSettings.versionMax = SNMP_VERSION_3;
   //Random data generation callback function
   snmpAgentSettings.randCallback = snmpAgentRandCallback;
#endif

   //SNMP agent initialization
   error = snmpAgentInit(&snmpAgentContext, &snmpAgentSettings);
   //Failed to initialize SNMP agent?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize SNMP agent!\r\n");
   }

   //Load standard MIB-II
   snmpAgentLoadMib(&snmpAgentContext, &mib2Module);
   //Load private MIB
   snmpAgentLoadMib(&snmpAgentContext, &privateMibModule);

   //Convert enterprise OID from string representation
   oidFromString(APP_SNMP_ENTERPRISE_OID, oid, sizeof(oid), &oidLen);
   //Set enterprise OID
   snmpAgentSetEnterpriseOid(&snmpAgentContext, oid, oidLen);

   //Set read-only community string
   error = snmpAgentCreateCommunity(&snmpAgentContext, "public",
      SNMP_ACCESS_READ_ONLY);

   //Set read-write community string
   snmpAgentCreateCommunity(&snmpAgentContext, "private",
      SNMP_ACCESS_READ_WRITE);

#if (SNMP_V3_SUPPORT == ENABLED)
   //Set context engine identifier
   snmpAgentSetContextEngine(&snmpAgentContext,
      APP_SNMP_CONTEXT_ENGINE, sizeof(APP_SNMP_CONTEXT_ENGINE) - 1);

   //Add a new user
   snmpAgentCreateUser(&snmpAgentContext, "usr-md5-none",
      SNMP_ACCESS_READ_WRITE, SNMP_KEY_FORMAT_TEXT,
      SNMP_AUTH_PROTOCOL_MD5, "authkey1",
      SNMP_PRIV_PROTOCOL_NONE, "");

   //Add a new user
   snmpAgentCreateUser(&snmpAgentContext, "usr-md5-des",
      SNMP_ACCESS_READ_WRITE, SNMP_KEY_FORMAT_TEXT,
      SNMP_AUTH_PROTOCOL_MD5, "authkey1",
      SNMP_PRIV_PROTOCOL_DES, "privkey1");
#endif

   //Start SNMP agent
   error = snmpAgentStart(&snmpAgentContext);
   //Failed to start SNMP agent?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SNMP agent!\r\n");
   }

   //Endless loop
   while(1)
   {
      //Process Wi-Fi events
      WF_Task();
      //Handle TCP/IP events
      netTask();
      //Handle SNMP agent events
      snmpAgentTask(&snmpAgentContext);
      //LED blinking task
      ledTask();
   }
}


/**
 * @brief FSM state change callback
 * @param[in] context Pointer to the DHCP client context
 * @param[in] interface interface Underlying network interface
 * @param[in] state New DHCP state
 **/

void dhcpClientStateChangeCallback(DhcpClientCtx *context,
   NetInterface *interface, DhcpState state)
{
   error_t error;
   IpAddr destIpAddr;
   SnmpTrapObject trapObjects[2];

   //DHCP process complete?
   if(state == DHCP_STATE_BOUND && dhcpPrevState == DHCP_STATE_PROBING)
   {
      //Destination IP address
      ipStringToAddr(APP_SNMP_TRAP_DEST_IP_ADDR, &destIpAddr);

      //Add the ifDescr.1 object to the variable binding list of the message
      oidFromString("1.3.6.1.2.1.2.2.1.2.1", trapObjects[0].oid,
         SNMP_MAX_OID_SIZE, &trapObjects[0].oidLen);

      //Add the ifPhysAddress.1 object to the variable binding list of the message
      oidFromString("1.3.6.1.2.1.2.2.1.6.1", trapObjects[1].oid,
         SNMP_MAX_OID_SIZE, &trapObjects[1].oidLen);

      //Send a SNMP trap
      error = snmpAgentSendTrap(&snmpAgentContext, &destIpAddr, SNMP_VERSION_2C,
         "public", SNMP_TRAP_LINK_UP, 0, trapObjects, 2);

      //Failed to send trap message?
      if(error)
      {
         //Debug message
         TRACE_ERROR("Failed to send SNMP trap message!\r\n");
      }
   }

   //Save current state
   dhcpPrevState = state;
}


/**
 * @brief Random data generation callback function
 * @param[out] data Buffer where to store the random data
 * @param[in] lenght Number of bytes that are required
 * @return Error code
 **/

error_t snmpAgentRandCallback(uint8_t *data, size_t length)
{
   size_t i;

   //Generate some random data
   for(i = 0; i < length; i++)
      data[i] = rand();

   //No error to report
   return NO_ERROR;
}
