#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{ 
 
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	while(1)
	{
     LED0=0;			  //LED0亮
		 delay_ms(200);
		 LED0=1;				//LED0灭
		 delay_ms(200);
        
        
	 }
}



