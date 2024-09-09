#include "stm32f10x.h"  // Include the device-specific header for STM32F10x

/**
  * @brief  Initializes Timer 2 with external trigger (from PA0) and interrupt.
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    // Enable clock for Timer 2 (TIM2) on APB1 bus
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Enable clock for GPIOA to use PA0 for external trigger
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Initialize GPIOA Pin 0 as input with pull-up (used for external trigger)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Input with pull-up resistor
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;      // Pin A0 as external trigger
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Configure Timer 2 external trigger mode: use PA0 (external trigger) as clock source
    TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);
    
    // Timer base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // No clock division
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // Timer counts upwards
    TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;  // Timer period: counts up to 9
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;  // No prescaler (timer clock = trigger clock)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  // No repetition counter
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);  // Initialize Timer 2 with these settings
    
    // Clear the update flag to avoid immediate interrupts
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    // Enable the update interrupt for Timer 2 (triggered when counter overflows)
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // Configure NVIC (Nested Vector Interrupt Controller) for Timer 2 interrupts
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // Set priority group

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // Set interrupt channel for Timer 2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // Enable the interrupt channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  // Preemption priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // Sub-priority
    NVIC_Init(&NVIC_InitStructure);
    
    // Start the Timer 2
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Get the current counter value of Timer 2.
  * @param  None
  * @retval uint16_t The current counter value.
  */
uint16_t Timer_GetCounter(void)
{
    return TIM_GetCounter(TIM2);  // Return the current value of the Timer 2 counter
}

/**
  * @brief  Timer 2 interrupt handler (currently commented out).
  *         This function is executed when Timer 2 overflows (reaches the period).
  * @param  None
  * @retval None
  */
/*
void TIM2_IRQHandler(void)
{
    // Check if the update interrupt flag is set
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // Clear the update interrupt flag to allow the timer to continue
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
*/
