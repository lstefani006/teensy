/**
 * @file mib2_impl.c
 * @brief MIB-II module implementation
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

//Dependencies
#include "core/net.h"
#include "mibs/mib_common.h"
#include "mibs/mib2_module.h"
#include "mibs/mib2_impl.h"
#include "crypto.h"
#include "asn1.h"
#include "oid.h"
#include "debug.h"

//Check TCP/IP stack configuration
#if (MIB2_SUPPORT == ENABLED)


/**
 * @brief MIB-II module initialization
 * @return Error code
 **/

error_t mib2Init(void)
{
   uint_t i;
   Mib2SysGroup *sysGroup;
   Mib2IfGroup *ifGroup;
#if (IPV4_SUPPORT == ENABLED)
   Mib2IpGroup *ipGroup;
#endif
#if (TCP_SUPPORT == ENABLED)
   Mib2TcpGroup *tcpGroup;
#endif

   //Debug message
   TRACE_INFO("Initializing standard MIB-II base...\r\n");

   //Clear MIB-II base
   memset(&mib2Base, 0, sizeof(mib2Base));

   //Point to the system group
   sysGroup = &mib2Base.sysGroup;

   //sysDescr object
   strcpy(sysGroup->sysDescr, "Description");
   sysGroup->sysDescrLen = strlen(sysGroup->sysDescr);
   //sysObjectID object
   sysGroup->sysObjectID[0] = 0;
   sysGroup->sysObjectIDLen = 1;
   //sysContact object
   strcpy(sysGroup->sysContact, "Contact");
   sysGroup->sysContactLen = strlen(sysGroup->sysContact);
   //sysName object
   strcpy(sysGroup->sysName, "Name");
   sysGroup->sysNameLen = strlen(sysGroup->sysName);
   //sysLocation object
   strcpy(sysGroup->sysLocation, "Location");
   sysGroup->sysLocationLen = strlen(sysGroup->sysLocation);
   //sysServices object
   sysGroup->sysServices = MIB2_SYS_SERVICE_INTERNET;

   //Point to the interfaces group
   ifGroup = &mib2Base.ifGroup;

   //Interfaces table entry
   for(i = 0; i < NET_INTERFACE_COUNT; i++)
   {
      //ifSpecific object
      ifGroup->ifTable[i].ifSpecific[0] = 0;
      ifGroup->ifTable[i].ifSpecificLen = 1;
   }

#if (IPV4_SUPPORT == ENABLED)
   //Point to the IP group
   ipGroup = &mib2Base.ipGroup;

   //ipForwarding object
   ipGroup->ipForwarding = MIB2_IP_FORWARDING_DISABLED;
   //ipDefaultTTL object
   ipGroup->ipDefaultTTL = IPV4_DEFAULT_TTL;
   //ipReasmTimeout object
   ipGroup->ipReasmTimeout = IPV4_FRAG_TIME_TO_LIVE / 1000;
#endif

#if (TCP_SUPPORT == ENABLED)
   //Point to the TCP group
   tcpGroup = &mib2Base.tcpGroup;

   //tcpRtoAlgorithm object
   tcpGroup->tcpRtoAlgorithm = MIB2_TCP_RTO_ALGORITHM_VANJ;
   //tcpRtoMin object
   tcpGroup->tcpRtoMin = TCP_MIN_RTO;
   //tcpRtoMax object
   tcpGroup->tcpRtoMax = TCP_MAX_RTO;
   //tcpMaxConn object
   tcpGroup->tcpMaxConn = SOCKET_MAX_COUNT;
#endif

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Lock MIB-II base
 **/

void mib2Lock(void)
{
   //Get exclusive access
   osAcquireMutex(&netMutex);
}


/**
 * @brief Unlock MIB-II base
 **/

void mib2Unlock(void)
{
   //Release exclusive access
   osReleaseMutex(&netMutex);
}


/**
 * @brief Get sysUpTime object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetSysUpTime(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   //Get object value
   value->timeTicks = osGetSystemTime() / 10;
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Get ifEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetIfEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   error_t error;
   size_t n;
   uint_t index;
   Mib2IfEntry *entry;

   //Point to the instance identifier
   n = object->oidLen;

   //The ifIndex is used as instance identifier
   error = mibDecodeIndex(oid, oidLen, &n, &index);
   //Invalid instance identifier?
   if(error) return error;

   //Sanity check
   if(n != oidLen)
      return ERROR_INSTANCE_NOT_FOUND;

   //Check index range
   if(index < 1 || index > NET_INTERFACE_COUNT)
      return ERROR_INSTANCE_NOT_FOUND;

   //Point to the interface table entry
   entry = &mib2Base.ifGroup.ifTable[index - 1];

   //ifIndex object?
   if(!strcmp(object->name, "ifIndex"))
   {
      //Get object value
      value->integer = entry->ifIndex;
   }
   //ifDescr object?
   else if(!strcmp(object->name, "ifDescr"))
   {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->ifDescrLen)
      {
         //Copy object value
         memcpy(value->octetString, entry->ifDescr, entry->ifDescrLen);
         //Return object length
         *valueLen = entry->ifDescrLen;
      }
      else
      {
         //Report an error
         error = ERROR_BUFFER_OVERFLOW;
      }
   }
   //ifType object?
   else if(!strcmp(object->name, "ifType"))
   {
      //Get object value
      value->integer = entry->ifType;
   }
   //ifMtu object?
   else if(!strcmp(object->name, "ifMtu"))
   {
      //Get object value
      value->integer = entry->ifMtu;
   }
   //ifSpeed object?
   else if(!strcmp(object->name, "ifSpeed"))
   {
      //Get object value
      value->gauge32 = entry->ifSpeed;
   }
   //ifPhysAddress object?
   else if(!strcmp(object->name, "ifPhysAddress"))
   {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->ifPhysAddressLen)
      {
         //Copy object value
         memcpy(value->octetString, entry->ifPhysAddress, entry->ifPhysAddressLen);
         //Return object length
         *valueLen = entry->ifPhysAddressLen;
      }
      else
      {
         //Report an error
         error = ERROR_BUFFER_OVERFLOW;
      }
   }
   //ifAdminStatus object?
   else if(!strcmp(object->name, "ifAdminStatus"))
   {
      //Get object value
      value->integer = entry->ifAdminStatus;
   }
   //ifOperStatus object?
   else if(!strcmp(object->name, "ifOperStatus"))
   {
      //Get object value
      value->integer = entry->ifOperStatus;
   }
   //ifLastChange object?
   else if(!strcmp(object->name, "ifLastChange"))
   {
      //Get object value
      value->timeTicks = entry->ifLastChange;
   }
   //ifInOctets object?
   else if(!strcmp(object->name, "ifInOctets"))
   {
      //Get object value
      value->counter32 = entry->ifInOctets;
   }
   //ifInUcastPkts object?
   else if(!strcmp(object->name, "ifInUcastPkts"))
   {
      //Get object value
      value->counter32 = entry->ifInUcastPkts;
   }
   //ifInNUcastPkts object?
   else if(!strcmp(object->name, "ifInNUcastPkts"))
   {
      //Get object value
      value->counter32 = entry->ifInNUcastPkts;
   }
   //ifInDiscards object?
   else if(!strcmp(object->name, "ifInDiscards"))
   {
      //Get object value
      value->counter32 = entry->ifInDiscards;
   }
   //ifInErrors object?
   else if(!strcmp(object->name, "ifInErrors"))
   {
      //Get object value
      value->counter32 = entry->ifInErrors;
   }
   //ifInUnknownProtos object?
   else if(!strcmp(object->name, "ifInUnknownProtos"))
   {
      //Get object value
      value->counter32 = entry->ifInUnknownProtos;
   }
   //ifOutOctets object?
   else if(!strcmp(object->name, "ifOutOctets"))
   {
      //Get object value
      value->counter32 = entry->ifOutOctets;
   }
   //ifOutUcastPkts object?
   else if(!strcmp(object->name, "ifOutUcastPkts"))
   {
      //Get object value
      value->counter32 = entry->ifOutUcastPkts;
   }
   //ifOutNUcastPkts object?
   else if(!strcmp(object->name, "ifOutNUcastPkts"))
   {
      //Get object value
      value->counter32 = entry->ifOutNUcastPkts;
   }
   //ifOutDiscards object?
   else if(!strcmp(object->name, "ifOutDiscards"))
   {
      //Get object value
      value->counter32 = entry->ifOutDiscards;
   }
   //ifOutErrors object?
   else if(!strcmp(object->name, "ifOutErrors"))
   {
      //Get object value
      value->counter32 = entry->ifOutErrors;
   }
   //ifOutQLen object?
   else if(!strcmp(object->name, "ifOutQLen"))
   {
      //Get object value
      value->gauge32 = entry->ifOutQLen;
   }
   //ifSpecific object?
   else if(!strcmp(object->name, "ifSpecific"))
   {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->ifSpecificLen)
      {
         //Copy object value
         memcpy(value->oid, entry->ifSpecific, entry->ifSpecificLen);
         //Return object length
         *valueLen = entry->ifSpecificLen;
      }
      else
      {
         //Report an error
         error = ERROR_BUFFER_OVERFLOW;
      }
   }
   //Unknown object?
   else
   {
      //The specified object does not exist
      error = ERROR_OBJECT_NOT_FOUND;
   }

   //Return status code
   return error;
}


/**
 * @brief Get next ifEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/

error_t mib2GetNextIfEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
   error_t error;
   size_t n;
   uint_t index;

   //Make sure the buffer is large enough to hold the OID prefix
   if(*nextOidLen < object->oidLen)
      return ERROR_BUFFER_OVERFLOW;

   //Copy OID prefix
   memcpy(nextOid, object->oid, object->oidLen);

   //Loop through network interfaces
   for(index = 1; index <= NET_INTERFACE_COUNT; index++)
   {
      //Append the instance identifier to the OID prefix
      n = object->oidLen;

      //The ifIndex is used as instance identifier
      error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
      //Any error to report?
      if(error) return error;

      //Check whether the resulting object identifier lexicographically
      //follows the specified OID
      if(oidComp(nextOid, n, oid, oidLen) > 0)
      {
         //Save the length of the resulting object identifier
         *nextOidLen = n;
         //Next object found
         return NO_ERROR;
      }
   }

   //The specified OID does not lexicographically precede the name
   //of some object
   return ERROR_OBJECT_NOT_FOUND;
}


#if (IPV4_SUPPORT == ENABLED)

/**
 * @brief Get ipAddrEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetIpAddrEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   error_t error;
   uint_t i;
   size_t n;
   Ipv4Addr ipAddr;
   NetInterface *interface;

   //Point to the instance identifier
   n = object->oidLen;

   //The ipAdEntAddr is used as instance identifier
   error = mibDecodeIpv4Addr(oid, oidLen, &n, &ipAddr);
   //Invalid instance identifier?
   if(error) return error;

   //Sanity check
   if(n != oidLen)
      return ERROR_INSTANCE_NOT_FOUND;

   //Loop through network interfaces
   for(i = 0; i < NET_INTERFACE_COUNT; i++)
   {
      //Point to the current interface
      interface = &netInterface[i];

      //Check address state
      if(interface->ipv4Context.addrState == IPV4_ADDR_STATE_VALID)
      {
         //Compare the current address against the IP address used as
         //instance identifier
         if(interface->ipv4Context.addr == ipAddr)
            break;
      }
   }

   //IP address not assigned to any interface?
   if(i >= NET_INTERFACE_COUNT)
      return ERROR_INSTANCE_NOT_FOUND;

   //ipAdEntAddr object?
   if(!strcmp(object->name, "ipAdEntAddr"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &interface->ipv4Context.addr);
   }
   //ipAdEntIfIndex object?
   else if(!strcmp(object->name, "ipAdEntIfIndex"))
   {
      //Get object value
      value->integer = interface->id + 1;
   }
   //ipAdEntNetMask object?
   else if(!strcmp(object->name, "ipAdEntNetMask"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &interface->ipv4Context.subnetMask);
   }
   //ipAdEntBcastAddr object?
   else if(!strcmp(object->name, "ipAdEntBcastAddr"))
   {
      //Get object value
      value->integer = 1;
   }
   //ipAdEntReasmMaxSize object?
   else if(!strcmp(object->name, "ipAdEntReasmMaxSize"))
   {
      //Get object value
      value->integer = IPV4_MAX_FRAG_DATAGRAM_SIZE;
   }
   //Unknown object?
   else
   {
      //The specified object does not exist
      error = ERROR_OBJECT_NOT_FOUND;
   }

   //Return status code
   return error;
}


/**
 * @brief Get next ipAddrEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/

error_t mib2GetNextIpAddrEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
   error_t error;
   uint_t i;
   size_t n;
   Ipv4Addr ipAddr;
   NetInterface *interface;

   //Initialize IP address
   ipAddr = IPV4_UNSPECIFIED_ADDR;

   //Make sure the buffer is large enough to hold the OID prefix
   if(*nextOidLen < object->oidLen)
      return ERROR_BUFFER_OVERFLOW;

   //Copy OID prefix
   memcpy(nextOid, object->oid, object->oidLen);

   //Loop through network interfaces
   for(i = 0; i < NET_INTERFACE_COUNT; i++)
   {
      //Point to the current interface
      interface = &netInterface[i];

      //Check address state
      if(interface->ipv4Context.addrState == IPV4_ADDR_STATE_VALID)
      {
         //Append the instance identifier to the OID prefix
         n = object->oidLen;

         //The ipAdEntAddr is used as instance identifier
         error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, interface->ipv4Context.addr);
         //Any error to report?
         if(error) return error;

         //Check whether the resulting object identifier lexicographically
         //follows the specified OID
         if(oidComp(nextOid, n, oid, oidLen) > 0)
         {
            //Save the closest object identifier that follows the specified
            //OID in lexicographic order
            if(ipAddr == IPV4_UNSPECIFIED_ADDR)
            {
               ipAddr = interface->ipv4Context.addr;
            }
            else if(ntohl(interface->ipv4Context.addr) < ntohl(ipAddr))
            {
               ipAddr = interface->ipv4Context.addr;
            }
         }
      }
   }

   //The specified OID does not lexicographically precede the name
   //of some object?
   if(ipAddr == IPV4_UNSPECIFIED_ADDR)
      return ERROR_OBJECT_NOT_FOUND;

   //Append the instance identifier to the OID prefix
   n = object->oidLen;

   //The ipAdEntAddr is used as instance identifier
   error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, interface->ipv4Context.addr);
   //Any error to report?
   if(error) return error;

   //Save the length of the resulting object identifier
   *nextOidLen = n;
   //Next object found
   return NO_ERROR;
}


/**
 * @brief Set ipNetToMediaEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2SetIpNetToMediaEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen)
{
   //Not implemented
   return ERROR_WRITE_FAILED;
}


/**
 * @brief Get ipNetToMediaEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetIpNetToMediaEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   error_t error;
   size_t n;
   uint_t index;
   Ipv4Addr ipAddr;
   NetInterface *interface;
   ArpCacheEntry *entry;

   //Point to the instance identifier
   n = object->oidLen;

   //The ipNetToMediaIfIndex is used as 1st instance identifier
   error = mibDecodeIndex(oid, oidLen, &n, &index);
   //Invalid instance identifier?
   if(error) return error;

   //The ipNetToMediaNetAddress is used as 2nd instance identifier
   error = mibDecodeIpv4Addr(oid, oidLen, &n, &ipAddr);
   //Invalid instance identifier?
   if(error) return error;

   //Sanity check
   if(n != oidLen)
      return ERROR_INSTANCE_NOT_FOUND;

   //Check index range
   if(index < 1 || index > NET_INTERFACE_COUNT)
      return ERROR_INSTANCE_NOT_FOUND;

   //Point to the network interface
   interface = &netInterface[index - 1];

   //Search the ARP cache for the specified IP address
   entry = arpFindEntry(interface, ipAddr);

   //No matching entry found?
   if(entry == NULL)
      return ERROR_INSTANCE_NOT_FOUND;

   //ipNetToMediaIfIndex object?
   if(!strcmp(object->name, "ipNetToMediaIfIndex"))
   {
      //Get object value
      value->integer = index;
   }
   //ipNetToMediaPhysAddress object?
   else if(!strcmp(object->name, "ipNetToMediaPhysAddress"))
   {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= MIB2_PHYS_ADDRESS_SIZE)
      {
         //Copy object value
         macCopyAddr(value->octetString, &entry->macAddr);
         //Return object length
         *valueLen = MIB2_PHYS_ADDRESS_SIZE;
      }
      else
      {
         //Report an error
         error = ERROR_BUFFER_OVERFLOW;
      }
   }
   //ipNetToMediaNetAddress object?
   else if(!strcmp(object->name, "ipNetToMediaNetAddress"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &entry->ipAddr);
   }
   //ipNetToMediaType object?
   else if(!strcmp(object->name, "ipNetToMediaType"))
   {
      //Get object value
      value->integer = MIB2_IP_NET_TO_MEDIA_TYPE_DYNAMIC;
   }
   //Unknown object?
   else
   {
      //The specified object does not exist
      error = ERROR_OBJECT_NOT_FOUND;
   }

   //Return status code
   return error;
}


/**
 * @brief Get next ipNetToMediaEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/

error_t mib2GetNextIpNetToMediaEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
   error_t error;
   uint_t i;
   uint_t j;
   size_t n;
   uint32_t index;
   bool_t acceptable;
   Ipv4Addr ipAddr;
   NetInterface *interface;
   ArpCacheEntry *entry;

   //Initialize variables
   index = 0;
   ipAddr = IPV4_UNSPECIFIED_ADDR;

   //Make sure the buffer is large enough to hold the OID prefix
   if(*nextOidLen < object->oidLen)
      return ERROR_BUFFER_OVERFLOW;

   //Copy OID prefix
   memcpy(nextOid, object->oid, object->oidLen);

   //Loop through network interfaces
   for(i = 1; i <= NET_INTERFACE_COUNT; i++)
   {
      //Point to the current interface
      interface = &netInterface[i - 1];

      //Loop through ARP cache entries
      for(j = 0; j < ARP_CACHE_SIZE; j++)
      {
         //Point to the current entry
         entry = &interface->arpCache[j];

         //Valid entry?
         if(entry->state != ARP_STATE_NONE)
         {
            //Append the instance identifier to the OID prefix
            n = object->oidLen;

            //The ipNetToMediaIfIndex is used as 1st instance identifier
            error = mibEncodeIndex(nextOid, *nextOidLen, &n, i);
            //Any error to report?
            if(error) return error;

            //The ipNetToMediaNetAddress is used as 2nd instance identifier
            error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, entry->ipAddr);
            //Any error to report?
            if(error) return error;

            //Check whether the resulting object identifier lexicographically
            //follows the specified OID
            if(oidComp(nextOid, n, oid, oidLen) > 0)
            {
               //Perform lexicographic comparison
               if(index == 0)
                  acceptable = TRUE;
               else if(i < index)
                  acceptable = TRUE;
               else if(i > index)
                  acceptable = FALSE;
               else if(ntohl(entry->ipAddr) < ntohl(ipAddr))
                  acceptable = TRUE;
               else
                  acceptable = FALSE;

               //Save the closest object identifier that follows the specified
               //OID in lexicographic order
               if(acceptable)
               {
                  index = i;
                  ipAddr = entry->ipAddr;
               }
            }
         }
      }
   }

   //The specified OID does not lexicographically precede the name
   //of some object?
   if(index == 0)
      return ERROR_OBJECT_NOT_FOUND;

   //Append the instance identifier to the OID prefix
   n = object->oidLen;

   //The ipNetToMediaIfIndex is used as 1st instance identifier
   error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
   //Any error to report?
   if(error) return error;

   //The ipNetToMediaNetAddress is used as 2nd instance identifier
   error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, ipAddr);
   //Any error to report?
   if(error) return error;

   //Save the length of the resulting object identifier
   *nextOidLen = n;
   //Next object found
   return NO_ERROR;
}


/**
 * @brief Set tcpConnEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2SetTcpConnEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen)
{
   //Not implemented
   return ERROR_WRITE_FAILED;
}


/**
 * @brief Get tcpConnEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetTcpConnEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   error_t error;
   uint_t i;
   size_t n;
   Ipv4Addr localIpAddr;
   uint16_t localPort;
   Ipv4Addr remoteIpAddr;
   uint16_t remotePort;
   Socket *socket;

   //Point to the instance identifier
   n = object->oidLen;

   //The tcpConnLocalAddress is used as 1st instance identifier
   error = mibDecodeIpv4Addr(oid, oidLen, &n, &localIpAddr);
   //Invalid instance identifier?
   if(error) return error;

   //The tcpConnLocalPort is used as 2nd instance identifier
   error = mibDecodePort(oid, oidLen, &n, &localPort);
   //Invalid instance identifier?
   if(error) return error;

   //The tcpConnRemAddress is used as 3rd instance identifier
   error = mibDecodeIpv4Addr(oid, oidLen, &n, &remoteIpAddr);
   //Invalid instance identifier?
   if(error) return error;

   //The tcpConnRemPort is used as 4th instance identifier
   error = mibDecodePort(oid, oidLen, &n, &remotePort);
   //Invalid instance identifier?
   if(error) return error;

   //Sanity check
   if(n != oidLen)
      return ERROR_INSTANCE_NOT_FOUND;

   //Loop through socket descriptors
   for(i = 0; i < SOCKET_MAX_COUNT; i++)
   {
      //Point to current socket
      socket = &socketTable[i];

      //TCP socket?
      if(socketTable[i].type == SOCKET_TYPE_STREAM)
      {
         //Check local IP address
         if(socket->localIpAddr.length == sizeof(Ipv6Addr))
            continue;
         if(socket->localIpAddr.ipv4Addr != localIpAddr)
            continue;
         //Check local port number
         if(socket->localPort != localPort)
            continue;
         //Check remote IP address
         if(socket->remoteIpAddr.length == sizeof(Ipv6Addr))
            continue;
         if(socket->remoteIpAddr.ipv4Addr != remoteIpAddr)
            continue;
         //Check remote port number
         if(socket->remotePort != remotePort)
            continue;

         //A matching socket has been found
         break;
      }
   }

   //No matching connection found in socket table?
   if(i >= SOCKET_MAX_COUNT)
      return ERROR_INSTANCE_NOT_FOUND;

   //tcpConnState object?
   if(!strcmp(object->name, "tcpConnState"))
   {
      //Get object value
      switch(socket->state)
      {
      case TCP_STATE_CLOSED:
         value->integer = MIB2_TCP_CONN_STATE_CLOSED;
         break;
      case TCP_STATE_LISTEN:
         value->integer = MIB2_TCP_CONN_STATE_LISTEN;
         break;
      case TCP_STATE_SYN_SENT:
         value->integer = MIB2_TCP_CONN_STATE_SYN_SENT;
         break;
      case TCP_STATE_SYN_RECEIVED:
         value->integer = MIB2_TCP_CONN_STATE_SYN_RECEIVED;
         break;
      case TCP_STATE_ESTABLISHED:
         value->integer = MIB2_TCP_CONN_STATE_ESTABLISHED;
         break;
      case TCP_STATE_FIN_WAIT_1:
         value->integer = MIB2_TCP_CONN_STATE_FIN_WAIT_1;
         break;
      case TCP_STATE_FIN_WAIT_2:
         value->integer = MIB2_TCP_CONN_STATE_FIN_WAIT_2;
         break;
      case TCP_STATE_CLOSE_WAIT:
         value->integer = MIB2_TCP_CONN_STATE_CLOSE_WAIT;
         break;
      case TCP_STATE_LAST_ACK:
         value->integer = MIB2_TCP_CONN_STATE_LAST_ACK;
         break;
      case TCP_STATE_CLOSING:
         value->integer = MIB2_TCP_CONN_STATE_CLOSING;
         break;
      case TCP_STATE_TIME_WAIT:
         value->integer = MIB2_TCP_CONN_STATE_TIME_WAIT;
      default:
         value->integer = 0;
         break;
      }
   }
   //tcpConnLocalAddress object?
   else if(!strcmp(object->name, "tcpConnLocalAddress"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &socket->localIpAddr.ipv4Addr);
   }
   //tcpConnLocalPort object?
   else if(!strcmp(object->name, "tcpConnLocalPort"))
   {
      //Get object value
      value->integer = socket->localPort;
   }
   //tcpConnRemAddress object?
   else if(!strcmp(object->name, "tcpConnRemAddress"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &socket->remoteIpAddr.ipv4Addr);
   }
   //tcpConnRemPort object?
   else if(!strcmp(object->name, "tcpConnRemPort"))
   {
      //Get object value
      value->integer = socket->remotePort;
   }
   //Unknown object?
   else
   {
      //The specified object does not exist
      error = ERROR_OBJECT_NOT_FOUND;
   }

   //Return status code
   return error;
}


/**
 * @brief Get next tcpConnEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/

error_t mib2GetNextTcpConnEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
   error_t error;
   uint_t i;
   size_t n;
   bool_t acceptable;
   Ipv4Addr localIpAddr;
   uint16_t localPort;
   Ipv4Addr remoteIpAddr;
   uint16_t remotePort;
   Socket *socket;

   //Initialize variables
   localIpAddr = IPV4_UNSPECIFIED_ADDR;
   localPort = 0;
   remoteIpAddr = IPV4_UNSPECIFIED_ADDR;
   remotePort = 0;

   //Make sure the buffer is large enough to hold the OID prefix
   if(*nextOidLen < object->oidLen)
      return ERROR_BUFFER_OVERFLOW;

   //Copy OID prefix
   memcpy(nextOid, object->oid, object->oidLen);

   //Loop through socket descriptors
   for(i = 0; i < SOCKET_MAX_COUNT; i++)
   {
      //Point to current socket
      socket = &socketTable[i];

      //TCP socket?
      if(socketTable[i].type == SOCKET_TYPE_STREAM)
      {
         //Filter out IPv6 connections
         if(socket->localIpAddr.length != sizeof(Ipv6Addr) &&
            socket->remoteIpAddr.length != sizeof(Ipv6Addr))
         {
            //Append the instance identifier to the OID prefix
            n = object->oidLen;

            //The tcpConnLocalAddress is used as 1st instance identifier
            error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, socket->localIpAddr.ipv4Addr);
            //Any error to report?
            if(error) return error;

            //The tcpConnLocalPort is used as 2nd instance identifier
            error = mibEncodePort(nextOid, *nextOidLen, &n, socket->localPort);
            //Any error to report?
            if(error) return error;

            //The tcpConnRemAddress is used as 3rd instance identifier
            error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, socket->remoteIpAddr.ipv4Addr);
            //Any error to report?
            if(error) return error;

            //The tcpConnRemPort is used as 4th instance identifier
            error = mibEncodePort(nextOid, *nextOidLen, &n, socket->remotePort);
            //Any error to report?
            if(error) return error;

            //Check whether the resulting object identifier lexicographically
            //follows the specified OID
            if(oidComp(nextOid, n, oid, oidLen) > 0)
            {
               //Perform lexicographic comparison
               if(localPort == 0 && remotePort == 0)
                  acceptable = TRUE;
               else if(ntohl(socket->localIpAddr.ipv4Addr) < ntohl(localIpAddr))
                  acceptable = TRUE;
               else if(ntohl(socket->localIpAddr.ipv4Addr) > ntohl(localIpAddr))
                  acceptable = FALSE;
               else if(socket->localPort < localPort)
                  acceptable = TRUE;
               else if(socket->localPort > localPort)
                  acceptable = FALSE;
               else if(ntohl(socket->remoteIpAddr.ipv4Addr) < ntohl(remoteIpAddr))
                  acceptable = TRUE;
               else if(ntohl(socket->remoteIpAddr.ipv4Addr) > ntohl(remoteIpAddr))
                  acceptable = FALSE;
               else if(socket->remotePort < remotePort)
                  acceptable = TRUE;
               else
                  acceptable = FALSE;

               //Save the closest object identifier that follows the specified
               //OID in lexicographic order
               if(acceptable)
               {
                  localIpAddr = socket->localIpAddr.ipv4Addr;
                  localPort = socket->localPort;
                  remoteIpAddr = socket->remoteIpAddr.ipv4Addr;
                  remotePort = socket->remotePort;
               }
            }
         }
      }
   }

   //The specified OID does not lexicographically precede the name
   //of some object?
   if(localPort == 0 && remotePort == 0)
      return ERROR_OBJECT_NOT_FOUND;

   //Append the instance identifier to the OID prefix
   n = object->oidLen;

   //The tcpConnLocalAddress is used as 1st instance identifier
   error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, localIpAddr);
   //Any error to report?
   if(error) return error;

   //The tcpConnLocalPort is used as 2nd instance identifier
   error = mibEncodePort(nextOid, *nextOidLen, &n, localPort);
   //Any error to report?
   if(error) return error;

   //The tcpConnRemAddress is used as 3rd instance identifier
   error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, remoteIpAddr);
   //Any error to report?
   if(error) return error;

   //The tcpConnRemPort is used as 4th instance identifier
   error = mibEncodePort(nextOid, *nextOidLen, &n, remotePort);
   //Any error to report?
   if(error) return error;

   //Save the length of the resulting object identifier
   *nextOidLen = n;
   //Next object found
   return NO_ERROR;
}


/**
 * @brief Get udpEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t mib2GetUdpEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen)
{
   error_t error;
   uint_t i;
   size_t n;
   Ipv4Addr localIpAddr;
   uint16_t localPort;

   //Point to the instance identifier
   n = object->oidLen;

   //The udpLocalAddress is used as 1st instance identifier
   error = mibDecodeIpv4Addr(oid, oidLen, &n, &localIpAddr);
   //Invalid instance identifier?
   if(error) return error;

   //The udpLocalPort is used as 2nd instance identifier
   error = mibDecodePort(oid, oidLen, &n, &localPort);
   //Invalid instance identifier?
   if(error) return error;

   //Sanity check
   if(n != oidLen)
      return ERROR_INSTANCE_NOT_FOUND;

   //Loop through socket descriptors
   for(i = 0; i < SOCKET_MAX_COUNT; i++)
   {
      //Point to current socket
      Socket *socket = &socketTable[i];

      //UDP socket?
      if(socketTable[i].type == SOCKET_TYPE_DGRAM)
      {
         //Check local IP address
         if(socket->localIpAddr.length == sizeof(Ipv6Addr))
            continue;
         if(socket->localIpAddr.ipv4Addr != localIpAddr)
            continue;
         //Check local port number
         if(socket->localPort != localPort)
            continue;

         //A matching socket has been found
         break;
      }
   }

   //No matching connection found in socket table?
   if(i >= SOCKET_MAX_COUNT)
   {
      //Loop through the UDP callback table
      for(i = 0; i < UDP_CALLBACK_TABLE_SIZE; i++)
      {
         //Point to the current entry
         UdpRxCallbackDesc *entry = &udpCallbackTable[i];

         //Check whether the entry is currently in used
         if(entry->callback != NULL)
         {
            //Check local port number
            if(entry->port == localPort)
               break;
         }
      }

      //No matching connection found in UDP callback table?
      if(i >= UDP_CALLBACK_TABLE_SIZE)
         return ERROR_INSTANCE_NOT_FOUND;
   }

   //udpLocalAddress object?
   if(!strcmp(object->name, "udpLocalAddress"))
   {
      //Get object value
      ipv4CopyAddr(value->ipAddr, &localIpAddr);
   }
   //udpLocalPort object?
   else if(!strcmp(object->name, "udpLocalPort"))
   {
      //Get object value
      value->integer = localPort;
   }
   //Unknown object?
   else
   {
      //The specified object does not exist
      error = ERROR_OBJECT_NOT_FOUND;
   }

   //Return status code
   return error;
}


/**
 * @brief Get next udpEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/

error_t mib2GetNextUdpEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
   error_t error;
   uint_t i;
   size_t n;
   bool_t acceptable;
   Ipv4Addr localIpAddr;
   uint16_t localPort;

   //Initialize variables
   localIpAddr = IPV4_UNSPECIFIED_ADDR;
   localPort = 0;

   //Make sure the buffer is large enough to hold the OID prefix
   if(*nextOidLen < object->oidLen)
      return ERROR_BUFFER_OVERFLOW;

   //Copy OID prefix
   memcpy(nextOid, object->oid, object->oidLen);

   //Loop through socket descriptors
   for(i = 0; i < SOCKET_MAX_COUNT; i++)
   {
      //Point to current socket
      Socket *socket = &socketTable[i];

      //TCP socket?
      if(socketTable[i].type == SOCKET_TYPE_DGRAM)
      {
         //Filter out IPv6 connections
         if(socket->localIpAddr.length != sizeof(Ipv6Addr) &&
            socket->remoteIpAddr.length != sizeof(Ipv6Addr))
         {
            //Append the instance identifier to the OID prefix
            n = object->oidLen;

            //The udpLocalAddress is used as 1st instance identifier
            error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, socket->localIpAddr.ipv4Addr);
            //Any error to report?
            if(error) return error;

            //The udpLocalPort is used as 2nd instance identifier
            error = mibEncodePort(nextOid, *nextOidLen, &n, socket->localPort);
            //Any error to report?
            if(error) return error;

            //Check whether the resulting object identifier lexicographically
            //follows the specified OID
            if(oidComp(nextOid, n, oid, oidLen) > 0)
            {
               //Perform lexicographic comparison
               if(localPort == 0)
                  acceptable = TRUE;
               else if(ntohl(socket->localIpAddr.ipv4Addr) < ntohl(localIpAddr))
                  acceptable = TRUE;
               else if(ntohl(socket->localIpAddr.ipv4Addr) > ntohl(localIpAddr))
                  acceptable = FALSE;
               else if(socket->localPort < localPort)
                  acceptable = TRUE;
               else
                  acceptable = FALSE;

               //Save the closest object identifier that follows the specified
               //OID in lexicographic order
               if(acceptable)
               {
                  localIpAddr = socket->localIpAddr.ipv4Addr;
                  localPort = socket->localPort;
               }
            }
         }
      }
   }

   //Loop through the UDP callback table
   for(i = 0; i < UDP_CALLBACK_TABLE_SIZE; i++)
   {
      //Point to the current entry
      UdpRxCallbackDesc *entry = &udpCallbackTable[i];

      //Check whether the entry is currently in used
      if(entry->callback != NULL)
      {
         //Append the instance identifier to the OID prefix
         n = object->oidLen;

         //The udpLocalAddress is used as 1st instance identifier
         error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, IPV4_UNSPECIFIED_ADDR);
         //Any error to report?
         if(error) return error;

         //The udpLocalPort is used as 2nd instance identifier
         error = mibEncodePort(nextOid, *nextOidLen, &n, entry->port);
         //Any error to report?
         if(error) return error;

         //Check whether the resulting object identifier lexicographically
         //follows the specified OID
         if(oidComp(nextOid, n, oid, oidLen) > 0)
         {
            //Perform lexicographic comparison
            if(localPort == 0)
               acceptable = TRUE;
            else if(ntohl(IPV4_UNSPECIFIED_ADDR) < ntohl(localIpAddr))
               acceptable = TRUE;
            else if(ntohl(IPV4_UNSPECIFIED_ADDR) > ntohl(localIpAddr))
               acceptable = FALSE;
            else if(entry->port < localPort)
               acceptable = TRUE;
            else
               acceptable = FALSE;

            //Save the closest object identifier that follows the specified
            //OID in lexicographic order
            if(acceptable)
            {
               localIpAddr = IPV4_UNSPECIFIED_ADDR;
               localPort = entry->port;
            }
         }
      }
   }

   //The specified OID does not lexicographically precede the name
   //of some object?
   if(localPort == 0)
      return ERROR_OBJECT_NOT_FOUND;

   //Append the instance identifier to the OID prefix
   n = object->oidLen;

   //The udpLocalAddress is used as 1st instance identifier
   error = mibEncodeIpv4Addr(nextOid, *nextOidLen, &n, localIpAddr);
   //Any error to report?
   if(error) return error;

   //The udpLocalPort is used as 2nd instance identifier
   error = mibEncodePort(nextOid, *nextOidLen, &n, localPort);
   //Any error to report?
   if(error) return error;

   //Save the length of the resulting object identifier
   *nextOidLen = n;
   //Next object found
   return NO_ERROR;
}

#endif
#endif
