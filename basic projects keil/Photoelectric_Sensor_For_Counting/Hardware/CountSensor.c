#include "stm32f10x.h"                  // Device header

uint16_t CountSensor_Count;              // Counter for the sensor

/**
  * @brief  Initializes the sensor input pin and external interrupt.
  * @param  None
  * @retval None
  */
void CountSensor_Init(void)
{
    // Enable clocks for GPIOB and alternate function IO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    // Initialize GPIOB Pin 14 as input with pull-up resistor
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // Input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;            // Pin 14
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // GPIO speed
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Configure the GPIO pin for EXTI (external interrupt)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    
    // Configure EXTI line for pin 14
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;           // EXTI line 14 (PB14)
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;             // Enable EXTI
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   // Set to interrupt mode
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger on falling edge
    EXTI_Init(&EXTI_InitStructure);
    
    // Configure the NVIC (Nested Vector Interrupt Controller) for EXTI
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       // Set priority group

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;  // EXTI lines 15 to 10 IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // Enable the IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // Set preemption priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    // Set subpriority
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Returns the current count value of the sensor.
  * @param  None
  * @retval The count of the sensor.
  */
uint16_t CountSensor_Get(void)
{
    return CountSensor_Count;  // Return the current sensor count
}

/**
  * @brief  EXTI interrupt handler for pins 15 to 10 (used for PB14).
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
    // Check if EXTI Line 14 caused the interrupt
    if (EXTI_GetITStatus(EXTI_Line14) == SET)
    {
        // Debounce: Check if the pin is still low to prevent noise-induced counts
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
        {
            CountSensor_Count++;  // Increment the count on a falling edge
        }
        // Clear the interrupt pending bit for EXTI Line 14
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}
