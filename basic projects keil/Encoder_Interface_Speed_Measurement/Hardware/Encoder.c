#include "stm32f10x.h"                  // Device header for STM32F10x series microcontrollers

// Function to initialize the encoder using Timer 3 (TIM3)
void Encoder_Init(void)
{
    // Enable the clock for Timer 3 (TIM3) on APB1 bus
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // Enable the clock for GPIOA on APB2 bus (for encoder input pins PA6 and PA7)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Initialize GPIOA for encoder input
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       // Input mode with pull-up resistor
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // PA6 and PA7 for encoder signals
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // GPIO speed of 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);              // Initialize GPIOA with the specified settings
    
    // Initialize Timer 3 for encoder counting
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // No clock division
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // Up-counting mode
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;   // Set auto-reload register (ARR) to 65535 (16-bit)
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;    // Set prescaler to 1 (no division)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; // Not used in this case (for advanced timers)
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); // Initialize Timer 3 with the specified settings
    
    // Input capture initialization for encoder signals on TIM3 channels 1 and 2 (PA6 and PA7)
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);             // Initialize TIM_ICInitStructure with default values
    
    // Channel 1 input capture configuration (PA6)
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;    // Use Channel 1 for input capture
    TIM_ICInitStructure.TIM_ICFilter = 0xF;             // Apply input filter to suppress noise (0xF = max filter level)
    TIM_ICInit(TIM3, &TIM_ICInitStructure);             // Initialize input capture for Channel 1
    
    // Channel 2 input capture configuration (PA7)
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;    // Use Channel 2 for input capture
    TIM_ICInitStructure.TIM_ICFilter = 0xF;             // Apply input filter to suppress noise (0xF = max filter level)
    TIM_ICInit(TIM3, &TIM_ICInitStructure);             // Initialize input capture for Channel 2
    
    // Configure TIM3 to use encoder interface mode with both channels (TI1 and TI2)
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    // Encoder mode: TIM_EncoderMode_TI12 (both channels used)
    // Polarity: Count pulses on rising edges of both channels

    // Enable Timer 3 to start counting encoder pulses
    TIM_Cmd(TIM3, ENABLE);
}

// Function to get the current encoder count and reset the counter
int16_t Encoder_Get(void)
{
    int16_t Temp;
    
    // Read the current value of the encoder counter (number of pulses counted)
    Temp = TIM_GetCounter(TIM3);
    
    // Reset the encoder counter to 0 after reading to measure relative movement
    TIM_SetCounter(TIM3, 0);
    
    // Return the number of pulses counted since the last reset
    return Temp;
}
