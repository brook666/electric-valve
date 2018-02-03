#include "stm8l15x.h"
#include "sysclock.h"
#include "uart1.h"
#include "dma.h"
#include "timer.h"
 uint8_t uart1_dma_flag;
void LED_Init(void)
{
	GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
}

void delay_ms(u16 num)//不是很精确
{
  	u8 i = 0;
	while(num--)
	{
		for (i=0; i<120; i++);
	}
}

void main(void)
{	
  	enableInterrupts();
  	SystemClock_Init();
       // LED_Init();
	Uart1_Init();
        usart_dma_init() ;
	//Timer2_Init(ENABLE);
	while(1)
	{
	 //GPIO_ToggleBits(GPIOD, GPIO_Pin_4);
         //UART1_SendStr("UART1 REMAP TEST");
         Iterates_usart1_buffer();
	 //delay_ms(1000);                  
	}
}

