#include "stm32f10x.h"                  // Device header

/**
  * @brief  Initializes Timer 2 to generate periodic interrupts.
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    // Enable clock for Timer 2 (TIM2)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Configure Timer 2 to use the internal clock
    TIM_InternalClockConfig(TIM2);
    
    // Timer base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // No clock division
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // Timer counts up
    TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;                // Period = 10000 ticks
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;              // Prescaler = 7200 (72 MHz / 7200 = 10 kHz timer clock)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // No repetition counter
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);              // Apply the configuration
    
    // Clear the update flag to avoid immediate interrupts
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    // Enable the update interrupt for Timer 2
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // Configure NVIC (interrupt controller) to handle Timer 2 interrupts
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  // Set priority grouping
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                  // Set interrupt channel for Timer 2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  // Enable the interrupt channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;        // Set preemption priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;               // Set sub-priority
    NVIC_Init(&NVIC_InitStructure);
    
    // Enable Timer 2
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Timer 2 interrupt handler (commented out for now).
  *         This function is executed when Timer 2 reaches the configured period.
  * @param  None
  * @retval None
  */
/*
void TIM2_IRQHandler(void)
{
    // Check if the update interrupt flag is set
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // Clear the interrupt pending bit for the update event
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
*/
