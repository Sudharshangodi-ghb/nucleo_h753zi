#include "led.h"

#include "gpio.h"
#include "stm32h753xx.h"

/* LD1: GPIOB pin 0, active-high output. */
#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN 0U

void Led_Init(void)
{
    /* Reuse the generic GPIO output configuration for board LED wiring. */
    Gpio_Init(LED_GPIO_PORT, LED_GPIO_PIN,
              GPIO_MODE_OUTPUT, GPIO_PUSH_PULL, GPIO_NO_PULL);
    Gpio_Write(LED_GPIO_PORT, LED_GPIO_PIN, false);
}

void Led_Toggle(void)
{
    Gpio_Toggle(LED_GPIO_PORT, LED_GPIO_PIN);
}
