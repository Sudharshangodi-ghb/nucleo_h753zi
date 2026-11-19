# UART

The UART driver provides register-level polling-mode USART3 output for the
STM32H753ZI Nucleo board.

## USART3 configuration

| Parameter | Configuration |
|---|---|
| Peripheral | USART3 |
| TX pin | PD8 |
| RX pin | PD9 |
| Alternate function | AF7 |
| Baud rate | 115200 |
| Data format | 8-N-1 |
| Kernel clock assumption | PCLK1 at 64 MHz |

PD8 and PD9 connect to the ST-LINK VCP through the board solder bridges.

## Public API

```c
void Uart3_Init(void);
void Uart3_SendChar(char data);
void Uart3_SendString(const char *str);
```

## Source layout

```text
Drivers/UART/
├── uart3.c
├── uart3.h
└── README.md
```
