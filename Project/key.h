#ifndef __KEY_H
#define __KEY_H

#include "stm8s.h"

#define KEY_PORTB GPIOB

#define KEY_1 GPIO_PIN_0
#define KEY_2 GPIO_PIN_1
#define KEY_3 GPIO_PIN_2
#define KEY_4 GPIO_PIN_3
#define KEY_5 GPIO_PIN_4
#define KEY_6 GPIO_PIN_5
#define KEY_7 GPIO_PIN_6
#define KEY_8 GPIO_PIN_7

#define KEY_DOWN  0
#define KEY_UP    1

void Key_Init(void);
uint8_t Key_Scan();
#endif
