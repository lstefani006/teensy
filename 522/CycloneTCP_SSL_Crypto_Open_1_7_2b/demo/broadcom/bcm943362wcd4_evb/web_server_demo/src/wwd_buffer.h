/**
 * @file wwd_buffer.h
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

#ifndef _WWD_BUFFER_H
#define _WWD_BUFFER_H

//Dependencies
#include "wwd_constants.h"
#include "wwd_network_constants.h"

typedef struct
{
   uint32_t used;
   wwd_interface_t interface;
   uint32_t size;
   uint32_t offset;
   uint8_t data[1518 + WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX];
} WwdBuffer;

typedef WwdBuffer *wiced_buffer_t;

typedef struct
{
    void* internal_buffer;
    uint16_t buff_size;
} nons_buffer_init_t;

typedef void wiced_buffer_fifo_t;

#endif
