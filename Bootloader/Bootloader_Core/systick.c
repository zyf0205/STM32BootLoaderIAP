#include "systick.h"

static volatile uint32_t s_tick_ms = 0;

/**
 * @brief  SysTick 初始化
 * @param  ticks_per_sec  每秒中断次数（通常 1000，即 1ms）
 */
void SysTick_Init(uint32_t ticks_per_sec)
{
  /* SysTick 时钟源选择为 HCLK */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

  /* 计算重装载值 */
  SysTick->LOAD = SystemCoreClock / ticks_per_sec - 1;

  /* 清零当前计数器 */
  SysTick->VAL = 0;

  /* 开启 SysTick 中断 + 计数器 */
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_TICKINT_Msk |
                  SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief  SysTick 中断处理函数
 *         每 1ms 进入一次
 */
void SysTick_Handler(void)
{
  s_tick_ms++;
}

/**
 * @brief  毫秒级延时（阻塞）
 */
void Delay_ms(uint32_t ms)
{
  uint32_t start = s_tick_ms;
  while ((s_tick_ms - start) < ms)
    ;
}
