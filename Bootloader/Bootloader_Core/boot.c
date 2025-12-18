#include "stm32f4xx.h"
#include "boot.h"

typedef void (*app_entry_t)(void); /*app_entry_t变为函数指针类型*/

void Boot_JumpToApp(void)
{
  uint32_t app_msp;
  uint32_t app_reset;

  /* 1. 读取 APP 的 MSP */
  app_msp = *(uint32_t *)APP_START_ADDR;

  /* 2. 判断 MSP 是否在 SRAM（合法性校验） */
  if ((app_msp < SRAM_START_ADDR) || (app_msp > SRAM_END_ADDR))
  {
    return; // 没有有效 Application
  }

  /* 3. 读取 Reset_Handler */
  app_reset = *(uint32_t *)(APP_START_ADDR + 4);

  /* 4. 关全局中断 */
  __disable_irq();

  /* 5. 关闭 SysTick（Bootloader 常见中断源） */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* 6. 重定向中断向量表 */
  SCB->VTOR = APP_START_ADDR;

  /* 7. 设置主栈指针 */
  __set_MSP(app_msp);

  /* 8. 跳转到 Application */
  ((app_entry_t)app_reset)(); /*将地址转换为函数指针类型并跳转过去*/
}
