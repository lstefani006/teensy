/**
 * @file wilc1000_callbacks.c
 * @brief WILC1000 user callbacks
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
#include "driver/include/m2m_wifi.h"
#include "os/include/m2m_wifi_ex.h"
#include "debug.h"

//Wi-Fi configuration (AP mode)
#define APP_WIFI_AP_INTERFACE ENABLED
#define APP_WIFI_AP_SSID "TEST_WILC1000_AP"
#define APP_WIFI_AP_SECURITY M2M_WIFI_SEC_WEP
#define APP_WIFI_AP_KEY "1122334455"
#define APP_WIFI_AP_CHANNEL M2M_WIFI_CH_6

//Wi-Fi configuration (STA mode)
#define APP_WIFI_STA_INTERFACE DISABLED
#define APP_WIFI_STA_SSID "TEST_WILC1000_STA"
#define APP_WIFI_STA_SECURITY M2M_WIFI_SEC_WPA_PSK
#define APP_WIFI_STA_KEY "12345678"
#define APP_WIFI_STA_CHANNEL M2M_WIFI_CH_ALL


/**
 * @brief Create Wi-Fi network (AP mode)
 **/

void wilc1000EnableAp(void)
{
#if (APP_WIFI_AP_INTERFACE == ENABLED)
   int8_t status;
   tstrM2MAPConfig apConfig;

   //Debug message
   TRACE_INFO("WILC1000: Creating Wi-Fi network (%s)...\r\n", APP_WIFI_AP_SSID);

   //Initialize AP configuration paramters
   memset(&apConfig, 0, sizeof(apConfig));

   //Set SSID
   strcpy((char_t *) apConfig.au8SSID, APP_WIFI_AP_SSID);
   //Make the SSID visible
   apConfig.u8SsidHide = FALSE;
   //Set Wi-Fi channel
   apConfig.u8ListenChannel = APP_WIFI_AP_CHANNEL;

   //Security scheme?
   if(APP_WIFI_AP_SECURITY == M2M_WIFI_SEC_OPEN)
   {
      //Set security mode (open)
      apConfig.u8SecType = M2M_WIFI_SEC_OPEN;
   }
   else if(APP_WIFI_AP_SECURITY == M2M_WIFI_SEC_WEP)
   {
      //Set security mode (WEP)
      apConfig.u8SecType = M2M_WIFI_SEC_WEP;

      //Set WEP key
      apConfig.u8KeyIndx = 0;
      apConfig.u8KeySz = strlen(APP_WIFI_AP_KEY);
      strcpy((char_t *) apConfig.au8WepKey, APP_WIFI_AP_KEY);
   }
   else if(APP_WIFI_AP_SECURITY == M2M_WIFI_SEC_WPA_PSK)
   {
      //Set security mode (WPA/WPA2 personnal)
      apConfig.u8SecType = M2M_WIFI_SEC_WPA_PSK;

      //Set WPA key
      strcpy((char_t *) apConfig.au8PSK, APP_WIFI_AP_KEY);
   }

   //Enable AP
   status = os_m2m_wifi_enable_ap(&apConfig);

   //Debug message
   TRACE_INFO("  os_m2m_wifi_enable_ap = %d\r\n", status);

   //Delay required before sending a new command
   osDelayTask(200);
#endif
}


/**
 * @brief Connect to Wi-Fi network (STA mode)
 **/

void wilc1000Connect(void)
{
#if (APP_WIFI_STA_INTERFACE == ENABLED)
   int8_t status;

   //Debug message
   TRACE_INFO("WILC1000: Connecting to Wi-Fi network (%s)...\r\n", APP_WIFI_STA_SSID);

   //Security scheme?
   if(APP_WIFI_STA_SECURITY == M2M_WIFI_SEC_OPEN)
   {
      //Connect to the specified network (open)
      status = os_m2m_wifi_connect(APP_WIFI_STA_SSID, strlen(APP_WIFI_STA_SSID),
         M2M_WIFI_SEC_OPEN, NULL, APP_WIFI_STA_CHANNEL);
   }
   else if(APP_WIFI_STA_SECURITY == M2M_WIFI_SEC_WEP)
   {
      tstrM2mWifiWepParams wepParams;

      //Set WEP parameters
      wepParams.u8KeyIndx = 1;
      wepParams.u8KeySz = strlen(APP_WIFI_STA_KEY) + 1;
      strcpy((char_t *) wepParams.au8WepKey, APP_WIFI_STA_KEY);

      //Connect to the specified network (WEP)
      status = os_m2m_wifi_connect(APP_WIFI_STA_SSID, strlen(APP_WIFI_STA_SSID),
         M2M_WIFI_SEC_WEP, &wepParams, APP_WIFI_STA_CHANNEL);
   }
   else if(APP_WIFI_STA_SECURITY == M2M_WIFI_SEC_WPA_PSK)
   {
      //Connect to the specified network (WPA/WPA2 personnal)
      status = os_m2m_wifi_connect(APP_WIFI_STA_SSID, strlen(APP_WIFI_STA_SSID),
         M2M_WIFI_SEC_WPA_PSK, APP_WIFI_STA_KEY, APP_WIFI_STA_CHANNEL);
   }

   //Debug message
   TRACE_INFO("  os_m2m_wifi_connect = %d\r\n", status);

   //Delay required before sending a new command
   osDelayTask(200);
#endif
}


/**
 * @brief Callback function that processes Wi-Fi event notifications
 * @param[in] msgType Type of notification
 * @param[in] msg Pointer to the buffer containing the notification parameters
 **/

void wilc1000EventHook(uint8_t msgType, void *msg)
{
   tstrM2mWifiStateChanged *stateChangedMsg;

   //Debug message
   TRACE_INFO("WILC1000: Wi-Fi event hook\r\n");

   //Check message type
   if(msgType == M2M_WIFI_RESP_FIRMWARE_STRTED)
   {
      //Create a Wi-Fi network (AP mode)
      wilc1000EnableAp();
      //Connect to the specified Wi-Fi network (STA mode)
      wilc1000Connect();
   }
   else if(msgType == M2M_WIFI_RESP_CON_STATE_CHANGED)
   {
      //Connection state
      stateChangedMsg = (tstrM2mWifiStateChanged *) msg;

      //STA interface?
      if(stateChangedMsg->u8IfcId == INTERFACE_1)
      {
         //Check link state
         if(stateChangedMsg->u8CurrState == M2M_WIFI_CONNECTED)
         {
         }
         else
         {
            //Reconnect to the specified Wi-Fi network
            wilc1000Connect();
         }
      }
      //AP interface?
      else if(stateChangedMsg->u8IfcId == INTERFACE_2)
      {
      }
   }
}
