# GPIO and LED

The GPIO driver provides register-level input, output, alternate-function, and
analog configuration for STM32H753 GPIO ports.

```c
void Gpio_Init(GPIO_TypeDef *port, uint32_t pin,
               GpioMode mode, GpioOutputType output_type,
               GpioPull pull);
void Gpio_Write(GPIO_TypeDef *port, uint32_t pin, bool high);
void Gpio_Toggle(GPIO_TypeDef *port, uint32_t pin);
bool Gpio_Read(GPIO_TypeDef *port, uint32_t pin);
```

The board-specific LED driver uses the generic GPIO API:

| LED | Pin | Polarity |
|---|---|---|
| LD1 (green) | PB0 | Active high |

Public LED functions are declared in `led.h`. Keep board wiring in the
component driver and register operations in `gpio.c`.
