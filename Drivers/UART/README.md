# USART3

The polling UART driver sends debug output through the NUCLEO-H753ZI
ST-LINK Virtual COM Port.

| Setting | Value |
|---|---|
| Peripheral | USART3 |
| TX / RX | PD8 / PD9, AF7 |
| Format | 115200 baud, 8-N-1 |
| Clock assumption | PCLK1 = 64 MHz |

```c
void Uart3_Init(void);
void Uart3_SendChar(char data);
void Uart3_SendString(const char *str);
```

The baud divisor depends on the USART3 kernel clock. Recalculate it if the
firmware clock setup changes.
