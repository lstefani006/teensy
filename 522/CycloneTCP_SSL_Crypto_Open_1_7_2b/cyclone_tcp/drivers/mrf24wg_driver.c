/**
 * @file mrf24wg_driver.c
 * @brief MRF24WG Wi-Fi controller
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
#include "core/net.h"
#include "mrf24wg_driver.h"
#include "debug.h"

//MRF24WG universal driver
#include "wf_universal_driver.h"
#include "wf_debug_output.h"

//Underlying network interface
static NetInterface *nicDriverInterface;

//Transmit buffer
static Mrf24wgBuffer txBuffer[WF_TX_QUEUE_SIZE];
//Receive buffer
static uint8_t rxBuffer[MRF24WG_RX_BUFFER_SIZE];


/**
 * @brief MRF24WG driver
 **/

const NicDriver mrf24wgDriver =
{
   NIC_TYPE_ETHERNET,
   ETH_MTU,
   mrf24wgInit,
   mrf24wgTick,
   mrf24wgEnableIrq,
   mrf24wgDisableIrq,
   mrf24wgEventHandler,
   mrf24wgSendPacket,
   mrf24wgSetMulticastFilter,
   NULL,
   NULL,
   NULL,
   TRUE,
   TRUE,
   TRUE
};


/**
 * @brief MRF24WG initialization
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t mrf24wgInit(NetInterface *interface)
{
   //Debug message
   TRACE_INFO("Initializing MRF24WG...\r\n");

   //Save underlying network interface
   nicDriverInterface = interface;

   //Clear TX buffers
   memset(txBuffer, 0, sizeof(txBuffer));

   //Initialize MRF24WG controller
   WF_Init();

   //MRF24WG is now ready to send
   osSetEvent(&interface->nicTxEvent);

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief MRF24WG timer handler
 *
 * This routine is periodically called by the TCP/IP stack to
 * handle periodic operations such as polling the link state
 *
 * @param[in] interface Underlying network interface
 **/

void mrf24wgTick(NetInterface *interface)
{
}


/**
 * @brief Enable interrupts
 * @param[in] interface Underlying network interface
 **/

void mrf24wgEnableIrq(NetInterface *interface)
{
}


/**
 * @brief Disable interrupts
 * @param[in] interface Underlying network interface
 **/

void mrf24wgDisableIrq(NetInterface *interface)
{
}


/**
 * @brief MRF24WG event handler
 * @param[in] interface Underlying network interface
 **/

void mrf24wgEventHandler(NetInterface *interface)
{
}


/**
 * @brief Send a packet
 * @param[in] interface Underlying network interface
 * @param[in] buffer Multi-part buffer containing the data to send
 * @param[in] offset Offset to the first data byte
 * @return Error code
 **/

error_t mrf24wgSendPacket(NetInterface *interface,
   const NetBuffer *buffer, size_t offset)
{
   bool_t status;
   uint_t i;
   size_t length;

   //Retrieve the length of the packet
   length = netBufferGetLength(buffer) - offset;

   //Check the frame length
   if(length > MRF24WG_TX_BUFFER_SIZE)
   {
      //The transmitter can accept another packet
      osSetEvent(&interface->nicTxEvent);
      //Report an error
      return ERROR_INVALID_LENGTH;
   }

   //Loop through TX buffers
   for(i = 0; i < WF_TX_QUEUE_SIZE; i++)
   {
      //Check whether the current buffer is available
      if(!txBuffer[i].used)
         break;
   }

   //Any buffer available?
   if(i < WF_TX_QUEUE_SIZE)
   {
      //Save packet length
      txBuffer[i].length = length;

      //Copy user data to the transmit buffer
      netBufferRead(txBuffer[i].data, buffer, offset, length);

      //Enqueue packet
      status = WF_QueueTxPacket(txBuffer[i].data, length);

      //Check status code
      if(status)
         txBuffer[i].used = TRUE;
   }
   else
   {
      //No buffer available
      status = FALSE;
   }

   //The transmitter can accept another packet
   osSetEvent(&nicDriverInterface->nicTxEvent);

   //Return status code
   if(status)
      return NO_ERROR;
   else
      return ERROR_FAILURE;
}


/**
 * @brief Configure multicast MAC address filtering
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t mrf24wgSetMulticastFilter(NetInterface *interface)
{
   uint_t i;
   MacFilterEntry *entry;

   //Debug message
   TRACE_INFO("Updating MRF24WG multicast filter...\r\n");

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
         }
         else if(entry->deleteFlag)
         {
            //Remove the current entry from the MAC filter table
         }
      }
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Callback function that handles Wi-Fi events
 * @param[in] eventType Type of notification
 * @param[in] eventData Event data
 **/

void WF_ProcessEvent(uint8_t eventType, uint32_t eventData)
{
#if defined(WF_USE_DEBUG_OUTPUT)
   //Debug message
   DumpEventInfo(eventType, eventData);
#endif

   //Check event type
   switch(eventType)
   {
   //Initialization complete?
   case WF_EVENT_INITIALIZATION:
      //Use the factory preprogrammed station address
      WF_MacAddressGet(nicDriverInterface->macAddr.b);
      //Generate the 64-bit interface identifier
      macAddrToEui64(&nicDriverInterface->macAddr, &nicDriverInterface->eui64);
      break;

   //Connection established?
   case WF_EVENT_CONNECTION_SUCCESSFUL:
   case WF_EVENT_CONNECTION_REESTABLISHED:
   case WF_EVENT_SOFTAP_NETWORK_STARTED:
      //Link is up
      nicDriverInterface->linkState = TRUE;
      //Process link state change event
      nicNotifyLinkChange(nicDriverInterface);
      break;

   //Connection lost?
   case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
   case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
   case WF_EVENT_CONNECTION_FAILED:
   case WF_EVENT_DISCONNECT_COMPLETE:
      //Link is down
      nicDriverInterface->linkState = FALSE;
      //Process link state change event
      nicNotifyLinkChange(nicDriverInterface);
      break;

   //Any other event?
   default:
      break;
   }

#if defined(MRF24WG_EVENT_HOOK)
   //Invoke user callback function
   MRF24WG_EVENT_HOOK(eventType, eventData);
#endif
}


/**
 * @brief Callback function (packet received)
 **/

void WF_RxPacketReady(void)
{
   size_t n;

   //Retrieve the length of the packet
   n = WF_RxPacketLengthGet();

   //Copy the packet to the receive buffer
   WF_RxPacketCopy(rxBuffer, n);

   //CRC is not included in the transfer...
   rxBuffer[n++] = 0xCC;
   rxBuffer[n++] = 0xCC;
   rxBuffer[n++] = 0xCC;
   rxBuffer[n++] = 0xCC;

   //Pass the packet to the upper layer
   nicProcessPacket(nicDriverInterface, rxBuffer, n);

   //Release the packet
   WF_RxPacketDeallocate();
}


/**
 * @brief Callback function (packet transmitted)
 **/

void WF_TxComplete(uint8_t *p)
{
   uint_t i;

   //Loop through TX buffers
   for(i = 0; i < WF_TX_QUEUE_SIZE; i++)
   {
      if(txBuffer[i].data == p)
      {
         //Release current buffer
         txBuffer[i].used = FALSE;
      }
   }

   //The transmitter can accept another packet
   osSetEvent(&nicDriverInterface->nicTxEvent);
}
