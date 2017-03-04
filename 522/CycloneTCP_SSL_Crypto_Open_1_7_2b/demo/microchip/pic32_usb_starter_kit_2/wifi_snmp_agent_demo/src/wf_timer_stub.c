/*******************************************************************************
 MRF24WG Universal Driver Timer Stub Functions

  Summary: This module contains Timer functions needed by the Universal Driver.

  Description: This module is specific to:
                * PIC32MX795F512L on the Explorer16
                * MRF24WG PICTail plugged into the PICTail Plus connector towards
                   on the pin 1 edge with module facing the MCU (SPI Slot 1)
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


//==============================================================================
//                                  INCLUDES
//==============================================================================
#include <p32xxxx.h>
#include <sys/attribs.h>
#include "wf_universal_driver.h"
#include "core/net.h"

//==============================================================================
//                                  LOCAL GLOBALS
//==============================================================================

/*****************************************************************************
  Function:
    uint32_t WF_TimerInit(void);

  Summary:
    Configures a timer that increments a 32-bit counter every 1ms.

  Description:
    Called by Universal Driver during initialization to start a 1ms timer that
    will be used for various purposes.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_TimerInit(void)
{
   //Disable timer
   T1CON = 0;

   //Configure prescaler (1:8)
   T1CONSET = _T1CON_TCKPS0_MASK;
   //Set period register
   PR1 = ((40000000 / 8) / 1000) - 1;
   //Clear timer register
   TMR1 = 0;

   //Set interrupt priority
   IPC1CLR = _IPC1_T1IP_MASK;
   IPC1SET = (4 << _IPC1_T1IP_POSITION);
   //Set interrupt subpriority
   IPC1CLR = _IPC1_T1IS_MASK;
   IPC1SET = (0 << _IPC1_T1IS_POSITION);

   //Clear interrupt flag
   IFS0CLR = _IFS0_T1IF_MASK;
   //Enable timer interrupts
   IEC0SET = _IEC0_T1IE_MASK;

   //Start timer
   T1CONSET = _T1CON_TON_MASK;

   //Enable interrupts
   __builtin_enable_interrupts();
}

/*****************************************************************************
  Function:
    uint32_t WF_TimerRead();

  Summary:
    Called by the Universal Driver for various timing operations.

  Description:
    Returns the current value of the 1ms timer.

 Parameters:
    None

  Returns:
    Current value of 1ms timer

  Remarks:
    None
*****************************************************************************/
uint32_t WF_TimerRead(void)
{
   return systemTicks;
}

/*****************************************************************************
  Function:
    Timer1 Interrupt Routine

  Summary:
    Executes every 1ms

  Description:
    This interrupt occurs every 1ms.  The interrupt simply increments the global
    32-bit counter and clears itself.

 Parameters:
    None

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void __ISR(_TIMER_1_VECTOR, IPL4SOFT) timer1IrqHandler(void)
{
   //Clear interrupt flag
   IFS0CLR = _IFS0_T1IF_MASK;
   //Increment tick counter
   systemTicks++;
}
