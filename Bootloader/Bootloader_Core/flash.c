#include "flash.h"

void FLASH_EraseApplication(void)
{
  FLASH_Unlock();

  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                  FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                  FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

  /* 擦除 Sector 1-5 (根据你的 APP 大小调整)
  后续扇区不擦出一方面减少擦除时间，另一方面可用于后续扩展（存储用户配置等） */
  FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);

  FLASH_Lock();
}

void FLASH_WriteData(uint32_t addr, uint8_t *buf, uint32_t len)
{
  FLASH_Unlock();

  for (uint32_t i = 0; i < len; i += 4)
  {
    uint32_t data = 0xFFFFFFFF;

    for (uint32_t j = 0; j < 4 && (i + j) < len; j++)
    {
      data &= ~(0xFF << (j * 8));
      data |= (uint32_t)buf[i + j] << (j * 8);
    }

    FLASH_ProgramWord(addr + i, data);
  }

  FLASH_Lock();
}