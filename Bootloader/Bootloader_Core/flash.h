#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f4xx.h"

#define APP_START_ADDR 0x08008000

void FLASH_EraseApplication(void);
void FLASH_WriteData(uint32_t addr, uint8_t *buf, uint32_t len);

#endif
