#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(uint32_t ticks_per_sec);
void Delay_ms(uint32_t ms);

#endif
