/**
 * @file wilc1000_config.c
 * @brief WILC1000 configuration
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

#ifndef _WILC1000_CONFIG_H
#define _WILC1000_CONFIG_H

//Dependencies
#include <stdint.h>

//WILC1000 chip revision
#define CONF_WILC_USE_1000_REV_B

//SPI interface
#define CONF_WILC_USE_SPI     1

//Debug logs
#define CONF_WILC_DEBUG       1
//Debug output redirection
#define CONF_WILC_PRINTF      TRACE_PRINTF

//RST pin (PA24)
#define CONF_WILC_RST_PIN     PIO_PA24
#define CONF_WILC_RST_PIO     PIOA
#define CONF_WILC_RST_ID_PIO  ID_PIOA

//CE pin (PA6)
#define CONF_WILC_CE_PIN      PIO_PA6
#define CONF_WILC_CE_PIO      PIOA
#define CONF_WILC_CE_ID_PIO   ID_PIOA

//WAKE pin (PA25)
#define CONF_WILC_WAKE_PIN    PIO_PA25
#define CONF_WILC_WAKE_PIO    PIOA
#define CONF_WILC_WAKE_ID_PIO ID_PIOA

//CS pin (PA11)
#define CONF_WILC_CS_PIN      PIO_PA11
#define CONF_WILC_CS_PIO      PIOA
#define CONF_WILC_CS_ID_PIO   ID_PIOA

//IRQ pin (PA1)
#define CONF_WILC_IRQ_PIN     PIO_PA1
#define CONF_WILC_IRQ_PIO     PIOA
#define CONF_WILC_IRQ_ID_PIO  ID_PIOA

//SCK pin (PA14)
#define CONF_WILC_SCK_PIN     PIO_PA14A_SPCK

//MOSI pin (PA13)
#define CONF_WILC_MOSI_PIN    PIO_PA13A_MOSI

//MISO pin (PA12)
#define CONF_WILC_MISO_PIN    PIO_PA12A_MISO

//SPI instance
#define CONF_WILC_SPI         SPI
#define CONF_WILC_SPI_ID      ID_SPI
#define CONF_WILC_SPI_PIO     PIOA
#define CONF_WILC_SPI_ID_PIO  ID_PIOA

//SPI clock
#define CONF_WILC_SPI_CLOCK   48000000

//IRQ number
#define CONF_WILC_IRQn        PIOA_IRQn
//IRQ handler
#define CONF_WILC_IRQHandler  PIOA_Handler

//Interrupt priority grouping
#define CONF_WILC_IRQ_PRIORITY_GROUPING 3
//Interrupt group priority
#define CONF_WILC_IRQ_GROUP_PRIORITY    15
//Interrupt subpriority
#define CONF_WILC_IRQ_SUB_PRIORITY      0

//Forward function declaration
extern void wilc1000EventHook(uint8_t msgType, void *msg);

//Callback function that processes Wi-Fi event notifications
#define CONF_WILC_EVENT_HOOK(msgType, msg) wilc1000EventHook(msgType, msg)

#endif
