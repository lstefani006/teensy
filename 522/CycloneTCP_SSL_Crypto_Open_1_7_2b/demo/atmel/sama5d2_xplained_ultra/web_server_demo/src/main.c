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
#include <string.h>
#include "chip.h"
#include "cp15.h"
#include "mmu.h"
#include "peripherals/aic.h"
#include "peripherals/pio.h"
#include "peripherals/wdt.h"
#include "board.h"
#include "act8945a.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/sama5d2_eth.h"
#include "drivers/ksz8081.h"
#include "dhcp/dhcp_client.h"
#include "smtp/smtp_client.h"
#include "http/http_server.h"
#include "http/mime.h"
#include "yarrow.h"
#include "str.h"
#include "path.h"
#include "date_time.h"
#include "resource_manager.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-A5-D2"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::a5d2"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::a5d2"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_HTTP_MAX_CONNECTIONS 8

//Global variables
uint_t adcValue = 1000;
uint_t joystickState = 0;
int8_t ax = 0;
int8_t ay = 0;
int8_t az = 0;

DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext0;
DhcpClientCtx dhcpClientContext1;
SlaacSettings slaacSettings;
SlaacContext slaacContext0;
SlaacContext slaacContext1;
HttpServerSettings httpServerSettings;
HttpServerContext httpServerContext;
HttpConnection httpConnections[APP_HTTP_MAX_CONNECTIONS];
YarrowContext yarrowContext;
uint8_t seed[32];

//Forward declaration of functions
HttpAccessStatus httpServerAuthCallback(HttpConnection *connection,
   const char_t *user, const char_t *uri);

error_t httpServerCgiCallback(HttpConnection *connection,
   const char_t *param);

error_t httpServerUriNotFoundCallback(HttpConnection *connection,
   const char_t *uri);


/**
 * @brief PMIC configuration
 **/

void pmicInit(void)
{
   struct _pin act8945aPins[] = ACT8945A_PINS;

   //ACT8945A parameters
   struct _act8945a act8945a =
   {
      .desc =
      {
         .pin_chglev = ACT8945A_PIN_CHGLEV,
         .pin_irq = ACT8945A_PIN_IRQ,
         .pin_lbo = ACT8945A_PIN_LBO
      }
   };

   //TWI parameters
   struct _twi_desc twiDesc =
   {
      .addr = ACT8945A_ADDR,
      .freq = ACT8945A_FREQ,
      .transfert_mode = TWID_MODE_POLLING
   };

   //Configure ACT8945A pins
   pio_configure(act8945aPins, arraysize(act8945aPins));

   //Configure ACT8945A
   act8945a_configure(&act8945a, &twiDesc);
   act8945a_set_regulator_voltage(&act8945a, 6, 2500);
   act8945a_enable_regulator(&act8945a, 6, TRUE);
}


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   struct _pin led0Pin = PIN_LED_0;
   struct _pin led1Pin = PIN_LED_1;
   struct _pin led2Pin = PIN_LED_2;
   struct _pin buttonPin = PIN_PUSHBUTTON_1;

   //Configure red LED
   pio_configure(&led0Pin, 1);
   pio_set(&led0Pin);

   //Configure green LED
   pio_configure(&led1Pin, 1);
   pio_set(&led1Pin);

   //Configure blue LED
   pio_configure(&led2Pin, 1);
   pio_set(&led2Pin);

   //Configure push button
   pio_configure(&buttonPin, 1);
}


/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   struct _pin led1Pin = PIN_LED_1;

   //Endless loop
   while(1)
   {
      pio_clear(&led1Pin);
      osDelayTask(100);
      pio_set(&led1Pin);
      osDelayTask(900);
   }
}


/**
 * @brief Spurious IRQ handler
 **/

void spuriousIrqHandler(void)
{
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

   //Disable watchdog timer
   wdt_disable() ;

   //Disable protect mode
   aic_debug_config(AIC, FALSE, FALSE);
   //Set spurious interrupt handler
   aic_set_spurious_vector(spuriousIrqHandler);

   //Initialize MMU
   mmu_initialize();
   //Enable MMU
   cp15_enable_mmu();

   //Enable instruction cache
   cp15_enable_icache();
   //Enable data cache
   cp15_enable_dcache();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("*** CycloneTCP Web Server Demo ***\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: SAMA5D27\r\n");
   TRACE_INFO("\r\n");

   //Timer configuration
   timer_configure();
   //Configure PMIC
   pmicInit();
   //Configure I/Os
   ioInit();

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
   netSetHostname(interface, "WebServerDemo0");
   //Select the relevant network adapter
   netSetDriver(interface, &sama5d2EthDriver);
   netSetPhyDriver(interface, &ksz8081PhyDriver);
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
   error = dhcpClientInit(&dhcpClientContext0, &dhcpClientSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP client!\r\n");
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext0);
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
   error = slaacInit(&slaacContext0, &slaacSettings);
   //Failed to initialize SLAAC?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize SLAAC!\r\n");
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext0);
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
   httpServerGetDefaultSettings(&httpServerSettings);
   //Bind HTTP server to the desired interface
   httpServerSettings.interface = &netInterface[0];
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
   httpServerSettings.authCallback = httpServerAuthCallback;
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
 * @brief HTTP authentication callback
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string specifying the user name
 * @param[in] uri NULL-terminated string containing the path to the requested resource
 * @return Access status (HTTP_ACCESS_ALLOWED, HTTP_ACCESS_DENIED,
 *   HTTP_BASIC_AUTH_REQUIRED or HTTP_DIGEST_AUTH_REQUIRED)
 **/

HttpAccessStatus httpServerAuthCallback(HttpConnection *connection,
   const char_t *user, const char_t *uri)
{
   HttpAccessStatus status;

   //Manage access rights
   if(pathMatch(uri, "/passwords.txt"))
   {
      //This file is not visible
      status = HTTP_ACCESS_DENIED;
   }
   else if(pathMatch(uri, "/config/*"))
   {
      //This directory is not visible
      status = HTTP_ACCESS_DENIED;
   }
   else if(pathMatch(uri, "/admin/*"))
   {
      //Only the administrator can access this directory
      if(!strcmp(user, "administrator"))
      {
         //Check the administrator password
         if(httpCheckPassword(connection, "password", HTTP_AUTH_MODE_BASIC))
            status = HTTP_ACCESS_ALLOWED;
         else
            status = HTTP_ACCESS_BASIC_AUTH_REQUIRED;
      }
      else
      {
         //Users other than administrator cannot access this directory
         status = HTTP_ACCESS_BASIC_AUTH_REQUIRED;
      }
   }
   else
   {
      //No restriction for other directories
      status = HTTP_ACCESS_ALLOWED;
   }

   //Return access status
   return status;
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
      strcpy(connection->buffer, "SAMA5D2-Xplained-Ultra");
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
   error_t error;
   size_t n;
   char_t *buffer;

   //Process data.xml file?
   if(!strcasecmp(uri, "/data.xml"))
   {
      //Point to the scratch buffer
      buffer = connection->buffer + 384;

      //Format XML data
      n = sprintf(buffer, "<data>\r\n");
      n += sprintf(buffer + n, "  <ax>%d</ax>\r\n", ax);
      n += sprintf(buffer + n, "  <ay>%d</ay>\r\n", ay);
      n += sprintf(buffer + n, "  <az>%d</az>\r\n", az);
      n += sprintf(buffer + n, "  <adc>%u</adc>\r\n", adcValue);
      n += sprintf(buffer + n, "  <joystick>%u</joystick>\r\n", joystickState);

      //End of XML data
      n += sprintf(buffer + n, "</data>\r\n");

      //Format HTTP response header
      connection->response.version = connection->request.version;
      connection->response.statusCode = 200;
      connection->response.keepAlive = connection->request.keepAlive;
      connection->response.noCache = TRUE;
      connection->response.contentType = mimeGetType(".xml");
      connection->response.chunkedEncoding = FALSE;
      connection->response.contentLength = n;

      //Send the header to the client
      error = httpWriteHeader(connection);
      //Any error to report?
      if(error) return error;

      //Send response body
      error = httpWriteStream(connection, buffer, n);
      //Any error to report?
      if(error) return error;

      //Properly close output stream
      error = httpCloseStream(connection);
      //Return status code
      return error;
   }
   //Process send_mail.xml file?
   else if(!strcasecmp(uri, "/send_mail.xml"))
   {
      char_t *separator;
      char_t *property;
      char_t *value;
      char_t *p;
      SmtpAuthInfo authInfo;
      SmtpMail mail;
      SmtpMailAddr recipients[4];

      //Initialize structures to zero
      memset(&authInfo, 0, sizeof(authInfo));
      memset(&mail, 0, sizeof(mail));
      memset(recipients, 0, sizeof(recipients));

      //Set the relevant PRNG algorithm to be used
      authInfo.prngAlgo = YARROW_PRNG_ALGO;
      authInfo.prngContext = &yarrowContext;

      //Set email recipients
      mail.recipients = recipients;
      //Point to the scratch buffer
      buffer = connection->buffer;

      //Start of exception handling block
      do
      {
         //Process HTTP request body
         while(1)
         {
            //Read the HTTP request body until an ampersand is encountered
            error = httpReadStream(connection, buffer,
               HTTP_SERVER_BUFFER_SIZE - 1, &n, HTTP_FLAG_BREAK('&'));
            //End of stream detected?
            if(error) break;

            //Properly terminate the string with a NULL character
            buffer[n] = '\0';

            //Remove the trailing ampersand
            if(n > 0 && buffer[n - 1] == '&')
               buffer[--n] = '\0';

            //Decode the percent-encoded string
            httpDecodePercentEncodedString(buffer, buffer, HTTP_SERVER_BUFFER_SIZE);
            //Check whether a separator is present
            separator = strchr(buffer, '=');

            //Separator found?
            if(separator)
            {
               //Split the line
               *separator = '\0';
               //Get property name and value
               property = strTrimWhitespace(buffer);
               value = strTrimWhitespace(separator + 1);

               //Check property name
               if(!strcasecmp(property, "server"))
               {
                  //Save server name
                  authInfo.serverName = strDuplicate(value);
               }
               else if(!strcasecmp(property, "port"))
               {
                  //Save the server port to be used
                  authInfo.serverPort = atoi(value);
               }
               else if(!strcasecmp(property, "userName"))
               {
                  //Save user name
                  authInfo.userName = strDuplicate(value);
               }
               else if(!strcasecmp(property, "password"))
               {
                  //Save password
                  authInfo.password = strDuplicate(value);
               }
               else if(!strcasecmp(property, "useTls"))
               {
                  //Open a secure SSL/TLS session?
                  authInfo.useTls = TRUE;
               }
               else if(!strcasecmp(property, "recipient"))
               {
                  //Split the recipient address list
                  value = strtok_r(value, ", ", &p);

                  //Loop through the list
                  while(value != NULL)
                  {
                     //Save recipient address
                     recipients[mail.recipientCount].name = NULL;
                     recipients[mail.recipientCount].addr = strDuplicate(value);
                     recipients[mail.recipientCount].type = SMTP_RCPT_TYPE_TO;
                     //Get the next item in the list
                     value = strtok_r(NULL, ", ", &p);

                     //Increment the number of recipients
                     if(++mail.recipientCount >= arraysize(recipients))
                        break;
                  }
               }
               else if(!strcasecmp(property, "from"))
               {
                  //Save sender address
                  mail.from.name = NULL;
                  mail.from.addr = strDuplicate(value);
               }
               else if(!strcasecmp(property, "date"))
               {
                  //Save current time
                  mail.dateTime = strDuplicate(value);
               }
               else if(!strcasecmp(property, "subject"))
               {
                  //Save mail subject
                  mail.subject = strDuplicate(value);
               }
               else if(!strcasecmp(property, "body"))
               {
                  //Save mail body
                  mail.body = strDuplicate(value);
               }
            }
         }

         //Propagate exception if necessary
         if(error != ERROR_END_OF_STREAM)
            break;

         //Send mail
         error = smtpSendMail(&authInfo, &mail);

         //Point to the scratch buffer
         buffer = connection->buffer + 384;
         //Format XML data
         n = sprintf(buffer, "<data>\r\n  <status>");

         if(error == NO_ERROR)
            n += sprintf(buffer + n, "Mail successfully sent!\r\n");
         else if(error == ERROR_NAME_RESOLUTION_FAILED)
            n += sprintf(buffer + n, "Cannot resolve SMTP server name!\r\n");
         else if(error == ERROR_AUTHENTICATION_FAILED)
            n += sprintf(buffer + n, "Authentication failed!\r\n");
         else if(error == ERROR_UNEXPECTED_RESPONSE)
            n += sprintf(buffer + n, "Unexpected response from SMTP server!\r\n");
         else
            n += sprintf(buffer + n, "Failed to send mail (error %d)!\r\n", error);

         n += sprintf(buffer + n, "</status>\r\n</data>\r\n");

         //Format HTTP response header
         connection->response.version = connection->request.version;
         connection->response.statusCode = 200;
         connection->response.keepAlive = connection->request.keepAlive;
         connection->response.noCache = TRUE;
         connection->response.contentType = mimeGetType(".xml");
         connection->response.chunkedEncoding = FALSE;
         connection->response.contentLength = n;

         //Send the header to the client
         error = httpWriteHeader(connection);
         //Any error to report?
         if(error) break;

         //Send response body
         error = httpWriteStream(connection, buffer, n);
         //Any error to report?
         if(error) break;

         //Properly close output stream
         error = httpCloseStream(connection);
         //Any error to report?
         if(error) break;

         //End of exception handling block
      } while(0);

      //Free previously allocated memory
      osFreeMem((void *) authInfo.serverName);
      osFreeMem((void *) authInfo.userName);
      osFreeMem((void *) authInfo.password);
      osFreeMem((void *) recipients[0].addr);
      osFreeMem((void *) mail.from.addr);
      osFreeMem((void *) mail.dateTime);
      osFreeMem((void *) mail.subject);
      osFreeMem((void *) mail.body);

      //Return status code
      return error;
   }
   else
   {
      return ERROR_NOT_FOUND;
   }
}
