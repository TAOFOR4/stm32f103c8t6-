#include "stm32f10x.h"  // Device header for STM32F10x series microcontroller

// Function to initialize the ADC (Analog-to-Digital Converter)
void AD_Init(void)
{
    // Enable clock for ADC1 on APB2 bus
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // Enable clock for GPIOA (ADC input pin PA0 is connected to GPIOA)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Configure the ADC clock, divided by 6 from PCLK2 (72MHz / 6 = 12MHz)
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    // GPIO Initialization for PA0 (Analog Input)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       // Set PA0 as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;           // Using PA0 for ADC input
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // GPIO speed is irrelevant for analog mode
    GPIO_Init(GPIOA, &GPIO_InitStructure);              // Initialize GPIOA with these settings
    
    // Configure ADC Channel 0 (PA0) with a sample time of 55.5 cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    
    // ADC initialization structure configuration
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

// Function to get ADC conversion value
uint16_t AD_GetValue(void)
{
    // Start ADC software conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // Wait until the conversion is complete (ADC_FLAG_EOC: End of Conversion flag)
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    // Return the ADC conversion value (12-bit result)
    return ADC_GetConversionValue(ADC1);
}
