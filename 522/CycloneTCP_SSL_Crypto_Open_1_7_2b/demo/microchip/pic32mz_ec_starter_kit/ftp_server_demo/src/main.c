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
#include "pic32mz_ec_starter_kit.h"
#include "os_port.h"
#include "fs_port.h"
#include "core/net.h"
#include "drivers/pic32mz_eth.h"
#include "drivers/lan8740.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "ftp/ftp_server.h"
#include "path.h"
#include "ext_int_driver.h"
#include "debug.h"

//PIC32 onfiguration settings
#pragma config USERID = 0x0000
#pragma config FMIIEN = OFF
#pragma config FETHIO = ON
#pragma config PGL1WAY = OFF
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
#pragma config FUSBIDIO = OFF
#pragma config FPLLIDIV = DIV_3
#pragma config FPLLRNG = RANGE_13_26_MHZ
#pragma config FPLLICLK = PLL_POSC
#pragma config FPLLMULT = MUL_50
#pragma config FPLLODIV = DIV_2
#pragma config UPLLFSEL = FREQ_12MHZ
#pragma config UPLLEN = OFF
#pragma config FNOSC = SPLL
#pragma config DMTINTV = WIN_127_128
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config POSCMOD = EC
#pragma config OSCIOFNC = OFF
#pragma config FCKSM = CSECMD
#pragma config WDTPS = PS1048576
#pragma config WDTSPGM = STOP
#pragma config WINDIS= NORMAL
#pragma config FWDTEN = OFF
#pragma config FWDTWINSZ = WINSZ_25
#pragma config DMTCNT = DMT31
#pragma config FDMTEN = OFF
//#pragma config DEBUG = ON
#pragma config JTAGEN = OFF
#pragma config ICESEL = ICS_PGx2
#pragma config TRCEN = ON
#pragma config BOOTISA = MIPS32
#pragma config FECCCON = OFF_UNLOCKED
#pragma config FSLEEP = OFF
#pragma config DBGPER = PG_ALL
#pragma config EJTAGBEN = NORMAL
#pragma config CP = OFF
#pragma config_alt FWDTEN=OFF

//Application configuration
#define APP_USE_DEFAULT_MAC_ADDR ENABLED
#define APP_MAC_ADDR "00-AB-CD-EF-20-48"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::2048"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::2048"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//Global variables
DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
FtpServerSettings ftpServerSettings;
FtpServerContext ftpServerContext;

//Forward declaration of functions
uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
   const char_t *user);

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
   const char_t *user, const char_t *password);

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
   const char_t *user, const char_t *path);

//External 4 interrupt service routine
void __ISR(_EXTERNAL_4_VECTOR, IPL1AUTO) ext4IrqWrapper(void);
//Ethernet interrupt service routine
void __ISR(_ETHERNET_VECTOR, IPL1AUTO) ethIrqWrapper(void);


/**
 * @brief System initialization
 **/

void systemInit(void)
{
   //Execute system unlock sequence
   SYSKEY = 0xAA996655;
   SYSKEY = 0x556699AA;

   //Check PBDIVRDY bit
   while(!(PB2DIV & _PB2DIV_PBDIVRDY_MASK));
   //Configure PBCLK2 clock divisor (SYSCLK / 5);
   PB2DIV = _PB2DIV_ON_MASK | 4;

   //Check PBDIVRDY bit
   while(!(PB3DIV & _PB3DIV_PBDIVRDY_MASK));
   //Configure PBCLK3 clock divisor (SYSCLK / 5);
   PB3DIV = _PB3DIV_ON_MASK | 4;

   //Check PBDIVRDY bit
   while(!(PB4DIV & _PB4DIV_PBDIVRDY_MASK));
   //Configure PBCLK4 clock divisor (SYSCLK / 1);
   PB4DIV = _PB4DIV_ON_MASK | 0;

   //Check PBDIVRDY bit
   while(!(PB5DIV & _PB5DIV_PBDIVRDY_MASK));
   //Configure PBCLK5 clock divisor (SYSCLK / 2);
   PB5DIV = _PB5DIV_ON_MASK | 1;

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
   //Configure LED1 (RH0), LED2 (RH1) and LED3 (RH2)
   TRISHCLR = LED1_MASK | LED2_MASK | LED3_MASK;
   //Clear LEDs
   LATHCLR = LED1_MASK | LED2_MASK | LED3_MASK;
   //Disable analog pads
   ANSELHCLR = LED1_MASK | LED2_MASK | LED3_MASK;

   //Configure SW1 (RB12), SW2 (RB13) and SW3 (RB14)
   TRISBSET = SW1_MASK | SW2_MASK | SW3_MASK;
   //Enable pull-ups
   CNPUBSET = SW1_MASK | SW2_MASK | SW3_MASK;
   //Disable analog pads
   ANSELBCLR = SW1_MASK | SW2_MASK | SW3_MASK;

   //Configure MEB2_LED1 (RH0), MEB2_LED2 (RH2), MEB2_LED3 (RH1)
   //MEB2_LED4 (RH6) and MEB2_LED5 (RH11)
   TRISHCLR = MEB2_LED1_MASK | MEB2_LED2_MASK |
      MEB2_LED3_MASK | MEB2_LED4_MASK | MEB2_LED5_MASK;
   //Clear LEDs
   LATHCLR = MEB2_LED1_MASK | MEB2_LED2_MASK |
      MEB2_LED3_MASK | MEB2_LED4_MASK | MEB2_LED5_MASK;
   //Disable analog pads
   ANSELHCLR = MEB2_LED1_MASK | MEB2_LED2_MASK |
      MEB2_LED3_MASK | MEB2_LED4_MASK | MEB2_LED5_MASK;

   //Configure MEB2_S1 (RA0)
   TRISASET = MEB2_S1_MASK;
   //Enable pull-up
   CNPUASET = MEB2_S1_MASK;
   //Disable analog pad
   ANSELACLR = MEB2_S1_MASK;
}


/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      LATHSET = MEB2_LED2_MASK;
      osDelayTask(100);
      LATHCLR = MEB2_LED2_MASK;
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

   //System initialization
   systemInit();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("*** CycloneTCP FTP Server Demo ***\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("Copyright: 2010-2016 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: PIC32MZ2048ECH144\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //File system initialization
   error = fsInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize file system!\r\n");
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
   netSetHostname(interface, "FTPServerDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &pic32mzEthDriver);
   netSetPhyDriver(interface, &lan8740PhyDriver);
   //Set external interrupt line driver
   netSetExtIntDriver(interface, &extIntDriver);

#if (APP_USE_DEFAULT_MAC_ADDR == ENABLED)
   //Use the factory preprogrammed MAC address
   macStringToAddr("00-00-00-00-00-00", &macAddr);
   netSetMacAddr(interface, &macAddr);
#else
   //Override the factory preprogrammed address
   macStringToAddr(APP_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);
#endif

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

   //Get default settings
   ftpServerGetDefaultSettings(&ftpServerSettings);
   //Bind FTP server to the desired interface
   ftpServerSettings.interface = &netInterface[0];
   //Listen to port 21
   ftpServerSettings.port = FTP_PORT;
   //Root directory
   strcpy(ftpServerSettings.rootDir, "/");
   //User verification callback function
   ftpServerSettings.checkUserCallback = ftpServerCheckUserCallback;
   //Password verification callback function
   ftpServerSettings.checkPasswordCallback = ftpServerCheckPasswordCallback;
   //Callback used to retrieve file permissions
   ftpServerSettings.getFilePermCallback = ftpServerGetFilePermCallback;

   //FTP server initialization
   error = ftpServerInit(&ftpServerContext, &ftpServerSettings);
   //Failed to initialize FTP server?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize FTP server!\r\n");
   }

   //Start FTP server
   error = ftpServerStart(&ftpServerContext);
   //Failed to start FTP server?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start FTP server!\r\n");
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
 * @brief User verification callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @return Access status (FTP_ACCESS_ALLOWED, FTP_ACCESS_DENIED or FTP_PASSWORD_REQUIRED)
 **/

uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
   const char_t *user)
{
   //Debug message
   TRACE_INFO("FTP server: User verification\r\n");

   //Manage authentication policy
   if(!strcmp(user, "anonymous"))
   {
      return FTP_ACCESS_ALLOWED;
   }
   else if(!strcmp(user, "admin"))
   {
      return FTP_PASSWORD_REQUIRED;
   }
   else
   {
      return FTP_ACCESS_DENIED;
   }
}


/**
 * @brief Password verification callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @param[in] password NULL-terminated string that contains the corresponding password
 * @return Access status (FTP_ACCESS_ALLOWED or FTP_ACCESS_DENIED)
 **/

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
   const char_t *user, const char_t *password)
{
   //Debug message
   TRACE_INFO("FTP server: Password verification\r\n");

   //Verify password
   if(!strcmp(user, "admin") && !strcmp(password, "admin"))
   {
      return FTP_ACCESS_ALLOWED;
   }
   else
   {
      return FTP_ACCESS_DENIED;
   }
}


/**
 * @brief Callback used to retrieve file permissions
 * @param[in] connection Handle referencing a client connection
 * @param[in] user NULL-terminated string that contains the user name
 * @param[in] path Canonical path of the file
 * @return Permissions for the specified file
 **/

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
   const char_t *user, const char_t *path)
{
   uint_t perm;

   //Debug message
   TRACE_INFO("FTP server: Checking access rights for %s\r\n", path);

   //Manage access rights
   if(!strcmp(user, "anonymous"))
   {
      //Allow read/write access to temp directory
      if(pathMatch(path, "/temp/*"))
         perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
      //Allow read access only to other directories
      else
         perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ;
   }
   else if(!strcmp(user, "admin"))
   {
      //Allow read/write access
      perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
   }
   else
   {
      //Deny access
      perm = 0;
   }

   //Return the relevant permissions
   return perm;
}
