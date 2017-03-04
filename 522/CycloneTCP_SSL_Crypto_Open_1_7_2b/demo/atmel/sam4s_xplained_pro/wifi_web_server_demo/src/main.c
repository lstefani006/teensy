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
#include "sam4s.h"
#include "sam4s_xplained_pro.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/wilc1000_driver.h"
#include "dhcp/dhcp_client.h"
#include "dhcp/dhcp_server.h"
#include "ipv6/slaac.h"
#include "ipv6/ndp_router_adv.h"
#include "http/http_server.h"
#include "http/mime.h"
#include "str.h"
#include "path.h"
#include "date_time.h"
#include "resource_manager.h"
#include "debug.h"

//First Wi-Fi interface (STA mode) configuration
#define APP_IF0_MAC_ADDR "00-AB-CD-EF-15-00"
#define APP_IF0_USE_DHCP_CLIENT ENABLED
#define APP_IF0_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IF0_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF0_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IF0_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IF0_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_IF0_USE_SLAAC ENABLED
#define APP_IF0_IPV6_LINK_LOCAL_ADDR "fe80::432:0"

//Second Wi-Fi interface (AP mode) configuration
#define APP_IF1_MAC_ADDR "00-AB-CD-EF-15-00"
#define APP_IF1_USE_DHCP_SERVER ENABLED
#define APP_IF1_IPV4_HOST_ADDR "192.168.8.1"
#define APP_IF1_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF1_IPV4_DEFAULT_GATEWAY "0.0.0.0"
#define APP_IF1_IPV4_PRIMARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_SECONDARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_ADDR_RANGE_MIN "192.168.8.10"
#define APP_IF1_IPV4_ADDR_RANGE_MAX "192.168.8.99"

#define APP_IF1_USE_ROUTER_ADV ENABLED
#define APP_IF1_IPV6_LINK_LOCAL_ADDR "fe80::432:1"
#define APP_IF1_IPV6_PREFIX "fd00:1:2:3::"
#define APP_IF1_IPV6_PREFIX_LENGTH 64
#define APP_IF1_IPV6_GLOBAL_ADDR "fd00:1:2:3::432:1"

#define APP_HTTP_MAX_CONNECTIONS 4

//Global variables
DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
DhcpServerSettings dhcpServerSettings;
DhcpServerContext dhcpServerContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
NdpRouterAdvSettings ndpRouterAdvSettings;
NdpRouterAdvPrefixInfo ndpRouterAdvPrefixInfo[1];
NdpRouterAdvContext ndpRouterAdvContext;
HttpServerSettings httpServerSettings;
HttpServerContext httpServerContext;
HttpConnection httpConnections[APP_HTTP_MAX_CONNECTIONS];

//Forward declaration of functions
error_t wifiStaInterfaceInit(void);
error_t wifiApInterfaceInit(void);

error_t httpServerCgiCallback(HttpConnection *connection,
   const char_t *param);

error_t httpServerUriNotFoundCallback(HttpConnection *connection,
   const char_t *uri);


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Enable PIO peripheral clocks
   PMC->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOC);

   //Configure LED0
   PIO_LED0->PIO_PER = LED0;
   PIO_LED0->PIO_OER = LED0;
   PIO_LED0->PIO_SODR = LED0;

   //Configure SW0 button
   PIO_SW0->PIO_PER = SW0;
   PIO_SW0->PIO_ODR = SW0;
   PIO_SW0->PIO_PUER = SW0;
}


/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      PIO_LED0->PIO_CODR = LED0;
      osDelayTask(100);
      PIO_LED0->PIO_SODR = LED0;
      osDelayTask(900);
   }
}


/**
 * @brief Main entry point
 * @return Unused value
 **/

int_t main(void)
{
   error_t error;
   OsTask *task;

   //Update system core clock
   SystemCoreClockUpdate();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("*****************************************\r\n");
   TRACE_INFO("*** CycloneTCP Wi-Fi Web Server Demo ***\r\n");
   TRACE_INFO("****************************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: SAM4S\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //TCP/IP stack initialization
   error = netInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
   }

   //Configure the first network interface (Wi-Fi STA mode)
   wifiStaInterfaceInit();

   //Configure the second network interface (Wi-Fi AP mode)
   wifiApInterfaceInit();

   //Get default settings
   httpServerGetDefaultSettings(&httpServerSettings);
   //Bind HTTP server to the desired interface
   httpServerSettings.interface = NULL;
   //Listen to port 80
   httpServerSettings.port = HTTP_PORT;
   //Client connections
   httpServerSettings.maxConnections = APP_HTTP_MAX_CONNECTIONS;
   httpServerSettings.connections = httpConnections;
   //Specify the server's root directory
   strcpy(httpServerSettings.rootDirectory, "/www/");
   //Set default home page
   strcpy(httpServerSettings.defaultDocument, "index.shtm");
   //Callback functions
   httpServerSettings.cgiCallback = httpServerCgiCallback;
   httpServerSettings.uriNotFoundCallback = httpServerUriNotFoundCallback;

   //HTTP server initialization
   error = httpServerInit(&httpServerContext, &httpServerSettings);
   //Failed to initialize HTTP server?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize HTTP server!\r\n");
   }

   //Start HTTP server
   error = httpServerStart(&httpServerContext);
   //Failed to start HTTP server?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start HTTP server!\r\n");
   }

   //Create a task to blink the LED
   task = osCreateTask("Blink", blinkTask, NULL, 500, 1);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Start the execution of tasks
   osStartKernel();

   //This function should never return
   return 0;
}


/**
 * @brief Wi-Fi STA interface initialization
 **/

error_t wifiStaInterfaceInit(void)
{
   error_t error;
   NetInterface *interface;
   MacAddr macAddr;
#if (APP_IF0_USE_DHCP_CLIENT == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_IF0_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Configure the first network interface (Wi-Fi STA mode)
   interface = &netInterface[0];

   //Set interface name
   netSetInterfaceName(interface, "wlan0");
   //Set host name
   netSetHostname(interface, "WebServerDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &wilc1000StaDriver);
   //Set host MAC address
   macStringToAddr(APP_IF0_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
      //Exit immediately
      return error;
   }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_IF0_USE_DHCP_CLIENT == ENABLED)
   //Get default settings
   dhcpClientGetDefaultSettings(&dhcpClientSettings);
   //Set the network interface to be configured by DHCP
   dhcpClientSettings.interface = interface;
   //Disable rapid commit option
   dhcpClientSettings.rapidCommit = FALSE;

   //DHCP client initialization
   error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP client!\r\n");
      //Exit immediately
      return error;
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP client!\r\n");
      //Exit immediately
      return error;
   }
#else
   //Set IPv4 host address
   ipv4StringToAddr(APP_IF0_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IF0_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IF0_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IF0_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IF0_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_IF0_USE_SLAAC == ENABLED)
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
      //Exit immediately
      return error;
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext);
   //Failed to start SLAAC process?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SLAAC!\r\n");
      //Exit immediately
      return error;
   }
#else
   //Set link-local address
   ipv6StringToAddr(APP_IF0_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);
#endif
#endif

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief Wi-Fi AP interface initialization
 **/

error_t wifiApInterfaceInit(void)
{
   error_t error;
   NetInterface *interface;
   MacAddr macAddr;
   Ipv4Addr ipv4Addr;
   Ipv6Addr ipv6Addr;

   //Configure the second network interface (Wi-Fi AP mode)
   interface = &netInterface[1];

   //Set interface name
   netSetInterfaceName(interface, "wlan1");
   //Set host name
   netSetHostname(interface, "WebServerDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &wilc1000ApDriver);
   //Set host MAC address
   macStringToAddr(APP_IF1_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
      //Exit immediately
      return error;
   }

#if(IPV4_SUPPORT == ENABLED)
   //Set IPv4 host address
   ipv4StringToAddr(APP_IF1_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);

#if (APP_IF1_USE_DHCP_SERVER == ENABLED)
   //Get default settings
   dhcpServerGetDefaultSettings(&dhcpServerSettings);
   //Set the network interface to be configured by DHCP
   dhcpServerSettings.interface = interface;
   //Lease time, in seconds, assigned to the DHCP clients
   dhcpServerSettings.leaseTime = 3600;

   //Lowest and highest IP addresses in the pool that are available
   //for dynamic address assignment
   ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MIN, &dhcpServerSettings.ipAddrRangeMin);
   ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MAX, &dhcpServerSettings.ipAddrRangeMax);

   //Subnet mask
   ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &dhcpServerSettings.subnetMask);
   //Default gateway
   ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &dhcpServerSettings.defaultGateway);
   //DNS servers
   ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &dhcpServerSettings.dnsServer[0]);
   ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &dhcpServerSettings.dnsServer[1]);

   //DHCP server initialization
   error = dhcpServerInit(&dhcpServerContext, &dhcpServerSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP server!\r\n");
      //Exit immediately
      return error;
   }

   //Start DHCP server
   error = dhcpServerStart(&dhcpServerContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP server!\r\n");
      //Exit immediately
      return error;
   }
#endif
#endif

#if(IPV6_SUPPORT == ENABLED)
   //Set link-local address
   ipv6StringToAddr(APP_IF1_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);

   //Set IPv6 prefix
   ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ipv6Addr);
   ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IF1_IPV6_PREFIX_LENGTH);

   //Set global address
   ipv6StringToAddr(APP_IF1_IPV6_GLOBAL_ADDR, &ipv6Addr);
   ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

#if (APP_IF1_USE_ROUTER_ADV == ENABLED)
   //List of IPv6 prefixes to be advertised
   ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ndpRouterAdvPrefixInfo[0].prefix);
   ndpRouterAdvPrefixInfo[0].length = APP_IF1_IPV6_PREFIX_LENGTH;
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

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief CGI callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] param NULL-terminated string that contains the CGI parameter
 * @return Error code
 **/

error_t httpServerCgiCallback(HttpConnection *connection,
   const char_t *param)
{
   static uint_t pageCounter = 0;
   uint_t length;
   MacAddr macAddr;
#if (IPV4_SUPPORT == ENABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (IPV6_SUPPORT == ENABLED)
   uint_t n;
   Ipv6Addr ipv6Addr;
#endif

   //Underlying network interface
   NetInterface *interface = connection->socket->interface;

   //Check parameter name
   if(!strcasecmp(param, "PAGE_COUNTER"))
   {
      pageCounter++;
      sprintf(connection->buffer, "%u time%s", pageCounter, (pageCounter >= 2) ? "s" : "");
   }
   else if(!strcasecmp(param, "BOARD_NAME"))
   {
      strcpy(connection->buffer, "SAM4S-Xplained-Pro");
   }
   else if(!strcasecmp(param, "SYSTEM_TIME"))
   {
      systime_t time = osGetSystemTime();
      formatSystemTime(time, connection->buffer);
   }
   else if(!strcasecmp(param, "MAC_ADDR"))
   {
      netGetMacAddr(interface, &macAddr);
      macAddrToString(&macAddr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV4_ADDR"))
   {
      ipv4GetHostAddr(interface, &ipv4Addr);
      ipv4AddrToString(ipv4Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "SUBNET_MASK"))
   {
      ipv4GetSubnetMask(interface, &ipv4Addr);
      ipv4AddrToString(ipv4Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "DEFAULT_GATEWAY"))
   {
      ipv4GetDefaultGateway(interface, &ipv4Addr);
      ipv4AddrToString(ipv4Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV4_PRIMARY_DNS"))
   {
      ipv4GetDnsServer(interface, 0, &ipv4Addr);
      ipv4AddrToString(ipv4Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV4_SECONDARY_DNS"))
   {
      ipv4GetDnsServer(interface, 1, &ipv4Addr);
      ipv4AddrToString(ipv4Addr, connection->buffer);
   }
#if (IPV6_SUPPORT == ENABLED)
   else if(!strcasecmp(param, "LINK_LOCAL_ADDR"))
   {
      ipv6GetLinkLocalAddr(interface, &ipv6Addr);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "GLOBAL_ADDR"))
   {
      ipv6GetGlobalAddr(interface, 0, &ipv6Addr);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV6_PREFIX"))
   {
      ipv6GetPrefix(interface, 0, &ipv6Addr, &n);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
      length = strlen(connection->buffer);
      sprintf(connection->buffer + length, "/%u", n);
   }
   else if(!strcasecmp(param, "ROUTER"))
   {
      ipv6GetDefaultRouter(interface, 0, &ipv6Addr);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV6_PRIMARY_DNS"))
   {
      ipv6GetDnsServer(interface, 0, &ipv6Addr);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
   }
   else if(!strcasecmp(param, "IPV6_SECONDARY_DNS"))
   {
      ipv6GetDnsServer(interface, 1, &ipv6Addr);
      ipv6AddrToString(&ipv6Addr, connection->buffer);
   }
#endif
   else
   {
      return ERROR_INVALID_TAG;
   }

   //Get the length of the resulting string
   length = strlen(connection->buffer);

   //Send the contents of the specified environment variable
   return httpWriteStream(connection, connection->buffer, length);
}


/**
 * @brief URI not found callback
 * @param[in] connection Handle referencing a client connection
 * @param[in] uri NULL-terminated string containing the path to the requested resource
 * @return Error code
 **/

error_t httpServerUriNotFoundCallback(HttpConnection *connection,
   const char_t *uri)
{
   return ERROR_NOT_FOUND;
}
