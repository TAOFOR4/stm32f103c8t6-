#include "stm32f10x.h"                  // Include the STM32F10x header file for device-specific definitions

// Function to initialize PWM on TIM2, Channel 1 (PA0)
void PWM_Init(void)
{
    // Enable the clock for TIM2 (timer 2) on the APB1 peripheral bus
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // Enable the clock for GPIOA on the APB2 peripheral bus
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    // Optional: Enable clock for alternate function I/O and remap pins if needed
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // Initialize GPIOA pin 0 as Alternate Function Push-Pull (PWM output)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Set mode to Alternate Function Push-Pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        // Select Pin 0 (PA0)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // Set the speed to 50 MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);            // Apply the configuration to GPIOA
	
    // Configure TIM2 to use its internal clock
	TIM_InternalClockConfig(TIM2);
	
    // Time Base configuration for TIM2
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;   // No clock division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // Count up mode
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;    // Auto-reload value (ARR), defines the PWM period
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1; // Prescaler value, divides the clock frequency
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; // No repetition counter
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);  // Apply the configuration to TIM2
	
    // PWM Output Compare (OC) configuration for TIM2, Channel 1
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);             // Initialize the OC structure with default values
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   // Set PWM mode 1 (output high when counter < CCR, low otherwise)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // Output high when the counter is less than CCR
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // Enable the output for Channel 1
	TIM_OCInitStructure.TIM_Pulse = 0;                  // Initial duty cycle (compare value, CCR)
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);            // Apply OC configuration to TIM2 Channel 1
	
    // Enable TIM2 to start counting
	TIM_Cmd(TIM2, ENABLE);
}

// Function to set the compare value (duty cycle) for TIM2 Channel 1 (PA0)
void PWM_SetCompare1(uint16_t Compare)
{
    // Set the Compare (CCR1) value for Channel 1 of TIM2
	TIM_SetCompare1(TIM2, Compare);
}
