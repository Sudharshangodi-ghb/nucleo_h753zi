/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart3.h"

#include "stm32h753xx.h"

/* USART3: PD8 TX, PD9 RX, alternate function 7. */
#define UART3_BAUDRATE 115200U
#define UART3_CLOCK_HZ 64000000U

void Uart3_Init(void)
{
    /* Enable GPIOD and USART3 peripheral clocks. */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    /* Select PCLK1 as the USART3 kernel clock. */
    RCC->D2CCIP2R &= ~RCC_D2CCIP2R_USART28SEL;
    (void)RCC->AHB4ENR;
    (void)RCC->APB1LENR;

    /* Configure PD8 and PD9 as alternate-function pins. */
    GPIOD->MODER &= ~((3U << 16U) | (3U << 18U));
    GPIOD->MODER |= ((2U << 16U) | (2U << 18U));

    /* Select alternate function AF7 for USART3. */
    GPIOD->AFR[1] &= ~((0xFU << 0U) | (0xFU << 4U));
    GPIOD->AFR[1] |= ((7U << 0U) | (7U << 4U));

    GPIOD->OTYPER &= ~((1U << 8U) | (1U << 9U));
    GPIOD->OSPEEDR &= ~((3U << 16U) | (3U << 18U));
    GPIOD->PUPDR &= ~((3U << 16U) | (3U << 18U));

    /* Configure 115200 baud, 8 data bits, no parity, 1 stop bit. */
    USART3->CR1 = 0U;
    USART3->CR2 = 0U;
    USART3->CR3 = 0U;

    USART3->BRR = (UART3_CLOCK_HZ + (UART3_BAUDRATE / 2U))
                  / UART3_BAUDRATE;

    USART3->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void Uart3_SendChar(char data)
{
    /* Poll TXE until the transmit data register is available. */
    while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
    }

    USART3->TDR = (uint8_t)data;
}

void Uart3_SendString(const char *str)
{
    while (*str != '\0')
    {
        Uart3_SendChar(*str);
        str++;
    }
}
