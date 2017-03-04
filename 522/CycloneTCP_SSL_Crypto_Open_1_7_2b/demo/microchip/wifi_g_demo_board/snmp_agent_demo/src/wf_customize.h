/*******************************************************************************
 MRF24WG Universal Driver Customization.

  Summary: This module contains defines that allow for customization of the
           Universal Driver.

  Description: None
*******************************************************************************/

/* MRF24WG0M Universal Driver
*
* Copyright (c) 2012-2014, Microchip Technology, Inc. <www.microchip.com>
* Contact Microchip for the latest version.
*
* This program is free software; distributed under the terms of BSD
* license:
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __WF_CUSTOMIZE_H
#define __WF_CUSTOMIZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Customize the Universal Driver as needed
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define WF_HOST_BYTE_ORDER LITTLE_ENDIAN // PIC32 is little-endian

#if defined (__CC_ARM)
   #define INLINE //__inline               // make use of toolchain inline functions (or not)
#elif defined (__ICCARM__)
   #define INLINE inline                 // make use of toolchain inline functions (or not)
#elif defined (__GNUC__)
   #define INLINE inline                 // make use of toolchain inline functions (or not)
#endif

#define WF_TX_QUEUE_SIZE  4              // set size of Tx Queue (see documentation)

// enable or disable Wi-Fi features as needed
#define WF_USE_WPS                       // enable or disable Wi-Fi Protected Setup
#define WF_USE_SOFTAP                    // enable or disable Soft AP
#define WF_USE_ADHOC                     // enable or disable Wi-Fi Ad-Hoc mode
#define WF_USE_PSPOLL                    // allow application to use PS-Poll mode

// enable or disable error checking (generate error events)
#define WF_USE_ERROR_CHECKING            // enable or disable Universal Driver error checking

// enable or disable debug output to the serial console (errors, status, etc.)
#define WF_USE_DEBUG_OUTPUT              // enable or disable Universal Driver console debug output

// enable or disable console input command line capability
//#define WF_USE_CONSOLE                   // enable input commands at console
#define WF_CONSOLE_HISTORY_COUNT  4      // number of commands to store in the console history buffer (0 for no history)
#define WF_CONSOLE_USE_COLOR             // enable if colored event output to the console is desired

#define WF_PRINTF(...) fprintf(stderr, __VA_ARGS__)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// End Customization block
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------












//---------------------------------------------
//---------------------------------------------
// Error checking below this point; do no alter
//---------------------------------------------
//---------------------------------------------

#if !defined(WF_HOST_BYTE_ORDER)
    #error "Must define WF_HOST_BYTE_ORDER as LITTLE_ENDIAN or BIG_ENDIAN"
#endif

#if ((WF_HOST_BYTE_ORDER != BIG_ENDIAN) && (WF_HOST_BYTE_ORDER != LITTLE_ENDIAN))
    #error "WF_HOST_BYTE_ORDER must be set to BIG_ENDIAN or LITTLE_ENDIAN"
#endif


#if (WF_TX_QUEUE_SIZE < 2)
    #error "WF_TX_QUEUE_SIZE should be at least 2"
#endif

#ifdef __cplusplus
}
#endif


#endif /* __WF_CUSTOMIZE_H */



