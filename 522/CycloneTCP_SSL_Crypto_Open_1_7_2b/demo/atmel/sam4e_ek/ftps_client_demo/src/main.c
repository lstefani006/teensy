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
#include "sam4e.h"
#include "sam4e_ek.h"
#include "lcd.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/sam4e_eth.h"
#include "drivers/ksz8051.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "ftp/ftp_client.h"
#include "tls.h"
#include "tls_cipher_suites.h"
#include "yarrow.h"
#include "ext_int_driver.h"
#include "error.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-04-16"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::416"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::416"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//Server hostname and port
#define APP_FTP_SERVER_NAME "test.rebex.net"
#define APP_FTP_IMPLICIT_TLS ENABLED
#define APP_FTP_EXPLICIT_TLS DISABLED
#define APP_FTP_LOGIN "demo"
#define APP_FTP_PASSWORD "password"
#define APP_FTP_FILENAME "readme.txt"

//Compilation options
#define APP_SET_CIPHER_SUITES DISABLED
#define APP_SET_SERVER_NAME DISABLED
#define APP_SET_TRUSTED_CA_LIST DISABLED

#if (APP_SET_CIPHER_SUITES == ENABLED)

//List of preferred ciphersuites
static const unsigned short cipherSuites[] =
{
   TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384,
   TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384,
   TLS_RSA_WITH_AES_256_GCM_SHA384,
   TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
   TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256,
   TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256,
   TLS_RSA_WITH_AES_128_GCM_SHA256,
   TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
   TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256,
   TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256,
   TLS_RSA_WITH_AES_256_CBC_SHA256,
   TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
   TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256,
   TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256,
   TLS_RSA_WITH_AES_128_CBC_SHA256,
   TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
   TLS_RSA_WITH_CAMELLIA_256_CBC_SHA,
   TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,
   TLS_RSA_WITH_AES_256_CBC_SHA,
   TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
   TLS_RSA_WITH_CAMELLIA_128_CBC_SHA,
   TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA,
   TLS_RSA_WITH_AES_128_CBC_SHA,
   TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
   TLS_RSA_WITH_SEED_CBC_SHA,
   TLS_DHE_RSA_WITH_SEED_CBC_SHA,
   TLS_RSA_WITH_3DES_EDE_CBC_SHA,
   TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA,
   TLS_RSA_WITH_RC4_128_SHA
};

#endif

//Global variables
uint_t lcdLine = 0;
uint_t lcdColumn = 0;

DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
YarrowContext yarrowContext;
uint8_t seed[32];


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
      GLCD_DisplayChar(lcdLine, lcdColumn, 1, c);

      //Advance the cursor position
      if(++lcdColumn >= 20)
      {
         lcdColumn = 0;
         lcdLine++;
      }
   }
}


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Enable PIO peripheral clocks
   PMC->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOD);

   //Configure LED1
   PIO_LED1->PIO_PER = LED1;
   PIO_LED1->PIO_OER = LED1;
   PIO_LED1->PIO_SODR = LED1;

   //Configure LED2
   PIO_LED2->PIO_PER = LED2;
   PIO_LED2->PIO_OER = LED2;
   PIO_LED2->PIO_SODR = LED2;

   //Configure LED3
   PIO_LED3->PIO_PER = LED3;
   PIO_LED3->PIO_OER = LED3;
   PIO_LED3->PIO_SODR = LED3;

   //Configure LED4
   PIO_LED4->PIO_PER = LED4;
   PIO_LED4->PIO_OER = LED4;
   PIO_LED4->PIO_SODR = LED4;

   //Configure up button
   PIO_BT_UP->PIO_PER = BT_UP;
   PIO_BT_UP->PIO_ODR = BT_UP;

   //Configure down button
   PIO_BT_DOWN->PIO_PER = BT_DOWN;
   PIO_BT_DOWN->PIO_ODR = BT_DOWN;
}


/**
 * @brief SSL/TLS initialization callback
 * @param[in] context Pointer to the FTP client context
 * @param[in] tlsContext Pointer to the SSL/TLS context
 **/

error_t ftpClientTlsInitCallback(FtpClientContext *context,
   TlsContext *tlsContext)
{
   error_t error;

   //Debug message
   TRACE_INFO("FTP Client: TLS initialization callback\r\n");

   //Set the PRNG algorithm to be used
   error = tlsSetPrng(tlsContext, YARROW_PRNG_ALGO, &yarrowContext);
   //Any error to report?
   if(error)
      return error;

#if (APP_SET_CIPHER_SUITES == ENABLED)
   //Preferred cipher suite list
   error = tlsSetCipherSuites(tlsContext, cipherSuites, arraysize(cipherSuites));
   //Any error to report?
   if(error)
      return error;
#endif

#if (APP_SET_SERVER_NAME == ENABLED)
   //Set the fully qualified domain name of the server
   error = tlsSetServerName(tlsContext, APP_FTP_SERVER_NAME);
   //Any error to report?
   if(error)
      return error;
#endif

#if (APP_SET_TRUSTED_CA_LIST == ENABLED)
   //Import the list of trusted CA certificates
   error = tlsSetTrustedCaList(tlsContext, trustedCaList, trustedCaListLength);
   //Any error to report?
   if(error)
      return error;
#endif

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief FTP client test routine
 * @return Error code
 **/

error_t ftpClientTest(void)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   FtpClientContext ftpContext;
   static char_t buffer[256];

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");
   //Resolve FTP server name
   error = getHostByName(NULL, APP_FTP_SERVER_NAME, &ipAddr, 0);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Connecting to FTP server %s\r\n", ipAddrToString(&ipAddr, NULL));

#if (APP_FTP_IMPLICIT_TLS == ENABLED)
   //Register SSL initialisation callback
   ftpRegisterTlsInitCallback(&ftpContext, ftpClientTlsInitCallback);

   //Connect to the FTP server (implicit SSL)
   error = ftpConnect(&ftpContext, NULL, &ipAddr, 990,
      FTP_IMPLICIT_SECURITY | FTP_PASSIVE_MODE);
#elif (APP_FTP_EXPLICIT_TLS == ENABLED)
   //Register SSL initialisation callback
   ftpRegisterTlsInitCallback(&ftpContext, ftpClientTlsInitCallback);

   //Connect to the FTP server (explicit SSL)
   error = ftpConnect(&ftpContext, NULL, &ipAddr, 21,
      FTP_EXPLICIT_SECURITY | FTP_PASSIVE_MODE);
#else
   //Connect to the FTP server (no security)
   error = ftpConnect(&ftpContext, NULL, &ipAddr, 21,
      FTP_NO_SECURITY | FTP_PASSIVE_MODE);
#endif

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to connect to FTP server!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Successful connection\r\n");

   //Start of exception handling block
   do
   {
      //Login to the FTP server using the provided username and password
      error = ftpLogin(&ftpContext, APP_FTP_LOGIN, APP_FTP_PASSWORD, "");
      //Any error to report?
      if(error) break;

      //Open the specified file for reading
      error = ftpOpenFile(&ftpContext, APP_FTP_FILENAME, FTP_FOR_READING | FTP_BINARY_TYPE);
      //Any error to report?
      if(error) break;

      //Dump the contents of the file
      while(1)
      {
         //Read data
         error = ftpReadFile(&ftpContext, buffer, sizeof(buffer) - 1, &length, 0);
         //End of file?
         if(error) break;

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Dump current data
         TRACE_INFO("%s", buffer);
      }

      //End the string with a line feed
      TRACE_INFO("\r\n");
      //Close the file
      error = ftpCloseFile(&ftpContext);

      //End of exception handling block
   } while(0);

   //Close the connection
   ftpClose(&ftpContext);
   //Debug message
   TRACE_INFO("Connection closed...\r\n");

   //Return status code
   return error;
}


/**
 * @brief User task
 **/

void userTask(void *param)
{
   char_t buffer[40];
#if (IPV4_SUPPORT == ENABLED)
   Ipv4Addr ipv4Addr;
#endif

   //Point to the network interface
   NetInterface *interface = &netInterface[0];

   //Initialize LCD display
   lcdSetCursor(2, 0);
   printf("IPv4 Addr\r\n");
   lcdSetCursor(5, 0);
   printf("Press user button\r\nto run test\r\n");

   //Endless loop
   while(1)
   {
#if (IPV4_SUPPORT == ENABLED)
      //Display IPv4 host address
      lcdSetCursor(3, 0);
      ipv4GetHostAddr(interface, &ipv4Addr);
      printf("%-16s\r\n", ipv4AddrToString(ipv4Addr, buffer));
#endif

      //Up button pressed?
      if(!(PIO_BT_UP->PIO_PDSR & BT_UP))
      {
         //FTP client test routine
         ftpClientTest();

         //Wait for the up button to be released
         while(!(PIO_BT_UP->PIO_PDSR & BT_UP));
      }

      //Loop delay
      osDelayTask(100);
   }
}


/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      PIO_LED3->PIO_CODR = LED3;
      osDelayTask(100);
      PIO_LED3->PIO_SODR = LED3;
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
   NetInterface *interface;
   OsTask *task;
   MacAddr macAddr;
#if (APP_USE_DHCP == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Update system core clock
   SystemCoreClockUpdate();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("*** CycloneTCP FTPS Client Demo ***\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: SAM4E\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //Initialize LCD display
   GLCD_Init();
   GLCD_SetBackColor(Blue);
   GLCD_SetTextColor(White);
   GLCD_Clear(Blue);

   //Welcome message
   lcdSetCursor(0, 0);
   printf("FTPS Client Demo\r\n");

   //Generate a random seed

   //PRNG initialization
   error = yarrowInit(&yarrowContext);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize PRNG!\r\n");
   }

   //Properly seed the PRNG
   error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to seed PRNG!\r\n");
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
   netSetHostname(interface, "FTPSClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &sam4eEthDriver);
   netSetPhyDriver(interface, &ksz8051PhyDriver);
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

   //Create user task
   task = osCreateTask("User Task", userTask, NULL, 500, 1);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
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
