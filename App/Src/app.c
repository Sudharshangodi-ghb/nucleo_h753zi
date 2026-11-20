/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "app.h"
#include "ethernet_mac.h"
#include "led.h"
#include "uart3.h"

#define LED_DELAY_COUNT     10000000U

/* Software delay used for the periodic LED and status-loop timing. */
static void Delay(void)
{
    for (volatile uint32_t count = 0U;
         count < LED_DELAY_COUNT;
         count++)
    {
        __asm volatile ("nop");
    }
}

void App_Init(void)
{
    /* Application-level initialization calls each peripheral driver. */
    Led_Init();
    Uart3_Init();

    Uart3_SendString("\r\n");
    Uart3_SendString("================================\r\n");
    Uart3_SendString(" STM32H753ZI Bare-Metal App\r\n");
    Uart3_SendString("================================\r\n");

    Uart3_SendString("[INIT] GPIO LED initialization OK\r\n");
    Uart3_SendString("[INIT] USART3 initialization OK\r\n");
    Uart3_SendString("[INIT] LD1 configured on PB0\r\n");

    if (Ethernet_Init())
    {
        Ethernet_Status status;
        Ethernet_GetStatus(&status);

        Uart3_SendString("[INIT] Ethernet MAC/PHY initialized\r\n");
        Uart3_SendString(status.link_up
                         ? "[ETH] Link up\r\n"
                         : "[ETH] Link down\r\n");
    }
    else
    {
        Uart3_SendString("[ERROR] Ethernet initialization failed\r\n");
    }
}

void App_Run(void)
{
    Uart3_SendString("[APP] Application started\r\n");

    while (1)
    {
        Led_Toggle();
        Uart3_SendString("[LED] LD1 toggled\r\n");
        Delay();
    }
}
