/**
 * @file mkv5x_eth.h
 * @brief Freescale Kinetis KV5x Ethernet MAC controller
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

#ifndef _MKV5X_ETH_H
#define _MKV5X_ETH_H

//Number of TX buffers
#ifndef MKV5X_ETH_TX_BUFFER_COUNT
   #define MKV5X_ETH_TX_BUFFER_COUNT 2
#elif (MKV5X_ETH_TX_BUFFER_COUNT < 1)
   #error MKV5X_ETH_TX_BUFFER_COUNT parameter is not valid
#endif

//TX buffer size
#ifndef MKV5X_ETH_TX_BUFFER_SIZE
   #define MKV5X_ETH_TX_BUFFER_SIZE 1536
#elif (MKV5X_ETH_TX_BUFFER_SIZE != 1536)
   #error MKV5X_ETH_TX_BUFFER_SIZE parameter is not valid
#endif

//Number of RX buffers
#ifndef MKV5X_ETH_RX_BUFFER_COUNT
   #define MKV5X_ETH_RX_BUFFER_COUNT 4
#elif (MKV5X_ETH_RX_BUFFER_COUNT < 1)
   #error MKV5X_ETH_RX_BUFFER_COUNT parameter is not valid
#endif

//RX buffer size
#ifndef MKV5X_ETH_RX_BUFFER_SIZE
   #define MKV5X_ETH_RX_BUFFER_SIZE 1536
#elif (MKV5X_ETH_RX_BUFFER_SIZE != 1536)
   #error MKV5X_ETH_RX_BUFFER_SIZE parameter is not valid
#endif

//Interrupt priority grouping
#ifndef MKV5X_ETH_IRQ_PRIORITY_GROUPING
   #define MKV5X_ETH_IRQ_PRIORITY_GROUPING 3
#elif (MKV5X_ETH_IRQ_PRIORITY_GROUPING < 0)
   #error MKV5X_ETH_IRQ_PRIORITY_GROUPING parameter is not valid
#endif

//Ethernet interrupt group priority
#ifndef MKV5X_ETH_IRQ_GROUP_PRIORITY
   #define MKV5X_ETH_IRQ_GROUP_PRIORITY 12
#elif (MKV5X_ETH_IRQ_GROUP_PRIORITY < 0)
   #error MKV5X_ETH_IRQ_GROUP_PRIORITY parameter is not valid
#endif

//Ethernet interrupt subpriority
#ifndef MKV5X_ETH_IRQ_SUB_PRIORITY
   #define MKV5X_ETH_IRQ_SUB_PRIORITY 0
#elif (MKV5X_ETH_IRQ_SUB_PRIORITY < 0)
   #error MKV5X_ETH_IRQ_SUB_PRIORITY parameter is not valid
#endif

//Enhanced transmit buffer descriptor
#define ENET_TBD0_R                0x8000
#define ENET_TBD0_TO1              0x4000
#define ENET_TBD0_W                0x2000
#define ENET_TBD0_TO2              0x1000
#define ENET_TBD0_L                0x0800
#define ENET_TBD0_TC               0x0400
#define ENET_TBD1_DATA_LENGTH      0xFFFF
#define ENET_TBD2_DATA_POINTER_H   0xFFFF
#define ENET_TBD3_DATA_POINTER_L   0xFFFF
#define ENET_TBD4_INT              0x4000
#define ENET_TBD4_TS               0x2000
#define ENET_TBD4_PINS             0x1000
#define ENET_TBD4_IINS             0x0800
#define ENET_TBD5_TXE              0x8000
#define ENET_TBD5_UE               0x2000
#define ENET_TBD5_EE               0x1000
#define ENET_TBD5_FE               0x0800
#define ENET_TBD5_LCE              0x0400
#define ENET_TBD5_OE               0x0200
#define ENET_TBD5_TSE              0x0100
#define ENET_TBD8_BDU              0x8000
#define ENET_TBD10_TIMESTAMP_H     0xFFFF
#define ENET_TBD11_TIMESTAMP_L     0xFFFF

//Enhanced receive buffer descriptor
#define ENET_RBD0_E                0x8000
#define ENET_RBD0_RO1              0x4000
#define ENET_RBD0_W                0x2000
#define ENET_RBD0_RO2              0x1000
#define ENET_RBD0_L                0x0800
#define ENET_RBD0_M                0x0100
#define ENET_RBD0_BC               0x0080
#define ENET_RBD0_MC               0x0040
#define ENET_RBD0_LG               0x0020
#define ENET_RBD0_NO               0x0010
#define ENET_RBD0_CR               0x0004
#define ENET_RBD0_OV               0x0002
#define ENET_RBD0_TR               0x0001
#define ENET_RBD1_DATA_LENGTH      0xFFFF
#define ENET_RBD2_DATA_POINTER_H   0xFFFF
#define ENET_RBD3_DATA_POINTER_L   0xFFFF
#define ENET_RBD4_ME               0x8000
#define ENET_RBD4_PE               0x0400
#define ENET_RBD4_CE               0x0200
#define ENET_RBD4_UC               0x0100
#define ENET_RBD4_INT              0x0080
#define ENET_RBD5_VPCP             0xE000
#define ENET_RBD5_ICE              0x0020
#define ENET_RBD5_PCR              0x0010
#define ENET_RBD5_VLAN             0x0004
#define ENET_RBD5_IPV6             0x0002
#define ENET_RBD5_FRAG             0x0001
#define ENET_RBD6_HEADER_LENGTH    0xF800
#define ENET_RBD6_PROTOCOL_TYPE    0x00FF
#define ENET_RBD7_PAYLOAD_CHECKSUM 0xFFFF
#define ENET_RBD8_BDU              0x8000
#define ENET_RBD10_TIMESTAMP_H     0xFFFF
#define ENET_RBD11_TIMESTAMP_L     0xFFFF

//Kinetis KV5x Ethernet MAC driver
extern const NicDriver mkv5xEthDriver;

//Kinetis KV5x Ethernet MAC related functions
error_t mkv5xEthInit(NetInterface *interface);
void mkv5xEthInitGpio(NetInterface *interface);
void mkv5xEthInitBufferDesc(NetInterface *interface);

void mkv5xEthTick(NetInterface *interface);

void mkv5xEthEnableIrq(NetInterface *interface);
void mkv5xEthDisableIrq(NetInterface *interface);
void mkv5xEthEventHandler(NetInterface *interface);

error_t mkv5xEthSendPacket(NetInterface *interface,
   const NetBuffer *buffer, size_t offset);

error_t mkv5xEthReceivePacket(NetInterface *interface,
   uint8_t *buffer, size_t size, size_t *length);

error_t mkv5xEthSetMulticastFilter(NetInterface *interface);
error_t mkv5xEthUpdateMacConfig(NetInterface *interface);

void mkv5xEthWritePhyReg(uint8_t phyAddr, uint8_t regAddr, uint16_t data);
uint16_t mkv5xEthReadPhyReg(uint8_t phyAddr, uint8_t regAddr);

uint32_t mkv5xEthCalcCrc(const void *data, size_t length);

#endif
