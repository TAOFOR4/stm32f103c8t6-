#include "stm32f10x.h"                  // Device header

int16_t Encoder_Count;  // Variable to store the encoder count

/**
  * @brief  Initializes the rotary encoder pins and external interrupts.
  * @param  None
  * @retval None
  */
void Encoder_Init(void)
{
    // Enable clock for GPIOB and AFIO (Alternate Function I/O)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    // Configure GPIOB pins 0 and 1 for encoder channels A and B
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // Input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  // Pins for channels A and B
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // GPIO speed
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Configure EXTI (External Interrupt) lines for pins 0 and 1 (Channels A and B)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  // Channel A
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);  // Channel B
    
    // Configure EXTI for Channel A (Pin 0)
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;  // Configure EXTI for pins 0 and 1
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                // Enable EXTI line
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      // Set to interrupt mode
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // Trigger on falling edge
    EXTI_Init(&EXTI_InitStructure);
    
    // Configure NVIC (Nested Vector Interrupt Controller) for EXTI lines 0 and 1
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // Set priority grouping
    
    // NVIC settings for EXTI0 (Channel A)
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;  // EXTI line 0 (Channel A)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   // Enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // Preemption priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // Sub-priority
    NVIC_Init(&NVIC_InitStructure);

    // NVIC settings for EXTI1 (Channel B)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;  // EXTI line 1 (Channel B)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   // Enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // Preemption priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  // Sub-priority
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Retrieves the current encoder count and resets the count.
  * @param  None
  * @retval The current encoder count.
  */
int16_t Encoder_Get(void)
{
    int16_t Temp;
    Temp = Encoder_Count;  // Copy current count to a temporary variable
    Encoder_Count = 0;     // Reset encoder count
    return Temp;           // Return the count
}

/**
  * @brief  Interrupt handler for EXTI0 (Channel A, Pin 0).
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        // Debounce: Confirm the pin is still low to avoid false triggering
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
        {
            // Check Channel B to determine the direction of rotation
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
            {
                Encoder_Count--;  // Counter-clockwise rotation
            }
        }
        // Clear interrupt flag for EXTI line 0
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/**
  * @brief  Interrupt handler for EXTI1 (Channel B, Pin 1).
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET)
    {
        // Debounce: Confirm the pin is still low to avoid false triggering
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
        {
            // Check Channel A to determine the direction of rotation
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
            {
                Encoder_Count++;  // Clockwise rotation
            }
        }
        // Clear interrupt flag for EXTI line 1
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
