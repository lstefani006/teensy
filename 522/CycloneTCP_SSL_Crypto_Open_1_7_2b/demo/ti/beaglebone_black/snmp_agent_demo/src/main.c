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
#include "soc_am335x.h"
#include "hw_control_am335x.h"
#include "cp15.h"
#include "cache.h"
#include "mmu.h"
#include "interrupt.h"
#include "gpio_v2.h"
#include "dmtimer.h"
#include "watchdog.h"
#include "beaglebone_black.h"
#include "core/net.h"
#include "drivers/am335x_eth.h"
#include "drivers/lan8710.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "snmp/snmp_agent.h"
#include "mibs/mib2_module.h"
#include "mibs/mib2_impl.h"
#include "oid.h"
#include "private_mib_module.h"
#include "private_mib_impl.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-00-00-00-00-00"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::3358"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::3358"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_SNMP_ENTERPRISE_OID "1.3.6.1.4.1.8072.9999.9999"
#define APP_SNMP_CONTEXT_ENGINE "\x80\x00\x00\x00\x01\x02\x03\x04"
#define APP_SNMP_TRAP_DEST_IP_ADDR "192.168.0.100"

//DMTimer2 input clock frequency
#define DMTIMER2_INPUT_CLK 24000000
//DMTimer2 tick frequency
#define DMTIMER2_TICK_FREQ 1000
//DMTimer2 reload value
#define DMTIMER2_RELOAD_VALUE (0xFFFFFFFF - (DMTIMER2_INPUT_CLK / DMTIMER2_TICK_FREQ))

//DDR memory region
#define DDR_START_ADDR 0x80000000
#define DDR_NUM_SECTIONS 512
//OCMC memory region
#define OCMC_START_ADDR 0x40300000
#define OCMC_NUM_SECTIONS 1
//Device memory region
#define DEV_START_ADDR 0x44000000
#define DEV_NUM_SECTIONS 960

//Page table
static volatile uint32_t pageTable[4 * 1024] __attribute__((aligned(16 * 1024)));

//Global variables
bool_t ledState = FALSE;
systime_t ledTime = 0;
DhcpState dhcpPrevState = DHCP_STATE_INIT;

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
 * @brief MMU configuration
 **/

void mmuInit(void)
{
   //Define DDR memory region
   REGION ddrRegion =
   {
      MMU_PGTYPE_SECTION,
      DDR_START_ADDR,
      DDR_NUM_SECTIONS,
      MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA, MMU_CACHE_WB_WA),
      MMU_REGION_NON_SECURE,
      MMU_AP_PRV_RW_USR_RW,
      (uint32_t *) pageTable
   };

   //OCMC memory region
   REGION ocmcRegion =
   {
      MMU_PGTYPE_SECTION,
      OCMC_START_ADDR,
      OCMC_NUM_SECTIONS,
      MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA, MMU_CACHE_WB_WA),
      MMU_REGION_NON_SECURE,
      MMU_AP_PRV_RW_USR_RW,
      (uint32_t *) pageTable
   };

   //Device memory region
   REGION devRegion =
   {
      MMU_PGTYPE_SECTION,
      DEV_START_ADDR,
      DEV_NUM_SECTIONS,
      MMU_MEMTYPE_DEVICE_SHAREABLE,
      MMU_REGION_NON_SECURE,
      MMU_AP_PRV_RW_USR_RW | MMU_SECTION_EXEC_NEVER,
      (uint32_t *) pageTable
   };

   //Initialize the page table
   MMUInit((uint32_t *) pageTable);

   //Map the defined regions
   MMUMemRegionMap(&ddrRegion);
   MMUMemRegionMap(&ocmcRegion);
   MMUMemRegionMap(&devRegion);

   //Enable the MMU
   MMUEnable((uint32_t *) pageTable);
}


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Enable clocks for GPIO1 instance
   GPIO1ModuleClkConfig();

   //Enable GPIO1 module
   GPIOModuleEnable(SOC_GPIO_1_REGS);

   //Configure LED1 as a GPIO (GPIO1_21/GPMC_A5)
   HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(5)) = CONTROL_CONF_MUXMODE(7);
   //Configure LED2 as a GPIO (GPIO1_22/GPMC_A6)
   HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(6)) = CONTROL_CONF_MUXMODE(7);
   //Configure LED3 as a GPIO (GPIO1_23/GPMC_A7)
   HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(7)) = CONTROL_CONF_MUXMODE(7);
   //Configure LED4 as a GPIO (GPIO1_24/GPMC_A8)
   HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(8)) = CONTROL_CONF_MUXMODE(7);

   //Configure LED1 as an output
   GPIODirModeSet(SOC_GPIO_1_REGS, 21, GPIO_DIR_OUTPUT);
   //Configure LED2 as an output
   GPIODirModeSet(SOC_GPIO_1_REGS, 22, GPIO_DIR_OUTPUT);
   //Configure LED3 as an output
   GPIODirModeSet(SOC_GPIO_1_REGS, 23, GPIO_DIR_OUTPUT);
   //Configure LED4 as an output
   GPIODirModeSet(SOC_GPIO_1_REGS, 24, GPIO_DIR_OUTPUT);
}


/**
 * @brief DMTimer2 interrupt handler
 **/

void timer2IrqHandler(void)
{
   //Clear interrupt flag
   DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_IT_FLAG);
   //Increment tick counter
   systemTicks++;
}


/**
 * @brief DMTimer2 initialization
 **/

void timer2Init(void)
{
   //Enable clocks for DMTimer2 instance
   DMTimer2ModuleClkConfig();

   //Disable timer
   DMTimerDisable(SOC_DMTIMER_2_REGS);
   //Disable prescaler
   DMTimerPreScalerClkDisable(SOC_DMTIMER_2_REGS);

   //Configure DMTimer2 for auto-reload and compare mode
   DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_CMP_ENABLE);
   //Set reload value
   DMTimerReloadSet(SOC_DMTIMER_2_REGS, DMTIMER2_RELOAD_VALUE);
   //Initialize counter value
   DMTimerCounterSet(SOC_DMTIMER_2_REGS, DMTIMER2_RELOAD_VALUE);

   //Enable DMTimer2 interrupts
   DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

   //Register interrupt handler
   IntRegister(SYS_INT_TINT2, timer2IrqHandler);
   //Configure interrupt priority
   IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);
   //Enable system interrupts
   IntSystemEnable(SYS_INT_TINT2);

   //Enable timer
   DMTimerEnable(SOC_DMTIMER_2_REGS);
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
         GPIOPinWrite(SOC_GPIO_1_REGS, 24, GPIO_PIN_HIGH);
         ledState = 1;
         ledTime = systemTicks + 100;
      }
      else
      {
         GPIOPinWrite(SOC_GPIO_1_REGS, 24, GPIO_PIN_LOW);
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
#if (APP_USE_DHCP == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Disable watchdog timer
   WatchdogTimerDisable(SOC_WDT_1_REGS);

   //Disable cache
   CacheDisable(CACHE_ALL);
   //Disable MMU
   CP15MMUDisable();

   //Configure and enable MMU
   mmuInit();
   //Enable cache
   CacheEnable(CACHE_ALL);

   //Initialize interrupt controller
   IntAINTCInit();

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
   TRACE_INFO("Target: AM335x\r\n");
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
   netSetDriver(interface, &am335xEthPort1Driver);
   netSetPhyDriver(interface, &lan8710PhyDriver);
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

   //Initialize DMTimer2
   timer2Init();

   //Enable processor IRQ
   IntMasterIRQEnable();

   //Endless loop
   while(1)
   {
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
