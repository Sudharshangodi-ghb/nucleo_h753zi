# GPIO

The GPIO driver provides reusable register-level configuration and pin
operations for STM32H753 GPIO ports A through K.

## Generic GPIO API

```c
void Gpio_Init(GPIO_TypeDef *port, uint32_t pin,
               GpioMode mode, GpioOutputType output_type,
               GpioPull pull);
void Gpio_Write(GPIO_TypeDef *port, uint32_t pin, bool high);
void Gpio_Toggle(GPIO_TypeDef *port, uint32_t pin);
bool Gpio_Read(GPIO_TypeDef *port, uint32_t pin);
```

The generic API supports input, output, alternate-function, and analog modes.
Output type and pull configuration are also reusable for LEDs, buttons,
enable pins, reset pins, and GPIO-controlled peripheral signals.

## Board components

| Component | File | Pin | Usage |
|---|---|---|---|
| LD1 | `led.c` | PB0 | Active-high status LED |

Higher-level components call the generic GPIO API rather than configuring
`MODER`, `OTYPER`, `PUPDR`, or `ODR` directly.

```text
App
  -> Led_Init()/Led_Toggle()
       -> Gpio_Init()/Gpio_Write()/Gpio_Toggle()
            -> STM32 GPIO peripheral registers
```

## Future GPIO components

Components that use direct digital GPIO can be added under this driver:

- Push buttons and switches
- External interrupt inputs
- Buzzers and relays
- Chip-select or peripheral enable pins
- Reset and power-enable pins
- DIP switches and keypad matrices
- GPIO direction-control pins

## Source layout

```text
Drivers/GPIO/
├── gpio.c
├── gpio.h
├── led.c
├── led.h
└── README.md
```
