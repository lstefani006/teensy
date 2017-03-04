/**
 * @file debug.c
 * @brief Debugging facilities
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
#include "bsp_irq_cfg.h"
#include "r7fs7g2x.h"
#include "debug.h"

//Function declaration
void lcdPutChar(char_t c);


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   //Disable protection
   R_SYSTEM->PRCR = 0xA50B;
   //Cancel SCI3 module stop state
   R_MSTP->MSTPCRB_b.MSTPB28 = 0;
   //Enable protection
   R_SYSTEM->PRCR = 0xA500;

   //Disable SCI3 module
   R_SCI3->SCR = 0x00;

   //Unlock PFS registers
   R_PMISC->PWPR_b.BOWI = 0;
   R_PMISC->PWPR_b.PSFWE = 1;

   //Configure TXD3 (P7_7)
   R_PFS->P707PFS_b.PMR = 1;
   R_PFS->P707PFS_b.PSEL = 5;
   R_PFS->P707PFS_b.PCR = 1;

   //Configure RXD3 (P7_6)
   R_PFS->P706PFS_b.PMR = 1;
   R_PFS->P706PFS_b.PSEL = 5;
   R_PFS->P706PFS_b.PCR = 1;

   //Lock PFS registers
   R_PMISC->PWPR_b.PSFWE = 0;
   R_PMISC->PWPR_b.BOWI = 1;

   //Configure UART (8 bits, no parity, 1 stop bit)
   R_SCI3->SMR = 0;
   //Configure prescaler for baud rate generator
   R_SCI3->SEMR_b.BGDM = 1;
   R_SCI3->SEMR_b.ABCS = 1;
   //Select serial communication mode
   R_SCI3->SCMR_b.SMIF = 0;
   //Set baudrate
   R_SCI3->BRR = (SystemCoreClock / 2) / (8 * baudrate) - 1;

   //Enable transmission and reception
   R_SCI3->SCR |= 0x30;
}


/**
 * @brief Display the contents of an array
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @param[in] prepend String to prepend to the left of each line
 * @param[in] data Pointer to the data array
 * @param[in] length Number of bytes to display
 **/

void debugDisplayArray(FILE *stream,
   const char_t *prepend, const void *data, size_t length)
{
   uint_t i;

   for(i = 0; i < length; i++)
   {
      //Beginning of a new line?
      if((i % 16) == 0)
         fprintf(stream, "%s", prepend);
      //Display current data byte
      fprintf(stream, "%02" PRIX8 " ", *((uint8_t *) data + i));
      //End of current line?
      if((i % 16) == 15 || i == (length - 1))
         fprintf(stream, "\r\n");
   }
}


/**
 * @brief Write character to stream
 * @param[in] c The character to be written
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @return On success, the character written is returned. If a writing
 *   error occurs, EOF is returned
 **/

int_t fputc(int_t c, FILE *stream)
{
   //Standard output?
   if(stream == stdout)
   {
      //Display current character
      lcdPutChar(c);

      //On success, the character written is returned
      return c;
   }
   //Standard error output?
   else if(stream == stderr)
   {
      //Wait for the transmitter to be ready
      while(!R_SCI3->SSR_b.TEND);
      //Send character
      R_SCI3->TDR = c;
      //Wait for the transfer to complete
      while(!R_SCI3->SSR_b.TEND);

      //On success, the character written is returned
      return c;
   }
   //Unknown output?
   else
   {
      //If a writing error occurs, EOF is returned
      return EOF;
   }
}
