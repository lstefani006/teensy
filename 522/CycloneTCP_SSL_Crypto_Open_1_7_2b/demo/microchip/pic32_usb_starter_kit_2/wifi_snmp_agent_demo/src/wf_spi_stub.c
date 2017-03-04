/*******************************************************************************
 MRF24WG SPI Stub Functions

  Summary: Functions to control MRF24WG RESET and HIBERNATE pins need by the
           Universal Driver.  Functions in this module should not be called
           directly by the application code.

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
// board-specific includes
#include <p32xxxx.h>
#include "wf_universal_driver.h"
#include "core/net.h"

//==============================================================================
//                                  DEFINES
//==============================================================================

//==============================================================================
//                                  LOCAL VARIABLES
//==============================================================================
bool g_intDisabled = false;

//==============================================================================
//                                  LOCAL FUNCTIONS
//==============================================================================


/*****************************************************************************
  Function:
    void WF_SpiInit(void);

  Summary:
    Initializes SPI controller used to communicate with MRF24WG

  Description:
    Called by Universal Driver during initialization to initialize SPI interface.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiInit(void)
{
   //Configure CS pin (RB2) as an output
   LATBSET = _LATB_LATB2_MASK;
   TRISBCLR = _TRISB_TRISB2_MASK;

   //Select master mode operation
   SPI1CON = _SPI1CON_MSTEN_MASK;
   //Set SPI mode
   SPI1CONSET = _SPI1CON_CKP_MASK | _SPI1CON_SMP_MASK;
   //Set SCK clock frequency
   SPI1BRG = ((40000000 / 2) / 10000000) - 1;
   //Enable SPI1 module
   SPI1CONSET = _SPI1CON_ON_MASK;
}

/*****************************************************************************
  Function:
    void WF_SpiEnableChipSelect(void);

  Summary:
    Selects the MRF24WG SPI by setting the CS line low.

  Description:
    Called by Universal Driver when preparing to transmit data to the MRF24WG.
    Note that MRF24WG interrupt must be disabled before MRF24WG SPI chip select
    is selected.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiEnableChipSelect(void)
{
   //Save current interrupt state
   g_intDisabled = WF_isEintDisabled();
   //Disable interrupts
   WF_EintDisable();

   //Assert CS line
   usleep(1);
   LATBCLR = _LATB_LATB2_MASK;
   usleep(1);
}

/*****************************************************************************
  Function:
    void WF_SpiDisableChipSelect(void);

  Summary:
    Deselects the MRF24WG SPI by setting CS high.

  Description:
    Called by Universal Driver after completing an SPI transaction.  Note that MRF24WG
    interrupt state must be restored after SPI chip select is deselected.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiDisableChipSelect(void)
{
   //Deassert CS line
   usleep(1);
   LATBSET = _LATB_LATB2_MASK;
   usleep(1);

   //Restore interrupt state
   if(!g_intDisabled)
   {
      //Enable interrupts
      WF_EintEnable();
   }
}


/*****************************************************************************
  Function:
    void WF_SpiTxRx(uint8_t *p_txBuf, uint16_t txLength, uint8_t *p_rxBuf, uint16_t rxLength);

  Summary:
    Transmits and receives SPI bytes with the MRF24WG.

  Description:
    Called by Universal Driver to communicate with the MRF24WG.

 Parameters:
    p_txBuf  -- pointer to the transmit buffer
    txLength -- number of bytes to be transmitted from p_txBuf
    p_rxBuf  -- pointer to receive buffer
    rxLength -- number of bytes to read and copy into p_rxBuf

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiTxRx(uint8_t *p_txBuf,
                uint16_t txLength,
                uint8_t *p_rxBuf,
                uint16_t rxLength)
{
   uint16_t i;
   uint16_t byteCount;
   uint8_t data;

   //Total number of bytes to transfer
   if(txLength >= rxLength)
      byteCount = txLength;
   else
      byteCount = rxLength;

   //Transfer data
   for(i = 0; i < byteCount; i++)
   {
      //Get the character to be written
      if(i < txLength)
         data = p_txBuf[i];
      else
         data = 0xFF;

      //Ensure the TX buffer is empty
      while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK));
      //Write character
      SPI1BUF = data;
      //Wait for the operation to complete
      while(!(SPI1STAT & _SPI1STAT_SPIRBF_MASK));

      //Read character
      data = SPI1BUF;

      //Save the received character
      if(i < rxLength)
         p_rxBuf[i] = data;
   }
}
