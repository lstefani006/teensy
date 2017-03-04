/**
 * @file wwd_buffer.c
 * @brief WICED network buffer management
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

//Dependencies
#include "core/net.h"
#include "wwd_buffer.h"
#include "wwd_buffer_interface.h"
#include "debug.h"

#define WWD_BUFFER_COUNT 6

OsMutex wwdBufferMutex;
OsSemaphore wwdBufferSemaphore;
WwdBuffer wwdBuffer[WWD_BUFFER_COUNT];

wwd_result_t wwd_buffer_init(void *native_arg)
{
   osCreateMutex(&wwdBufferMutex);
   osCreateSemaphore(&wwdBufferSemaphore, WWD_BUFFER_COUNT);

   memset(wwdBuffer, 0, sizeof(wwdBuffer));

   return WWD_SUCCESS;
}

wwd_result_t host_buffer_check_leaked(void)
{
   return WWD_SUCCESS;
}

wwd_result_t internal_host_buffer_get(wiced_buffer_t *buffer,
   wwd_buffer_dir_t direction, unsigned short size, unsigned long timeout)
{
   uint_t i;
   bool_t status;
   wwd_result_t ret;

   if(size > (1518 + WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX))
   {
      *buffer = NULL;
      return WWD_BUFFER_UNAVAILABLE_PERMANENT;
   }

   status = osWaitForSemaphore(&wwdBufferSemaphore, timeout);

   if(!status)
   {
      TRACE_ERROR("#### host_buffer_get TX failed\r\n");
      return WWD_BUFFER_UNAVAILABLE_TEMPORARY;
   }

   osAcquireMutex(&wwdBufferMutex);

   for(i = 0; i < WWD_BUFFER_COUNT; i++)
   {
      if(!wwdBuffer[i].used)
      {
         wwdBuffer[i].used = TRUE;
         wwdBuffer[i].size = size;
         wwdBuffer[i].offset = 0;
         break;
      }
   }

   if(i < WWD_BUFFER_COUNT)
   {
      *buffer = &wwdBuffer[i];
      ret = WWD_SUCCESS;
   }
   else
   {
      *buffer = NULL;
      ret = WWD_BUFFER_UNAVAILABLE_TEMPORARY;
   }

   osReleaseMutex(&wwdBufferMutex);
   return ret;
}

wwd_result_t host_buffer_get(wiced_buffer_t *buffer, wwd_buffer_dir_t direction, unsigned short size, wiced_bool_t wait)
{
   systime_t timeout;

   if(wait)
      timeout = INFINITE_DELAY;
   else
      timeout = 0;

   return internal_host_buffer_get(buffer, direction, size, timeout);
}

void host_buffer_release(wiced_buffer_t buffer, wwd_buffer_dir_t direction)
{
   osAcquireMutex(&wwdBufferMutex);

   buffer->used = FALSE;
   buffer->size = 0;
   buffer->offset = 0;

   osReleaseMutex(&wwdBufferMutex);

   osReleaseSemaphore(&wwdBufferSemaphore);
}

uint8_t *host_buffer_get_current_piece_data_pointer(wiced_buffer_t buffer)
{
   return buffer->data + buffer->offset;
}

uint16_t host_buffer_get_current_piece_size( wiced_buffer_t buffer )
{
   return buffer->size;
}

wiced_buffer_t host_buffer_get_next_piece( wiced_buffer_t buffer )
{
   return NULL;
}

wwd_result_t host_buffer_add_remove_at_front(wiced_buffer_t *buffer, int32_t add_remove_amount)
{
   (*buffer)->offset += add_remove_amount;
   (*buffer)->size -= add_remove_amount;

   return WWD_SUCCESS;
}

wwd_result_t host_buffer_set_size(wiced_buffer_t buffer, uint16_t size)
{
   buffer->size = size;
   return WWD_SUCCESS;
}
