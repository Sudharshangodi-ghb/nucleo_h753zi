/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32h753xx.h"

typedef enum
{
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_ALTERNATE = 2,
    GPIO_MODE_ANALOG = 3
} GpioMode;

typedef enum
{
    GPIO_NO_PULL = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} GpioPull;

typedef enum
{
    GPIO_PUSH_PULL = 0,
    GPIO_OPEN_DRAIN = 1
} GpioOutputType;

/* Enables the port clock and configures the selected GPIO mode. */
void Gpio_Init(GPIO_TypeDef *port, uint32_t pin,
               GpioMode mode, GpioOutputType output_type,
               GpioPull pull);

/* Sets an output pin to high or low. */
void Gpio_Write(GPIO_TypeDef *port, uint32_t pin, bool high);

/* Toggles an output pin. */
void Gpio_Toggle(GPIO_TypeDef *port, uint32_t pin);

/* Reads an input or output pin state. */
bool Gpio_Read(GPIO_TypeDef *port, uint32_t pin);

#endif /* GPIO_H */
