/**
 * @file mrf24wg_callbacks.c
 * @brief MRF24WG user callbacks
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
#include <string.h>
#include "wf_universal_driver.h"

//Wi-Fi configuration
#define APP_WIFI_PROFILE WF_SOFTAP_PROFILE
#define APP_WIFI_SSID "TEST_MRF24WG_AP"
#define APP_WIFI_SECURITY WF_SECURITY_WEP_40
#define APP_WIFI_WEP40_KEY "\x11\x22\x33\x44\x55"
#define APP_WIFI_WEP104_KEY "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd"
#define APP_WIFI_WPA_KEY "12369874"
#define APP_WIFI_CHANNEL 6


/**
 * @brief Connect to Wi-Fi network (STA mode)
 **/

void mrf24wgConnect(void)
{
   t_connectionProfile profile;

   //Infrastructure profile?
   if(APP_WIFI_PROFILE == WF_INFRASTRUCTURE_PROFILE)
   {
      t_infrastructure *infra;
      uint8_t channelList[] = {1, 6, 11, 0};

      //Set profile type
      profile.profileType = WF_INFRASTRUCTURE_PROFILE;

      //Shortcut pointer
      infra = &profile.profile.infrastructure;
      //Set SSID of the network to join
      strcpy(infra->ssid, APP_WIFI_SSID);
      //Set channel list
      memcpy(infra->channelList, channelList, sizeof(channelList));

      //Security scheme?
      if(APP_WIFI_SECURITY == WF_SECURITY_OPEN)
      {
         //Set security mode (open)
         infra->securityType = WF_SECURITY_OPEN;
      }
      else if(APP_WIFI_SECURITY == WF_SECURITY_WEP_40)
      {
         //Set security mode (WEP-40)
         infra->securityType = WF_SECURITY_WEP_40;
         //Set WEP key type
         infra->security.wepContext.keyType = WF_SECURITY_WEP_OPENKEY;

         //Set WEP key
         memcpy(infra->security.wepContext.key, APP_WIFI_WEP40_KEY, 5);
         memcpy(infra->security.wepContext.key + 5, APP_WIFI_WEP40_KEY, 5);
         memcpy(infra->security.wepContext.key + 10, APP_WIFI_WEP40_KEY, 5);
         memcpy(infra->security.wepContext.key + 15, APP_WIFI_WEP40_KEY, 5);
      }
      else if(APP_WIFI_SECURITY == WF_SECURITY_WEP_104)
      {
         //Set security mode (WEP-104)
         infra->securityType = WF_SECURITY_WEP_104;
         //Set WEP key type
         infra->security.wepContext.keyType = WF_SECURITY_WEP_OPENKEY;

         //Set WEP key
         memcpy(infra->security.wepContext.key, APP_WIFI_WEP104_KEY, 13);
         memcpy(infra->security.wepContext.key + 13, APP_WIFI_WEP104_KEY, 13);
         memcpy(infra->security.wepContext.key + 26, APP_WIFI_WEP104_KEY, 13);
         memcpy(infra->security.wepContext.key + 39, APP_WIFI_WEP104_KEY, 13);
      }
      else if(APP_WIFI_SECURITY == WF_SECURITY_WPA_AUTO)
      {
         //Set security mode (WPA/WPA2)
         infra->securityType = WF_SECURITY_WPA_AUTO;

         //Set passphrase
         strcpy(infra->security.wpaContext.passphrase, APP_WIFI_WPA_KEY);
         infra->security.wpaContext.useBinaryKey = false;
      }

      //Configure the reconnect mode in case the connection is lost
      infra->reconnectMode.beaconTimeout = WF_DEFAULT_BEACON_TIMEOUT;
      infra->reconnectMode.beaconTimeoutAction = WF_ATTEMPT_TO_RECONNECT;
      infra->reconnectMode.deauthAction = WF_ATTEMPT_TO_RECONNECT;
      infra->reconnectMode.retryCount = WF_RETRY_FOREVER;
   }
   //Soft-AP profile?
   else if(APP_WIFI_PROFILE == WF_SOFTAP_PROFILE)
   {
      t_softAp *softAp;

      //Set profile type
      profile.profileType = WF_SOFTAP_PROFILE;

      //Shortcut pointer
      softAp = &profile.profile.softAp;
       //Set SSID of the network to join
      strcpy(softAp->ssid, APP_WIFI_SSID);
      //Set channel
      softAp->channel = APP_WIFI_CHANNEL;

      //Security scheme?
      if(APP_WIFI_SECURITY == WF_SECURITY_OPEN)
      {
         //Set security mode (open)
         softAp->securityType = WF_SECURITY_OPEN;
      }
      else if(APP_WIFI_SECURITY == WF_SECURITY_WEP_40)
      {
         //Set security mode (WEP-40)
         softAp->securityType = WF_SECURITY_WEP_40;
         //Set WEP key type
         softAp->wepContext.keyType = WF_SECURITY_WEP_OPENKEY;

         //Set WEP key
         memcpy(softAp->wepContext.key, APP_WIFI_WEP40_KEY, 5);
         memcpy(softAp->wepContext.key + 5, APP_WIFI_WEP40_KEY, 5);
         memcpy(softAp->wepContext.key + 10, APP_WIFI_WEP40_KEY, 5);
         memcpy(softAp->wepContext.key + 15, APP_WIFI_WEP40_KEY, 5);
      }
      else if(APP_WIFI_SECURITY == WF_SECURITY_WEP_104)
      {
         //Set security mode (WEP-104)
         softAp->securityType = WF_SECURITY_WEP_104;
         //Set WEP key type
         softAp->wepContext.keyType = WF_SECURITY_WEP_OPENKEY;

         //Set WEP key
         memcpy(softAp->wepContext.key, APP_WIFI_WEP104_KEY, 13);
         memcpy(softAp->wepContext.key + 13, APP_WIFI_WEP104_KEY, 13);
         memcpy(softAp->wepContext.key + 26, APP_WIFI_WEP104_KEY, 13);
         memcpy(softAp->wepContext.key + 39, APP_WIFI_WEP104_KEY, 13);
      }
   }

   //Initiate connection
   WF_ConnectionProfileSet(&profile);
   WF_Connect();
}


/**
 * @brief Callback function that processes Wi-Fi event notifications
 * @param[in] eventType Type of notification
 * @param[in] eventData Event data
 **/

void mrf24wgEventHook(uint8_t eventType, uint32_t eventData)
{
   //Check event type
   switch(eventType)
   {
   case WF_EVENT_INITIALIZATION:
      //Connect to the specified Wi-Fi network (STA mode)
      mrf24wgConnect();
      break;
   case WF_EVENT_CONNECTION_SUCCESSFUL:
      break;
   case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
   case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
   case WF_EVENT_CONNECTION_FAILED:
      break;
   case WF_EVENT_CONNECTION_REESTABLISHED:
      break;
   case WF_EVENT_DISCONNECT_COMPLETE:
      break;
   case WF_EVENT_SCAN_RESULTS_READY:
      break;
   case WF_EVENT_SOFTAP_NETWORK_STARTED:
      break;
   case WF_EVENT_SOFTAP_CLIENT_CONNECT:
      break;
   case WF_EVENT_SOFTAP_CLIENT_DISCONNECT:
      break;
   case WF_EVENT_ERROR:
      break;
   default:
      break;
   }
}
