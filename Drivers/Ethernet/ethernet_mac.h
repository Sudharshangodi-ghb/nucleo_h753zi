/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ETHERNET_MAC_H
#define ETHERNET_MAC_H

#include <stdbool.h>
#include <stdint.h>

#define ETH_PHY_ADDRESS 0U
#define ETH_RX_BUFFER_SIZE 1536U

/* Cached snapshot of LAN8742A link and operating-mode information. */
typedef struct
{
    bool link_up;
    bool full_duplex;
    bool speed_100m;
    uint16_t phy_id1;
    uint16_t phy_id2;
} Ethernet_Status;

/* Initializes the RMII/MAC/MTL/DMA skeleton and probes LAN8742A status. */
bool Ethernet_Init(void);

/* Performs a Clause 22 PHY register read. */
bool Ethernet_ReadPhy(uint16_t phy_reg, uint16_t *value);

/* Performs a Clause 22 PHY register write. */
bool Ethernet_WritePhy(uint16_t phy_reg, uint16_t value);

/* Refreshes and copies PHY status; this does not reconfigure the MAC link mode. */
void Ethernet_GetStatus(Ethernet_Status *status);

/* Reclaims completed RX descriptors and returns a count; payload delivery is deferred. */
uint32_t Ethernet_GetRxPacketCount(void);

#endif /* ETHERNET_MAC_H */
