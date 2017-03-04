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
#include "stm32f2xx.h"
#include "stm322xg_eval.h"
#include "stm322xg_eval_ioe.h"
#include "stm322xg_eval_lcd.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/stm32f2x7_eth.h"
#include "drivers/dp83848.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "snmp/snmp_agent.h"
#include "mibs/mib2_module.h"
#include "mibs/mib2_impl.h"
#include "oid.h"
#include "private_mib_module.h"
#include "private_mib_impl.h"
#include "ext_int_driver.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-02-07"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::207"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::207"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_SNMP_ENTERPRISE_OID "1.3.6.1.4.1.8072.9999.9999"
#define APP_SNMP_CONTEXT_ENGINE "\x80\x00\x00\x00\x01\x02\x03\x04"
#define APP_SNMP_TRAP_DEST_IP_ADDR "192.168.0.100"

//Global variables
uint_t lcdLine = 0;
uint_t lcdColumn = 0;
bool_t ledState = FALSE;
systime_t ledTime = 0;
DhcpState dhcpPrevState = DHCP_STATE_INIT;

Ipv4Addr ipv4HostAddr;
Ipv6Addr ipv6LinkLocalAddr;
Ipv6Addr ipv6GlobalAddr;

DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
SnmpAgentSettings snmpAgentSettings;
SnmpAgentContext snmpAgentContext;

//Forward declaration of functions
void dhcpClientStateChangeCallback(DhcpClientCtx *context,
   NetInterface *interface, DhcpState state);

error_t snmpAgentRandCallback(uint8_t *data, size_t length);


/**
 * @brief Set cursor location
 * @param[in] line Line number
 * @param[in] column Column number
 **/

void lcdSetCursor(uint_t line, uint_t column)
{
   lcdLine = MIN(line, 10);
   lcdColumn = MIN(column, 20);
}


/**
 * @brief Write a character to the LCD display
 * @param[in] c Character to be written
 **/

void lcdPutChar(char_t c)
{
   if(c == '\r')
   {
      lcdColumn = 0;
   }
   else if(c == '\n')
   {
      lcdColumn = 0;
      lcdLine++;
   }
   else if(lcdLine < 10 && lcdColumn < 20)
   {
      //Display current character
      LCD_DisplayChar(lcdLine * 24, 319 - (lcdColumn * 16), c);

      //Advance the cursor position
      if(++lcdColumn >= 20)
      {
         lcdColumn = 0;
         lcdLine++;
      }
   }
}


/**
 * @brief LCD display initialization
 **/

void lcdInit(void)
{
   //Initialize LCD display
   STM322xG_LCD_Init();
   LCD_SetBackColor(Blue);
   LCD_SetTextColor(White);
   LCD_SetFont(&Font16x24);
   LCD_Clear(Blue);

   //Refresh display
   lcdSetCursor(0, 0);
   printf("SNMP Agent Demo\r\n");
   lcdSetCursor(1, 0);
   printf("IPv4 Addr\r\n");
   printf("0.0.0.0\r\n");
   lcdSetCursor(4, 0);
   printf("IPv6 Link-Local Addr\r\n");
   lcdSetCursor(7, 0);
   printf("IPv6 Global Addr\r\n");

   //Initialize variables
   ipv4HostAddr = IPV4_UNSPECIFIED_ADDR;
   ipv6LinkLocalAddr = IPV6_UNSPECIFIED_ADDR;
   ipv6GlobalAddr = IPV6_UNSPECIFIED_ADDR;
}


/**
 * @brief LCD display task
 **/

void lcdTask(void)
{
   NetInterface *interface;
#if (IPV4_SUPPORT == ENABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (IPV6_SUPPORT == ENABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Point to the network interface
   interface = &netInterface[0];

#if (IPV4_SUPPORT == ENABLED)
   //Retrieve IPv4 host address
   ipv4GetHostAddr(interface, &ipv4Addr);

   //Check for any changes
   if(ipv4Addr != ipv4HostAddr)
   {
      //Save IPv4 host address
      ipv4HostAddr = ipv4Addr;

      //Refresh LCD display
      lcdSetCursor(2, 0);
      printf("%-16s\r\n", ipv4AddrToString(ipv4HostAddr, NULL));
   }
#endif

#if (IPV6_SUPPORT == ENABLED)
   //Retrieve IPv6 link-local address
   ipv6GetLinkLocalAddr(interface, &ipv6Addr);

   //Check for any changes
   if(!ipv6CompAddr(&ipv6Addr, &ipv6LinkLocalAddr))
   {
      //Save IPv6 link-local address
      ipv6LinkLocalAddr = ipv6Addr;

      //Refresh LCD display
      lcdSetCursor(5, 0);
      printf("%-40s\r\n", ipv6AddrToString(&ipv6LinkLocalAddr, NULL));
   }

   //Retrieve IPv6 global address
   ipv6GetGlobalAddr(interface, 0, &ipv6Addr);

   //Check for any changes
   if(!ipv6CompAddr(&ipv6Addr, &ipv6GlobalAddr))
   {
      //Save IPv6 global address
      ipv6GlobalAddr = ipv6Addr;

      //Refresh LCD display
      lcdSetCursor(8, 0);
      printf("%-40s\r\n", ipv6AddrToString(&ipv6GlobalAddr, NULL));
   }
#endif
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
         STM_EVAL_LEDOn(LED4);
         ledState = 1;
         ledTime = systemTicks + 100;
      }
      else
      {
         STM_EVAL_LEDOff(LED4);
         ledState = 0;
         ledTime = systemTicks + 900;
      }
   }
}


/**
 * @brief SysTick handler
 **/

void SysTick_Handler(void)
{
   systemTicks++;
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
#if (APP_USE_DHCP == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

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
   TRACE_INFO("Target: STM32F207\r\n");
   TRACE_INFO("\r\n");

   //LED configuration
   STM_EVAL_LEDInit(LED1);
   STM_EVAL_LEDInit(LED2);
   STM_EVAL_LEDInit(LED3);
   STM_EVAL_LEDInit(LED4);

   //Clear LEDs
   STM_EVAL_LEDOff(LED1);
   STM_EVAL_LEDOff(LED2);
   STM_EVAL_LEDOff(LED3);
   STM_EVAL_LEDOff(LED4);

   //Initialize I/O expander
   IOE_Config();
   //Initialize user button
   STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);

   //Initialize LCD display
   lcdInit();

   //Enable RNG peripheral clock
   RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
   //Enable RNG
   RNG_Cmd(ENABLE);

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
   netSetDriver(interface, &stm32f2x7EthDriver);
   netSetPhyDriver(interface, &dp83848PhyDriver);
   //Set external interrupt line driver
   netSetExtIntDriver(interface, &extIntDriver);
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
#if (APP_USE_DHCP == ENABLED)
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

   //Set default router
   ipv6StringToAddr(APP_IPV6_ROUTER, &ipv6Addr);
   ipv6SetDefaultRouter(interface, 0, &ipv6Addr);

   //Set primary and secondary DNS servers
   ipv6StringToAddr(APP_IPV6_PRIMARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 0, &ipv6Addr);
   ipv6StringToAddr(APP_IPV6_SECONDARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 1, &ipv6Addr);
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

   //Configure SysTick
   SysTick_Config(SystemCoreClock / 1000);

   //Endless loop
   while(1)
   {
      //Handle TCP/IP events
      netTask();
      //Handle SNMP agent events
      snmpAgentTask(&snmpAgentContext);
      //Refresh LCD display
      lcdTask();
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
   uint32_t value;

   //Generate some random data
   for(i = 0; i < length; i++)
   {
      //Check whether a new 32-bit random integer must be generated
      if((i % 4) == 0)
      {
         //Wait for the RNG to contain a valid data
         while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);

         //Get 32-bit random value
         value = RNG_GetRandomNumber();
      }

      //Copy random data
      data[i] = value & 0xFF;
      //Shift the value to the right
      value >>= 8;
   }

   //No error to report
   return NO_ERROR;
}
