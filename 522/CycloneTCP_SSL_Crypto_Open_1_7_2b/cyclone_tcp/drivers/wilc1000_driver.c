/**
 * @file wilc1000_driver.c
 * @brief WILC1000 Wi-Fi controller
 *
 * @section License
 *
 * Copyright (C) 2010-2016 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Open.
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

//Switch to the appropriate trace level
#define TRACE_LEVEL NIC_TRACE_LEVEL

//Dependencies
#include "driver/include/m2m_wifi.h"
#include "core/net.h"
#include "wilc1000_driver.h"
#include "wilc1000_config.h"
#include "debug.h"

//Underlying network interface
static NetInterface *wilc1000StaInterface = NULL;
static NetInterface *wilc1000ApInterface = NULL;

//Transmit buffer
static uint8_t txBuffer[WILC1000_TX_BUFFER_SIZE];
//Receive buffer
static uint8_t rxBuffer[WILC1000_RX_BUFFER_SIZE];


/**
 * @brief WILC1000 driver (STA mode)
 **/

const NicDriver wilc1000StaDriver =
{
   NIC_TYPE_ETHERNET,
   ETH_MTU,
   wilc1000Init,
   wilc1000Tick,
   wilc1000EnableIrq,
   wilc1000DisableIrq,
   wilc1000EventHandler,
   wilc1000SendPacket,
   wilc1000SetMulticastFilter,
   NULL,
   NULL,
   NULL,
   TRUE,
   TRUE,
   TRUE
};


/**
 * @brief WILC1000 driver (AP mode)
 **/

const NicDriver wilc1000ApDriver =
{
   NIC_TYPE_ETHERNET,
   ETH_MTU,
   wilc1000Init,
   wilc1000Tick,
   wilc1000EnableIrq,
   wilc1000DisableIrq,
   wilc1000EventHandler,
   wilc1000SendPacket,
   wilc1000SetMulticastFilter,
   NULL,
   NULL,
   NULL,
   TRUE,
   TRUE,
   TRUE
};


/**
 * @brief WILC1000 initialization
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t wilc1000Init(NetInterface *interface)
{
   int8_t status;
   tstrWifiInitParam param;

   //STA or AP mode?
   if(interface->nicDriver == &wilc1000StaDriver)
   {
      //Debug message
      TRACE_INFO("Initializing WILC1000 (STA mode)...\r\n");
   }
   else
   {
      //Debug message
      TRACE_INFO("Initializing WILC1000 (AP mode)...\r\n");
   }

   //Start of exception handling block
   do
   {
      //Initialization sequence is performed once at startup
      if(wilc1000StaInterface == NULL && wilc1000ApInterface == NULL)
      {
         //Low-level initialization
         status = nm_bsp_init();

         //Check status code
         if(status != M2M_SUCCESS)
            break;

         //Set default parameters
         memset(&param, 0, sizeof(param));

         //Register callback functions
         param.pfAppWifiCb = wilc1000AppWifiEvent;
         param.pfAppMonCb = NULL;
         param.strEthInitParam.pfAppWifiCb = NULL;
         param.strEthInitParam.pfAppEthCb = wilc1000AppEthEvent;

         //Set receive buffer
         param.strEthInitParam.au8ethRcvBuf = rxBuffer;
         param.strEthInitParam.u16ethRcvBufSize = WILC1000_RX_BUFFER_SIZE - ETH_CRC_SIZE;

         //Initialize WILC1000 controller
         status = m2m_wifi_init(&param);

         //Check status code
         if(status != M2M_SUCCESS)
            break;

         //Optionally set the station MAC address
         if(macCompAddr(&interface->macAddr, &MAC_UNSPECIFIED_ADDR))
         {
            //Use the factory preprogrammed station address
            status = m2m_wifi_get_mac_address(interface->macAddr.b);

            //Check status code
            if(status != M2M_SUCCESS)
               break;

            //Generate the 64-bit interface identifier
            macAddrToEui64(&interface->macAddr, &interface->eui64);
         }
         else
         {
            //Override the factory preprogrammed address
            status = m2m_wifi_set_mac_address(interface->macAddr.b);

            //Check status code
            if(status != M2M_SUCCESS)
               break;
         }
      }
      else
      {
         //Initialization was already done
         status = M2M_SUCCESS;
      }

      //STA or AP mode?
      if(interface->nicDriver == &wilc1000StaDriver)
      {
         //Save underlying network interface (STA mode)
         wilc1000StaInterface = interface;

         if(wilc1000ApInterface != NULL)
         {
            wilc1000StaInterface->macAddr = wilc1000ApInterface->macAddr;
            wilc1000StaInterface->eui64 = wilc1000ApInterface->eui64;
         }
      }
      else
      {
         //Save underlying network interface (AP mode)
         wilc1000ApInterface = interface;

         if(wilc1000StaInterface != NULL)
         {
            wilc1000ApInterface->macAddr = wilc1000StaInterface->macAddr;
            wilc1000ApInterface->eui64 = wilc1000StaInterface->eui64;
         }
      }

      //End of exception handling block
   } while(0);

   //WILC1000 is now ready to send
   osSetEvent(&interface->nicTxEvent);

   //Return status code
   if(status == M2M_SUCCESS)
      return NO_ERROR;
   else
      return ERROR_FAILURE;
}


/**
 * @brief WILC1000 timer handler
 *
 * This routine is periodically called by the TCP/IP stack to
 * handle periodic operations such as polling the link state
 *
 * @param[in] interface Underlying network interface
 **/

void wilc1000Tick(NetInterface *interface)
{
}


/**
 * @brief Enable interrupts
 * @param[in] interface Underlying network interface
 **/

void wilc1000EnableIrq(NetInterface *interface)
{
}


/**
 * @brief Disable interrupts
 * @param[in] interface Underlying network interface
 **/

void wilc1000DisableIrq(NetInterface *interface)
{
}


/**
 * @brief WILC1000 interrupt service routine
 * @return TRUE if a higher priority task must be woken. Else FALSE is returned
 **/

bool_t wilc1000IrqHandler(void)
{
   bool_t flag;

   //This flag will be set if a higher priority task must be woken
   flag = FALSE;

   //STA and/or AP mode?
   if(wilc1000StaInterface != NULL)
      wilc1000StaInterface->nicEvent = TRUE;
   else if(wilc1000ApInterface != NULL)
      wilc1000ApInterface->nicEvent = TRUE;

   //Notify the TCP/IP stack of the event
   flag = osSetEventFromIsr(&netEvent);

   //A higher priority task must be woken?
   return flag;
}


/**
 * @brief WILC1000 event handler
 * @param[in] interface Underlying network interface
 **/

void wilc1000EventHandler(NetInterface *interface)
{
   //Process Wi-Fi events
   m2m_wifi_handle_events(NULL);
}


/**
 * @brief Send a packet
 * @param[in] interface Underlying network interface
 * @param[in] buffer Multi-part buffer containing the data to send
 * @param[in] offset Offset to the first data byte
 * @return Error code
 **/

error_t wilc1000SendPacket(NetInterface *interface,
   const NetBuffer *buffer, size_t offset)
{
   int8_t status;
   size_t length;

   //Retrieve the length of the packet
   length = netBufferGetLength(buffer) - offset;

   //Check the frame length
   if(length > WILC1000_TX_BUFFER_SIZE)
   {
      //The transmitter can accept another packet
      osSetEvent(&interface->nicTxEvent);
      //Report an error
      return ERROR_INVALID_LENGTH;
   }

   //Copy user data to the transmit buffer
   netBufferRead(txBuffer, buffer, offset, length);

   //STA or AP mode?
   if(interface == wilc1000StaInterface)
   {
      //Send packet
      status = m2m_wifi_send_ethernet_pkt(txBuffer, length);
   }
   else
   {
      status = m2m_wifi_send_ethernet_pkt_ifc1(txBuffer, length);
   }

   //The transmitter can accept another packet
   osSetEvent(&interface->nicTxEvent);

   //Return status code
   if(status == M2M_SUCCESS)
      return NO_ERROR;
   else
      return ERROR_FAILURE;
}


/**
 * @brief Configure multicast MAC address filtering
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t wilc1000SetMulticastFilter(NetInterface *interface)
{
   uint_t i;
   MacFilterEntry *entry;

   //Debug message
   TRACE_INFO("Updating WILC1000 multicast filter...\r\n");

   //The MAC filter table contains the multicast MAC addresses
   //to accept when receiving an Ethernet frame
   for(i = 0; i < MAC_MULTICAST_FILTER_SIZE; i++)
   {
      //Point to the current entry
      entry = &interface->macMulticastFilter[i];

      //Check whether the MAC filter table should be updated for the
      //current multicast address
      if(!macCompAddr(&entry->addr, &MAC_UNSPECIFIED_ADDR))
      {
         if(entry->addFlag)
         {
            //Add a new entry to the MAC filter table
            m2m_wifi_enable_mac_mcast(entry->addr.b, TRUE);
         }
         else if(entry->deleteFlag)
         {
            //Remove the current entry from the MAC filter table
            m2m_wifi_enable_mac_mcast(entry->addr.b, FALSE);
         }
      }
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Callback function that handles Wi-Fi events
 * @param[in] msgType Type of notification
 * @param[in] msg Pointer to the buffer containing the notification parameters
 **/

void wilc1000AppWifiEvent(uint8_t msgType, void *msg)
{
   tstrM2mWifiStateChanged *stateChangedMsg;

   //Debug message
   TRACE_INFO("WILC1000 Wi-Fi event callback\r\n");

   //Check message type
   if(msgType == M2M_WIFI_RESP_FIRMWARE_STRTED)
   {
      //Debug message
      TRACE_INFO("  M2M_WIFI_RESP_FIRMWARE_STRTED\r\n");
   }
   else if(msgType == M2M_WIFI_RESP_CON_STATE_CHANGED)
   {
      //Debug message
      TRACE_INFO("  M2M_WIFI_RESP_CON_STATE_CHANGED\r\n");

      //Connection state
      stateChangedMsg = (tstrM2mWifiStateChanged*) msg;

      //Check interface identifier
      if(stateChangedMsg->u8IfcId == INTERFACE_1)
      {
         //Check whether STA mode is enabled
         if(wilc1000StaInterface != NULL)
         {
            //Check link state
            if(stateChangedMsg->u8CurrState == M2M_WIFI_CONNECTED)
            {
               //Link is up
               wilc1000StaInterface->linkState = TRUE;
            }
            else
            {
               //Link is down
               wilc1000StaInterface->linkState = FALSE;
            }

            //Process link state change event
            nicNotifyLinkChange(wilc1000StaInterface);
         }
      }
      else if(stateChangedMsg->u8IfcId == INTERFACE_2)
      {
         //Check whether AP mode is enabled
         if(wilc1000ApInterface != NULL)
         {
            //Check link state
            if(stateChangedMsg->u8CurrState == M2M_WIFI_CONNECTED)
            {
               //Link is up
               wilc1000ApInterface->linkState = TRUE;
            }
            else
            {
               //Link is down
               wilc1000ApInterface->linkState = FALSE;
            }

            //Process link state change event
            nicNotifyLinkChange(wilc1000ApInterface);
         }
      }
   }

#if defined(CONF_WILC_EVENT_HOOK)
   //Release exclusive access
   osReleaseMutex(&netMutex);
   //Invoke user callback function
   CONF_WILC_EVENT_HOOK(msgType, msg);
   //Get exclusive access
   osAcquireMutex(&netMutex);
#endif
}


/**
 * @brief Callback function that handles events in bypass mode
 * @param[in] msgType Type of notification
 * @param[in] msg Pointer to the buffer containing the notification parameters
 * @param[in] ctrlBuf Pointer to the control buffer
 **/

void wilc1000AppEthEvent(uint8_t msgType, void *msg, void *ctrlBuf)
{
   size_t n;
   tstrM2mIpCtrlBuf *ctrl;

   //Debug message
   TRACE_DEBUG("WILC1000 RX event callback\r\n");

   //Point to the control buffer
   ctrl = (tstrM2mIpCtrlBuf *) ctrlBuf;

   //Check message type
   if(msgType == M2M_WIFI_RESP_ETHERNET_RX_PACKET)
   {
      //Debug message
      TRACE_DEBUG("  M2M_WIFI_RESP_ETHERNET_RX_PACKET\r\n");

      //Retrieve the length of the packet
      n = ctrl->u16DataSize;

      //CRC is not included in the transfer...
      rxBuffer[n++] = 0xCC;
      rxBuffer[n++] = 0xCC;
      rxBuffer[n++] = 0xCC;
      rxBuffer[n++] = 0xCC;

      //Check interface identifier
      if(ctrl->u8IfcId == INTERFACE_1)
      {
         //Pass the packet to the upper layer (STA mode)
         if(wilc1000StaInterface != NULL)
            nicProcessPacket(wilc1000StaInterface, rxBuffer, n);
      }
      else if(ctrl->u8IfcId == INTERFACE_2)
      {
         //Pass the packet to the upper layer (AP mode)
         if(wilc1000ApInterface != NULL)
            nicProcessPacket(wilc1000ApInterface, rxBuffer, n);
      }
   }
}
