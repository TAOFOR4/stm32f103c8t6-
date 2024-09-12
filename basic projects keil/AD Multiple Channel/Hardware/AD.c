#include "stm32f10x.h"  // Device header for STM32F10x series microcontroller

// Function to initialize the ADC and GPIO pins for analog input
void AD_Init(void)
{
    // Enable clock for ADC1 on APB2 bus
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // Enable clock for GPIOA (ADC input pins are connected to GPIOA)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Set ADC clock to PCLK2 divided by 6 (72 MHz / 6 = 12 MHz)
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    // GPIO Initialization for PA0, PA1, PA2, and PA3 (Analog Input pins)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       // Set PA0, PA1, PA2, PA3 as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;  // Analog input on PA0, PA1, PA2, PA3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // GPIO speed is irrelevant for analog mode
    GPIO_Init(GPIOA, &GPIO_InitStructure);              // Initialize GPIOA with these settings
    
    // ADC Initialization structure configuration
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              // ADC works independently
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;          // Align the ADC result to the right (default)
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // No external trigger (software start)
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;             // Disable continuous conversion mode (single conversion)
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                   // Single channel mode (no scanning multiple channels)
    ADC_InitStructure.ADC_NbrOfChannel = 1;                         // Single channel conversion
    ADC_Init(ADC1, &ADC_InitStructure);                             // Initialize ADC1 with the configuration
    
    // Enable ADC1
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC Calibration process
    ADC_ResetCalibration(ADC1);                                     // Reset ADC calibration
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);             // Wait for calibration reset to complete
    ADC_StartCalibration(ADC1);                                     // Start ADC calibration
    while (ADC_GetCalibrationStatus(ADC1) == SET);                  // Wait for calibration to finish
}

// Function to get ADC conversion value from a specified ADC channel
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    // Configure the selected ADC channel with sampling time of 55.5 cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
    
    // Start ADC software conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // Wait until the conversion is complete (ADC_FLAG_EOC: End of Conversion flag)
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    // Return the ADC conversion value (12-bit result)
    return ADC_GetConversionValue(ADC1);
}
