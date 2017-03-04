/**
 * @file dhcp_client.h
 * @brief DHCP client (Dynamic Host Configuration Protocol)
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

#ifndef _DHCP_CLIENT_H
#define _DHCP_CLIENT_H

//Dependencies
#include "core/net.h"
#include "core/socket.h"
#include "core/udp.h"
#include "dhcp/dhcp_common.h"

//DHCP client support
#ifndef DHCP_CLIENT_SUPPORT
   #define DHCP_CLIENT_SUPPORT ENABLED
#elif (DHCP_CLIENT_SUPPORT != ENABLED && DHCP_CLIENT_SUPPORT != DISABLED)
   #error DHCP_CLIENT_SUPPORT parameter is not valid
#endif

//DHCP client tick interval
#ifndef DHCP_CLIENT_TICK_INTERVAL
   #define DHCP_CLIENT_TICK_INTERVAL 200
#elif (DHCP_CLIENT_TICK_INTERVAL < 10)
   #error DHCP_CLIENT_TICK_INTERVAL parameter is not valid
#endif

//Maximum length of host name
#ifndef DHCP_CLIENT_MAX_HOSTNAME_LEN
   #define DHCP_CLIENT_MAX_HOSTNAME_LEN 15
#elif (DHCP_CLIENT_MAX_HOSTNAME_LEN < 1)
   #error DHCP_CLIENT_MAX_HOSTNAME_LEN parameter is not valid
#endif

//Random delay before sending the first message
#ifndef DHCP_CLIENT_INIT_DELAY
   #define DHCP_CLIENT_INIT_DELAY 2000
#elif (DHCP_CLIENT_INIT_DELAY < 0)
   #error DHCP_CLIENT_INIT_DELAY parameter is not valid
#endif

//Initial retransmission timeout (DHCPDISCOVER)
#ifndef DHCP_CLIENT_DISCOVER_INIT_RT
   #define DHCP_CLIENT_DISCOVER_INIT_RT 4000
#elif (DHCP_CLIENT_DISCOVER_INIT_RT < 1000)
   #error DHCP_CLIENT_DISCOVER_INIT_RT parameter is not valid
#endif

//Maximum retransmission timeout (DHCPDISCOVER)
#ifndef DHCP_CLIENT_DISCOVER_MAX_RT
   #define DHCP_CLIENT_DISCOVER_MAX_RT 16000
#elif (DHCP_CLIENT_DISCOVER_MAX_RT < 1000)
   #error DHCP_CLIENT_DISCOVER_MAX_RT parameter is not valid
#endif

//Maximum retransmission count (DHCPREQUEST)
#ifndef DHCP_CLIENT_REQUEST_MAX_RC
   #define DHCP_CLIENT_REQUEST_MAX_RC 4
#elif (DHCP_CLIENT_REQUEST_MAX_RC < 1)
   #error DHCP_CLIENT_REQUEST_MAX_RC parameter is not valid
#endif

//Initial retransmission timeout (DHCPREQUEST)
#ifndef DHCP_CLIENT_REQUEST_INIT_RT
   #define DHCP_CLIENT_REQUEST_INIT_RT 4000
#elif (DHCP_CLIENT_REQUEST_INIT_RT < 1000)
   #error DHCP_CLIENT_REQUEST_INIT_RT parameter is not valid
#endif

//Maximum retransmission timeout (DHCPREQUEST)
#ifndef DHCP_CLIENT_REQUEST_MAX_RT
   #define DHCP_CLIENT_REQUEST_MAX_RT 64000
#elif (DHCP_CLIENT_REQUEST_MAX_RT < 1000)
   #error DHCP_CLIENT_REQUEST_MAX_RT parameter is not valid
#endif

//Minimum delay between DHCPREQUEST messages in RENEWING and REBINDING states
#ifndef DHCP_CLIENT_REQUEST_MIN_DELAY
   #define DHCP_CLIENT_REQUEST_MIN_DELAY 60000
#elif (DHCP_CLIENT_REQUEST_MIN_DELAY < 1000)
   #error DHCP_CLIENT_REQUEST_MIN_DELAY parameter is not valid
#endif

//Number of probe packets
#ifndef DHCP_CLIENT_PROBE_NUM
   #define DHCP_CLIENT_PROBE_NUM 1
#elif (DHCP_CLIENT_PROBE_NUM < 0)
   #error DHCP_CLIENT_PROBE_NUM parameter is not valid
#endif

//Delay until repeated probe
#ifndef DHCP_CLIENT_PROBE_DELAY
   #define DHCP_CLIENT_PROBE_DELAY 1000
#elif (DHCP_CLIENT_PROBE_DELAY < 100)
   #error DHCP_CLIENT_PROBE_DELAY parameter is not valid
#endif

//Random factor used to determine the delay between retransmissions
#ifndef DHCP_CLIENT_RAND_FACTOR
   #define DHCP_CLIENT_RAND_FACTOR 1000
#elif (DHCP_CLIENT_RAND_FACTOR < 100)
   #error DHCP_CLIENT_RAND_FACTOR parameter is not valid
#endif

//Forward declaration of DhcpClientCtx structure
struct _DhcpClientCtx;
#define DhcpClientCtx struct _DhcpClientCtx


/**
 * @brief DHCP FSM states
 **/

typedef enum
{
   DHCP_STATE_INIT        = 0,
   DHCP_STATE_SELECTING   = 1,
   DHCP_STATE_REQUESTING  = 2,
   DHCP_STATE_INIT_REBOOT = 3,
   DHCP_STATE_REBOOTING   = 4,
   DHCP_STATE_PROBING     = 5,
   DHCP_STATE_BOUND       = 6,
   DHCP_STATE_RENEWING    = 7,
   DHCP_STATE_REBINDING   = 8
} DhcpState;


/**
 * @brief DHCP configuration timeout callback
 **/

typedef void (*DhcpTimeoutCallback)(DhcpClientCtx *context,
   NetInterface *interface);


/**
 * @brief Link state change callback
 **/

typedef void (*DhcpLinkChangeCallback)(DhcpClientCtx *context,
   NetInterface *interface, bool_t linkState);


/**
 * @brief FSM state change callback
 **/

typedef void (*DhcpStateChangeCallback)(DhcpClientCtx *context,
   NetInterface *interface, DhcpState state);


/**
 * @brief DHCP client settings
 **/

typedef struct
{
   NetInterface *interface;                           ///<Network interface to configure
   char_t hostname[DHCP_CLIENT_MAX_HOSTNAME_LEN + 1]; //Host name
   bool_t rapidCommit;                                ///<Quick configuration using rapid commit
   bool_t manualDnsConfig;                            ///<Force manual DNS configuration
   systime_t timeout;                                 ///<DHCP configuration timeout
   DhcpTimeoutCallback timeoutEvent;                  ///<DHCP configuration timeout event
   DhcpLinkChangeCallback linkChangeEvent;            ///<Link state change event
   DhcpStateChangeCallback stateChangeEvent;          ///<FSM state change event
} DhcpClientSettings;


/**
 * @brief DHCP client context
 **/

struct _DhcpClientCtx
{
   DhcpClientSettings settings; ///<DHCP client settings
   bool_t running;              ///<This flag tells whether the DHCP client is running or not
   DhcpState state;             ///<Current state of the FSM
   bool_t timeoutEventDone;     ///<Timeout callback function has been called
   systime_t timestamp;         ///<Timestamp to manage retransmissions
   systime_t timeout;           ///<Timeout value
   systime_t retransmitTimeout; ///<Retransmission timeout
   uint_t retransmitCount;      ///<Retransmission counter
   Ipv4Addr serverIpAddr;       ///<DHCP server IPv4 address
   Ipv4Addr requestedIpAddr;    ///<Requested IPv4 address
   uint32_t transactionId;      ///<Value to match requests with replies
   systime_t configStartTime;   ///<Address acquisition or renewal process start time
   systime_t leaseStartTime;    ///<Lease start time
   uint32_t leaseTime;          ///<Lease time
   uint32_t t1;                 ///<Time at which the client enters the RENEWING state
   uint32_t t2;                 ///<Time at which the client enters the REBINDING state
};


//Tick counter to handle periodic operations
extern systime_t dhcpClientTickCounter;

//DHCP client related functions
void dhcpClientGetDefaultSettings(DhcpClientSettings *settings);
error_t dhcpClientInit(DhcpClientCtx *context, const DhcpClientSettings *settings);
error_t dhcpClientStart(DhcpClientCtx *context);
error_t dhcpClientStop(DhcpClientCtx *context);
DhcpState dhcpClientGetState(DhcpClientCtx *context);

void dhcpClientTick(DhcpClientCtx *context);
void dhcpClientLinkChangeEvent(DhcpClientCtx *context);

void dhcpClientStateInit(DhcpClientCtx *context);
void dhcpClientStateSelecting(DhcpClientCtx *context);
void dhcpClientStateRequesting(DhcpClientCtx *context);
void dhcpClientStateInitReboot(DhcpClientCtx *context);
void dhcpClientStateRebooting(DhcpClientCtx *context);
void dhcpClientStateProbing(DhcpClientCtx *context);
void dhcpClientStateBound(DhcpClientCtx *context);
void dhcpClientStateRenewing(DhcpClientCtx *context);
void dhcpClientStateRebinding(DhcpClientCtx *context);

error_t dhcpClientSendDiscover(DhcpClientCtx *context);
error_t dhcpClientSendRequest(DhcpClientCtx *context);
error_t dhcpClientSendDecline(DhcpClientCtx *context);

void dhcpClientProcessMessage(NetInterface *interface,
   const IpPseudoHeader *pseudoHeader, const UdpHeader *udpHeader,
   const NetBuffer *buffer, size_t offset, void *params);

void dhcpClientParseOffer(DhcpClientCtx *context,
   const DhcpMessage *message, size_t length);

void dhcpClientParseAckNak(DhcpClientCtx *context,
   const DhcpMessage *message, size_t length);

void dhcpClientChangeState(DhcpClientCtx *context,
   DhcpState newState, systime_t delay);

void dhcpClientCheckTimeout(DhcpClientCtx *context);

uint16_t dhcpClientComputeElapsedTime(DhcpClientCtx *context);

void dhcpDumpConfig(DhcpClientCtx *context);

#endif
