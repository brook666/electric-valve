#include "timer.h"
void Timer2_Init( FunctionalState NewState)    
{    
  /* Enable TIM2 clock */   
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,NewState);            
  TIM2_DeInit();  
  
   /* TIM2 TimeBase Configuration */
  
  TIM2_TimeBaseInit(TIM2_Prescaler_128,TIM2_CounterMode_Up,65535);         
  TIM2_ARRPreloadConfig(ENABLE); 
  //TIM2_SetCounter(0);
  TIM2_ITConfig(TIM2_IT_Update, NewState);    
  
  /* TIM2 Channel 2 Input Capture Configuration */
  TIM2_ICInit( TIM2_Channel_1,
                 TIM2_ICPolarity_Rising,
                 TIM2_ICSelection_DirectTI,
                 TIM2_ICPSC_DIV1,0);
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT );
  TIM2_ITConfig(TIM2_IT_CC1,NewState); 
  
  TIM2_Cmd(NewState); 
} 