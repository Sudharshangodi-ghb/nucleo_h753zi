# Ethernet driver

The STM32H753 Ethernet MAC/DMA connects to the onboard LAN8742A PHY over RMII.
This driver is a register-level bring-up skeleton; it is not a complete network
interface.

## Implemented and missing

Implemented:

- RMII GPIO and SYSCFG configuration
- Ethernet clock enable and DMA reset
- Four-entry DMA descriptor rings and receive buffers
- MTL and provisional MAC setup
- Clause 22 MDIO access and LAN8742A ID/link-status reads

Not implemented:

- Applying negotiated speed and duplex settings to the MAC
- Transmitting frames or delivering received payloads to the application
- A network interface or LwIP integration

`Ethernet_GetRxPacketCount()` only counts and reclaims completed descriptors;
it does not return packet data.

## Board connections

| MCU pin | RMII signal | Direction |
|---|---|---|
| PA1 | `RMII_REF_CLK` (50 MHz) | PHY to MCU |
| PA2 | `RMII_MDIO` | Bidirectional |
| PA7 | `RMII_CRS_DV` | PHY to MCU |
| PC1 | `RMII_MDC` | MCU to PHY |
| PC4, PC5 | `RMII_RXD0`, `RMII_RXD1` | PHY to MCU |
| PG11 | `RMII_TX_EN` | MCU to PHY |
| PG13, PB13 | `RMII_TXD0`, `RMII_TXD1` | MCU to PHY |

The PHY address is 0. PB13 also has a board-routing conflict; check JP6 and
the relevant solder bridges for the board revision. See the schematics and
user manual in [Docs](../../Docs/README.md) before hardware changes.

## DMA and memory

Descriptors and RX buffers are 32-byte aligned in the `.eth_dma` section,
which the linker places in RAM_D1. Ethernet DMA must not use DTCM. Before
enabling D-cache or adding packet transfers, ensure the DMA buffers are
non-cacheable or perform the required cache maintenance and memory barriers.

Initialization follows this order:

1. Configure RMII pins and select RMII in SYSCFG.
2. Enable Ethernet clocks and reset DMA.
3. Initialize descriptors, DMA, MTL queues, and MAC.
4. Probe the PHY over MDIO.
5. Start DMA and enable MAC receive/transmit.

The MAC currently uses a provisional 100 Mbps/full-duplex configuration; it
does not yet follow negotiated PHY settings.

## Public API

See [ethernet_mac.h](ethernet_mac.h) for declarations:

| Function | Current behavior |
|---|---|
| `Ethernet_Init()` | Initializes RMII/MAC/MTL/DMA and probes the PHY |
| `Ethernet_ReadPhy()` / `Ethernet_WritePhy()` | Read or write a Clause 22 register |
| `Ethernet_GetStatus()` | Reads PHY link, speed, duplex, and ID status |
| `Ethernet_GetRxPacketCount()` | Counts and reclaims completed RX descriptors |

## Adding LwIP later

First add frame-level TX and RX APIs and connect them through an Ethernet
`netif`. For a bare-metal port, start with `NO_SYS=1`, a suitable `lwipopts.h`,
and the raw API. Integrate lwIP's source lists from `src/Filelists.cmake`;
its top-level `CMakeLists.txt` is for standalone packaging, not as a subproject.
See the [lwIP source lists](https://github.com/STMicroelectronics/stm32-mw-lwip/blob/master/src/Filelists.cmake).

## References

- [STM32H7 reference manual RM0433](../../Docs/rm0433-stm32h742-stm32h743-753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [NUCLEO-H753ZI user manual UM2407](../../Docs/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf)
- [STM32H753 datasheet](../../Docs/DS_stm32h753zi.pdf)
- [LAN8742A datasheet](../../Docs/DS_LAN8742_00001989A.pdf)
- [Board schematics](../../Docs/README.md)
