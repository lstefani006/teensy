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
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "soc_omapl138.h"
#include "psc.h"
#include "uart.h"
#include "lcdkomapl138.h"
#include "debug.h"

#ifdef __TI_ARM__
   #include <stdio.h>
   #include <file.h>
#endif

//Forward declaration of functions
void debugRedirect(void);


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   //Enable UART2 module
   PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART2, 0, PSC_MDCTL_NEXT_ENABLE);

   //Configure UART2 pin multiplexing
   UARTPinMuxSetup(2, FALSE);

   //Enable UART2
   UARTEnable(SOC_UART_2_REGS);

   //UART2 configuration
   UARTConfigSetExpClk(SOC_UART_2_REGS, SOC_UART_2_MODULE_FREQ,
      baudrate, UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);

   //Enable FIFO mode for transmitter and receiver
   UARTFIFOEnable(SOC_UART_2_REGS);

   //Set the receiver FIFO Trigger level
   UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1);

#ifdef __TI_ARM__
   //Redirect output stream
   debugRedirect();
#endif
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
   //Standard output or error output?
   if(stream == stdout || stream == stderr)
   {
      //Send character
      UARTCharPut(SOC_UART_2_REGS, c);

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


#ifdef __TI_ARM__

/**
 * @brief Open file for I/O
 * @param[in] path Filename of the file to be opened
 * @param[in] flags Attributes that specify how the file is manipulated
 * @param[in] fd File descriptor
 * @return Non-negative file descriptor
 */

int_t debugStreamOpen(const char_t *path, uint_t flags, int_t fd)
{
   //Not implemented
   return 0;
}


/**
 * @brief Close file for I/O
 * @param[in] fd File descriptor
 * @return This function should return -1 to indicate an error
 */

int_t debugStreamClose(int_t fd)
{
   //Not implemented
   return 0;
}


/**
 * @brief Read characters from a file
 * @param[in] fd File descriptor
 * @param[out] buffer Buffer where the read characters are placed
 * @param[in] count Number of characters to read from the file
 * @return The number of bytes read
 **/

int_t debugStreamRead(int fd, char_t *buffer, uint_t count)
{
   //Not implemented
   return 0;
}


/**
 * @brief Write characters from a file
 * @param[in] fd File descriptor
 * @param[in] buffer Buffer where the write characters are placed
 * @param[in] count Number of characters to write to the file
 * @return The number of bytes written
 **/

int_t debugStreamWrite(int_t fd, const char_t *buffer, uint_t count)
{
   uint_t i;

   //Send all the characters
   for(i = 0; i < count; i++)
      fputc(buffer[i], stdout);

   //Return the number of characters written
   return count;
}


/**
 * @brief Set the file position indicator
 * @param[in] fd File descriptor
 * @param[in] offset Offset from the origin
 * @param[in] origin Position used as reference for the offset
 * @return The new value of the file position indicator
 **/

off_t debugStreamSeek(int_t fd, off_t offset, int_t origin)
{
   //Not implemented
   return 0;
}


/**
 * @brief Delete file
 * @param[in] path Filename of the file to be deleted
 * @return If successful, this function returns 0
 **/

int_t debugStreamUnlink(const char_t * path)
{
   //Not implemented
   return 0;
}


/**
 * @brief Rename file
 * @param[in] old Current name of the file
 * @param[in] new New name for the file
 * @return If successful, this function returns 0
 **/

int_t debugStreamRename(const char_t *old, const char_t *new)
{
   //Not implemented
   return 0;
}


/**
 * @brief Redirect output stream
 **/

void debugRedirect(void)
{
   //Add a new device
   add_device("debug", _MSA, debugStreamOpen, debugStreamClose, debugStreamRead,
      debugStreamWrite, debugStreamSeek, debugStreamUnlink, debugStreamRename);

   //Redirect stderr to the debug UART
   freopen("debug:", "w", stderr);

   //Disable buffering
   setvbuf(stderr, NULL, _IONBF, 0);
}

#endif
