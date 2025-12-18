#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"

/* USER KEY 使用 PC13 */
#define KEY_GPIO_PORT GPIOC
#define KEY_GPIO_PIN GPIO_Pin_13
#define KEY_GPIO_CLK RCC_AHB1Periph_GPIOC

void Key_Init(void);
uint8_t Key_IsPressed(void);

#endif
