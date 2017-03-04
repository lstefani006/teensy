/*******************************************************************************
 MRF24WG External Interrupt Stub Functions

  Summary: Functions to control MRF24WG External Interrupt.

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
//                                  DEFINES
//==============================================================================

/*****************************************************************************
  Function:
    void WF_EintInit(void);

  Summary:
    Configure host processor external interrupt.  This line is asserted low by
    MRF24WG.

  Description:
    Called by Universal Driver during initialization.  The interrupt should be
    configured as falling-edge triggered.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintInit(void)
{
   //Disable INT1 interrupt
   IEC0CLR = _IEC0_INT1IE_MASK;

   //Configure INT1 pin (RD8))
   PORTDSET = _PORTD_RD8_MASK;
   TRISDSET = _TRISD_TRISD8_MASK;

   //Configure edge polarity for INT1 interrupt (falling edge)
   INTCONCLR = _INTCON_INT1EP_MASK;

   //Set interrupt priority
   IPC1CLR = _IPC1_INT1IP_MASK;
   IPC1SET = (1 << _IPC1_INT1IP_POSITION);

   //Set interrupt subpriority
   IPC1CLR = _IPC1_INT1IS_MASK;
   IPC1SET = (0 << _IPC1_INT1IS_POSITION);
}

/*****************************************************************************
  Function:
    void WF_EintEnable(void);

  Summary:
    Enables the MRF24WG external interrupt

  Description:
    Called by Universal Driver during normal operations to enable the MRF24WG
    external interrupt.

    Note:  The Universal Driver periodically disables the external interrupt for
           short periods.  Because of this, a falling edge may have been missed.
           So, before enabling the interrupt the code below checks if line is
           low, and if so, forces the interrupt.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintEnable(void)
{
   //Check whether the IRQ line is low
   if(!(PORTD & _PORTD_RD8_MASK))
   {
      //Force the interrupt
      IFS0SET = _IFS0_INT1IF_MASK;
   }

   //Enable INT1 interrupt
   IEC0SET = _IEC0_INT1IE_MASK;
}

/*****************************************************************************
  Function:
    void WF_EintDisable(void);

  Summary:
    Disables the MRF24WG external interrupt

  Description:
    Called by Universal Driver during normal operations to disable the MRF24WG
    external interrupt.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintDisable(void)
{
   //Disable INT1 interrupt
   IEC0CLR = _IEC0_INT1IE_MASK;
}

/*****************************************************************************
  Function:
    bool WF_isEintDisabled(void);

  Summary:
    Determines if the external interrupt is disabled

  Description:
    Called by Universal Driver during normal operations to check if the current
    state of the external interrupt is disabled.

 Parameters:
    None

  Returns:
    True if interrupt is disabled, else False

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
bool WF_isEintDisabled(void)
{
   //Check if the INT1 interrupt is enabled
   if(IEC0 & _IEC0_INT1IE_MASK)
      return false;
   else
      return true;
}

/*****************************************************************************
  Function:
    void _WFInterrupt(void);

  Summary:
    MRF24WG external interrupt handler

  Description:
    This interrupt handler should:
        1) clear the interrupt
        2) ensure the interrupt is disabled upon exit (Universal Driver will reenable it)
        3) call WF_EintHandler()

 Parameters:
    None

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void __ISR(_EXTERNAL_1_VECTOR, IPL1SOFT) ext1IrqHandler(void)
{
   //Clear interrupt flag
   IFS0CLR = _IFS0_INT1IF_MASK;
   //Disable interrupts
   WF_EintDisable();
   //Call interrupt handler
   WF_EintHandler();
}
