/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UART3_H
#define UART3_H

#include <stdint.h>

/* Initializes USART3 on PD8/PD9 for the 115200-baud ST-LINK VCP. */
void Uart3_Init(void);

/* Sends one character through USART3. */
void Uart3_SendChar(char data);

/* Sends a null-terminated string through USART3. */
void Uart3_SendString(const char *str);

#endif /* UART3_H */
