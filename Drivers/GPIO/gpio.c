#include "gpio.h"

static void Gpio_EnablePortClock(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    }
    else if (port == GPIOB)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    }
    else if (port == GPIOC)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    }
    else if (port == GPIOD)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    }
    else if (port == GPIOE)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    }
    else if (port == GPIOF)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;
    }
    else if (port == GPIOG)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;
    }
    else if (port == GPIOH)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;
    }
#ifdef GPIOI
    else if (port == GPIOI)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;
    }
#endif
#ifdef GPIOJ
    else if (port == GPIOJ)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;
    }
#endif
#ifdef GPIOK
    else if (port == GPIOK)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;
    }
#endif

    (void)RCC->AHB4ENR;
}

void Gpio_Init(GPIO_TypeDef *port, uint32_t pin,
               GpioMode mode, GpioOutputType output_type,
               GpioPull pull)
{
    if (port == NULL || pin > 15U)
    {
        return;
    }

    Gpio_EnablePortClock(port);

    uint32_t pin_position = pin * 2U;

    port->MODER &= ~(3U << pin_position);
    port->MODER |= ((uint32_t)mode << pin_position);

    port->OTYPER &= ~(1U << pin);
    port->OTYPER |= ((uint32_t)output_type << pin);

    port->PUPDR &= ~(3U << pin_position);
    port->PUPDR |= ((uint32_t)pull << pin_position);

    port->OSPEEDR &= ~(3U << pin_position);
    port->OSPEEDR |= (3U << pin_position);
}

void Gpio_Write(GPIO_TypeDef *port, uint32_t pin, bool high)
{
    if (port == NULL || pin > 15U)
    {
        return;
    }

    if (high)
    {
        port->BSRR = (1U << pin);
    }
    else
    {
        port->BSRR = (1U << (pin + 16U));
    }
}

void Gpio_Toggle(GPIO_TypeDef *port, uint32_t pin)
{
    if (port != NULL && pin <= 15U)
    {
        port->ODR ^= (1U << pin);
    }
}

bool Gpio_Read(GPIO_TypeDef *port, uint32_t pin)
{
    if (port == NULL || pin > 15U)
    {
        return false;
    }

    return (port->IDR & (1U << pin)) != 0U;
}
