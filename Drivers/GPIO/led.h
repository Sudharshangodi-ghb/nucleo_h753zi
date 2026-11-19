#ifndef LED_H
#define LED_H

/* Initializes LD1 on PB0 as a push-pull output. */
void Led_Init(void);

/* Toggles the LD1 output state. */
void Led_Toggle(void);

#endif /* LED_H */
