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
#include "samv71.h"
#include "samv71_xplained_ultra.h"
#include "os_port.h"
#include "resource_manager.h"
#include "core/net.h"
#include "drivers/samv71_eth.h"
#include "drivers/ksz8061.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "tls.h"
#include "tls_cipher_suites.h"
#include "yarrow.h"
#include "ext_int_driver.h"
#include "error.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-71-21"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::7121"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::7121"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//Server hostname and port
#define APP_SERVER_NAME "www.oryx-embedded.com"
#define APP_SERVER_PORT 443
#define APP_REQUEST_URI "/test.php"

//Compilation options
#define APP_SET_CIPHER_SUITES DISABLED
#define APP_SET_SERVER_NAME DISABLED
#define APP_SET_TRUSTED_CA_LIST DISABLED
#define APP_SET_CLIENT_CERT DISABLED

//Trusted CA bundle
#define APP_CA_CERT_BUNDLE "certs/ca_cert_bundle.pem"

//Client's certificate and private key
#define APP_CLIENT_CERT "certs/client_rsa_cert.pem"
#define APP_CLIENT_PRIVATE_KEY "certs/client_rsa_key.pem"

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

//Credentials
char_t *clientCert = NULL;
size_t clientCertLength = 0;
char_t *clientPrivateKey = NULL;
size_t clientPrivateKeyLength = 0;
char_t *trustedCaList = NULL;
size_t trustedCaListLength = 0;

DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
YarrowContext yarrowContext;
uint8_t seed[32];


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Disable write protection
   MATRIX->MATRIX_WPMR = MATRIX_WPMR_WPKEY_PASSWD;
   //Select PB12 function rather than ERASE function
   MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO12;

   //Enable PIO peripheral clocks
   PMC->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOB) | (1 << ID_PIOC);

   //Configure LED0
   PIO_LED0->PIO_PER = LED0;
   PIO_LED0->PIO_OER = LED0;
   PIO_LED0->PIO_SODR = LED0;

   //Configure LED1
   PIO_LED1->PIO_PER = LED1;
   PIO_LED1->PIO_OER = LED1;
   PIO_LED1->PIO_SODR = LED1;

   //Configure SW0 button
   PIO_SW0->PIO_PER = SW0;
   PIO_SW0->PIO_ODR = SW0;
   PIO_SW0->PIO_PUER = SW0;

   //Configure SW1 button
   PIO_SW1->PIO_PER = SW1;
   PIO_SW1->PIO_ODR = SW1;
   PIO_SW1->PIO_PUER = SW1;
}


/**
 * @brief SSL client test routine
 * @return Error code
 **/

error_t sslClientTest(void)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   static char_t buffer[256];

   //Underlying socket
   Socket *socket = NULL;
   //SSL/TLS context
   TlsContext *tlsContext = NULL;

   //Debug message
   TRACE_INFO("Resolving server name...\r\n");

   //Resolve SSL server name
   error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Create a new socket to handle the request
   socket = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP);
   //Any error to report?
   if(!socket)
   {
      //Debug message
      TRACE_INFO("Failed to open socket!\r\n");
      //Exit immediately
      return ERROR_OPEN_FAILED;
   }

   //Start of exception handling block
   do
   {
      //Debug message
      TRACE_INFO("Connecting to SSL server %s\r\n", ipAddrToString(&ipAddr, NULL));

      //Connect to the SSL server
      error = socketConnect(socket, &ipAddr, APP_SERVER_PORT);
      //Any error to report?
      if(error) break;

      //Initialize SSL/TLS context
      tlsContext = tlsInit();
      //Initialization failed?
      if(!tlsContext)
      {
         //Report an error
         error = ERROR_OUT_OF_MEMORY;
         //Exit immediately
         break;
      }

      //Bind TLS to the relevant socket
      error = tlsSetSocket(tlsContext, socket);
      //Any error to report?
      if(error) break;

      //Select client operation mode
      error = tlsSetConnectionEnd(tlsContext, TLS_CONNECTION_END_CLIENT);
      //Any error to report?
      if(error) break;

      //Set the PRNG algorithm to be used
      error = tlsSetPrng(tlsContext, YARROW_PRNG_ALGO, &yarrowContext);
      //Any error to report?
      if(error) break;

#if (APP_SET_CIPHER_SUITES == ENABLED)
      //Preferred cipher suite list
      error = tlsSetCipherSuites(tlsContext, cipherSuites, arraysize(cipherSuites));
      //Any error to report?
      if(error) break;
#endif

#if (APP_SET_SERVER_NAME == ENABLED)
      //Set the fully qualified domain name of the server
      error = tlsSetServerName(tlsContext, APP_SERVER_NAME);
      //Any error to report?
      if(error) break;
#endif

#if (APP_SET_TRUSTED_CA_LIST == ENABLED)
      //Import the list of trusted CA certificates
      error = tlsSetTrustedCaList(tlsContext, trustedCaList, trustedCaListLength);
      //Any error to report?
      if(error) break;
#endif

#if (APP_SET_CLIENT_CERT == ENABLED)
      //Import the client's certificate
      error = tlsAddCertificate(tlsContext, clientCert,
         clientCertLength, clientPrivateKey, clientPrivateKeyLength);
      //Any error to report?
      if(error) break;
#endif

      //Establish a secure session
      error = tlsConnect(tlsContext);
      //TLS handshake failure?
      if(error) break;

      //Format HTTP request
      sprintf(buffer, "GET %s HTTP/1.0\r\nHost: %s:%u\r\n\r\n",
         APP_REQUEST_URI, APP_SERVER_NAME, APP_SERVER_PORT);

      //Debug message
      TRACE_INFO("\r\n");
      TRACE_INFO("HTTP request:\r\n%s", buffer);

      //Send the request
      error = tlsWrite(tlsContext, buffer, strlen(buffer), 0);
      //Any error to report?
      if(error) break;

      //Debug message
      TRACE_INFO("HTTP response:\r\n");

      //Read the whole response
      while(1)
      {
         //Read data
         error = tlsRead(tlsContext, buffer, sizeof(buffer) - 1, &length, 0);
         //End of stream?
         if(error) break;

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Debug message
         TRACE_INFO("%s", buffer);
      }

      //Successfull processing
      error = NO_ERROR;

      //End of exception handling block
   } while(0);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to communicate with SSL server!\r\n");
   }

   //Terminate TLS session
   tlsFree(tlsContext);
   //Close socket
   socketClose(socket);

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
   //Endless loop
   while(1)
   {
      //SW0 button pressed?
      if(!(PIO_SW0->PIO_PDSR & SW0))
      {
         //SSL client test routine
         sslClientTest();

         //Wait for the SW0 button to be released
         while(!(PIO_SW0->PIO_PDSR & SW0));
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
   uint_t i;
   uint32_t value;
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
   WDT->WDT_MR = WDT_MR_WDDIS;

   //Enable I-cache
   SCB_EnableICache();
   //D-cache is not supported
   //SCB_EnableDCache();

   //Update system core clock
   SystemCoreClockUpdate();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("******************************\r\n");
   TRACE_INFO("*** CycloneSSL Client Demo ***\r\n");
   TRACE_INFO("******************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: SAMV71\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //Enable TRNG peripheral clock
   PMC->PMC_PCER1 = (1 << (ID_TRNG - 32));
   //Enable the TRNG
   TRNG->TRNG_CR = TRNG_CR_KEY_PASSWD | TRNG_CR_ENABLE;

   //Generate a random seed
   for(i = 0; i < 32; i += 4)
   {
      //Wait for the TRNG to contain a valid data
      while(!(TRNG->TRNG_ISR & TRNG_ISR_DATRDY));

      //Get 32-bit random value
      value = TRNG->TRNG_ODATA;

      //Copy random value
      seed[i] = value & 0xFF;
      seed[i + 1] = (value >> 8) & 0xFF;
      seed[i + 2] = (value >> 16) & 0xFF;
      seed[i + 3] = (value >> 24) & 0xFF;
   }

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

   //Debug message
   TRACE_INFO("Loading credentials...\r\n");

   //Start of exception handling block
   do
   {
      //Load trusted CA certificates
      error = resGetData(APP_CA_CERT_BUNDLE, (uint8_t **) &trustedCaList, &trustedCaListLength);
      //Any error to report?
      if(error) break;

      //Load client's certificate
      error = resGetData(APP_CLIENT_CERT, (uint8_t **) &clientCert, &clientCertLength);
      //Any error to report?
      if(error) break;

      //Load client's private key
      error = resGetData(APP_CLIENT_PRIVATE_KEY, (uint8_t **) &clientPrivateKey, &clientPrivateKeyLength);
      //Any error to report?
      if(error) break;

      //End of exception handling block
   } while(0);

   //Check error code
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to load credentials!\r\n");
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
   netSetHostname(interface, "SSLClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &samv71EthDriver);
   netSetPhyDriver(interface, &ksz8061PhyDriver);
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
   task = osCreateTask("User Task", userTask, NULL, 600, 1);
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
