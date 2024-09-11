#include "stm32f10x.h"                  // Device header for STM32F10x series

// Function to initialize and configure PWM using TIM2 and GPIOA Pin 1
void PWM_Init(void)
{
    // Enable clock for TIM2 (Timer 2) on APB1 bus
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // Enable clock for GPIOA on APB2 bus
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Configure PA1 as Alternate Function Push-Pull (AF_PP) for PWM output
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // Set mode as Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // Use Pin 1 (PA1) for PWM output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // Set output speed to 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);             // Initialize GPIOA with the specified settings
    
    // Configure TIM2 to use the internal clock source
    TIM_InternalClockConfig(TIM2);
    
    // Configure TIM2 time base settings (frequency, counter mode, etc.)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // No clock division
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // Up counting mode
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;  // Set auto-reload register (ARR) to 19999 for 50 Hz PWM
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // Set prescaler (PSC) to 71, resulting in a 1 MHz timer clock
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  // No repetition counter used
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);   // Initialize TIM2 with the specified settings
    
    // Configure Output Compare (OC) settings for PWM on Channel 2 (PA1)
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);               // Initialize OC structure with default values
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // Set PWM Mode 1 (output high until CCR, then low)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // Set polarity to high (active high output)
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  // Enable output on Channel 2
    TIM_OCInitStructure.TIM_Pulse = 0;                    // Initial duty cycle set to 0 (output low)
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);              // Initialize TIM2 Channel 2 with the specified settings
    
    // Start the TIM2 counter to begin generating PWM signals
    TIM_Cmd(TIM2, ENABLE);
}

// Function to set the PWM duty cycle for TIM2 Channel 2
void PWM_SetCompare2(uint16_t Compare)
{
    // Set the Capture/Compare Register (CCR2) for Channel 2 to define duty cycle
    TIM_SetCompare2(TIM2, Compare);
}
