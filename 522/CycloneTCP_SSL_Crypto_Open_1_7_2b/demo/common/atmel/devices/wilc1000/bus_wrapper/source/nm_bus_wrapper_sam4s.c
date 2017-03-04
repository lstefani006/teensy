/**
 * @file nm_bus_wrapper_sam4s.c
 * @brief SPI bus wrapper for SAM4S microcontrollers
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

#include <stdio.h>
#include "sam4s.h"
#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"

//Maximum transfer size
#define NM_BUS_MAX_TRX_SZ 4096

//Macro definitions
#define SPI_ASSERT_CS()   CONF_WILC_CS_PIO->PIO_CODR = CONF_WILC_CS_PIN
#define SPI_DEASSERT_CS() CONF_WILC_CS_PIO->PIO_SODR = CONF_WILC_CS_PIN


/**
 * @brief Bus capabilities information
 **/

tstrNmBusCapabilities egstrNmBusCapabilities =
{
   NM_BUS_MAX_TRX_SZ
};


/**
 * @brief SPI transfer
 * @param[in] pu8Mosi The data to be written to the slave device
 * @param[out] pu8Miso The data received from the slave device
 * @param[in] u16Sz Number of bytes to be transferred
 * @return Status code (M2M_SUCCESS or M2M_ERR_BUS_FAIL)
 **/

static sint8 spi_rw(uint8 *pu8Mosi, uint8 *pu8Miso, uint16 u16Sz)
{
   //Pull the CS pin low
   SPI_ASSERT_CS();

   //Set source address
   CONF_WILC_SPI->SPI_TPR = (uint32_t) pu8Mosi;
   //Set the number of data to be transferred
   CONF_WILC_SPI->SPI_TCR = u16Sz;

   //Full-duplex transfer?
   if(pu8Miso != NULL)
   {
      //Set destination address
      CONF_WILC_SPI->SPI_RPR = (uint32_t) pu8Miso;
      //Set the number of data to be transferred
      CONF_WILC_SPI->SPI_RCR = u16Sz;

      //Start DMA transfer
      CONF_WILC_SPI->SPI_PTCR = SPI_PTCR_TXTEN | SPI_PTCR_RXTEN;

      //Wait for the DMA transfer to be complete
      while(!(CONF_WILC_SPI->SPI_SR & SPI_SR_TXBUFE));
      while(!(CONF_WILC_SPI->SPI_SR & SPI_SR_RXBUFF));
      while(!(CONF_WILC_SPI->SPI_SR & SPI_SR_TXEMPTY));
   }
   else
   {
      //Start DMA transfer
      CONF_WILC_SPI->SPI_PTCR = SPI_PTCR_TXTEN;

      //Wait for the DMA transfer to be complete
      while(!(CONF_WILC_SPI->SPI_SR & SPI_SR_TXBUFE));
      while(!(CONF_WILC_SPI->SPI_SR & SPI_SR_TXEMPTY));
   }

   //Disable DMA transfer
   CONF_WILC_SPI->SPI_PTCR = SPI_PTCR_TXTDIS | SPI_PTCR_RXTDIS;

   //Terminate the operation by raising the CS pin
   SPI_DEASSERT_CS();

   //Successful operation
   return M2M_SUCCESS;
}


/**
 * @brief SPI bus initialization
 * @param[in] pvinit Unused parameter
 * @return Status code (M2M_SUCCESS or M2M_ERR_BUS_FAIL)
 **/

sint8 nm_bus_init(void *pvinit)
{
   uint32 div;

   //Enable PIO peripheral clock
   PMC->PMC_PCER0 = (1 << CONF_WILC_SPI_ID_PIO);
   //Enable SPI peripheral clock
   PMC->PMC_PCER0 = (1 << CONF_WILC_SPI_ID);

   //Default CS state
   SPI_DEASSERT_CS();

   //Disable interrupts on SPI pins
   CONF_WILC_SPI_PIO->PIO_IDR = CONF_WILC_SCK_PIN | CONF_WILC_MOSI_PIN | CONF_WILC_MISO_PIN;
   //Internal pull-up resistors
   CONF_WILC_SPI_PIO->PIO_PUER = CONF_WILC_SCK_PIN | CONF_WILC_MOSI_PIN | CONF_WILC_MISO_PIN;
   //Assign corresponding pins to Peripheral A function
   CONF_WILC_SPI_PIO->PIO_ABCDSR[0] &= ~(CONF_WILC_SCK_PIN | CONF_WILC_MOSI_PIN | CONF_WILC_MISO_PIN);
   CONF_WILC_SPI_PIO->PIO_ABCDSR[1] &= ~(CONF_WILC_SCK_PIN | CONF_WILC_MOSI_PIN | CONF_WILC_MISO_PIN);
   //Disable the PIO from controlling the corresponding pins
   CONF_WILC_SPI_PIO->PIO_PDR = CONF_WILC_SCK_PIN | CONF_WILC_MOSI_PIN | CONF_WILC_MISO_PIN;;

   //Disable SPI module
   CONF_WILC_SPI->SPI_CR = SPI_CR_SPIDIS;
   //Reset SPI module
   CONF_WILC_SPI->SPI_CR = SPI_CR_SWRST;
   CONF_WILC_SPI->SPI_CR = SPI_CR_SWRST;

   //Master mode operation
   CONF_WILC_SPI->SPI_MR = SPI_MR_MODFDIS | SPI_MR_MSTR;

   //Calculate clock divider
   div = SystemCoreClock / CONF_WILC_SPI_CLOCK;

   //SPI configuration (8-bit words, clock phase = 1, clock polarity = 0)
   CONF_WILC_SPI->SPI_CSR[0] = SPI_CSR_SCBR(div) | SPI_CSR_BITS_8_BIT | SPI_CSR_NCPHA;

   //Enable SPI module
   CONF_WILC_SPI->SPI_CR = SPI_CR_SPIEN;

   //Disable DMA transfer
   CONF_WILC_SPI->SPI_PTCR = SPI_PTCR_TXTDIS | SPI_PTCR_RXTDIS;

   //Reset WILC1000
   nm_bsp_reset();

   //Successful operation
   return M2M_SUCCESS;
}


/**
 * @brief IOCTL command processing
 * @param[in] u8Cmd Command opcode
 * @param[in] pvParameter Command parameters
 * @return Status code (M2M_SUCCESS or M2M_ERR_BUS_FAIL)
 **/

sint8 nm_bus_ioctl(uint8 u8Cmd, void *pvParameter)
{
   sint8 ret;
#ifdef CONF_WILC_USE_SPI
   tstrNmSpiRw *spiRwParams;
#endif

   //Check commande opcode
   switch(u8Cmd)
   {
#ifdef CONF_WILC_USE_SPI
   //Read/write command?
   case NM_BUS_IOCTL_RW:
      //Retrieve command parameters
      spiRwParams = (tstrNmSpiRw *) pvParameter;
      //Perform SPI transfer
      ret = spi_rw(spiRwParams->pu8InBuf, spiRwParams->pu8OutBuf, spiRwParams->u16Sz);
      break;
#endif
   //Invalid command?
   default:
      //Debug message
      M2M_ERR("Invalid IOCTL command!\n");
      //Report an error
      ret = M2M_ERR_BUS_FAIL;
      break;
   }

   //Return status code
   return ret;
}


/**
 * @brief SPI bus deinitialization
 * @param[in] pvinit Unused parameter
 * @return Status code (M2M_SUCCESS or M2M_ERR_BUS_FAIL)
 **/

sint8 nm_bus_deinit(void)
{
   return M2M_SUCCESS;
}
