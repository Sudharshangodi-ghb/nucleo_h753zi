/*
 * Copyright (c) 2026 Sudharshan Reddy Godi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app.h"

/* Firmware entry point: initialize peripherals, then enter the application loop. */
int main(void)
{
    App_Init();

    App_Run();

    /* App_Run() is expected not to return */
    while (1)
    {
    }
}