#include "stm32f10x.h"                  // Include the STM32F10x device header

uint16_t AD_Value[4];                    // Array to store the converted ADC values (4 channels)

void AD_Init(void)
{
	// Enable the clock for ADC1, GPIOA (where the ADC channels are connected), and DMA1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   // Enable ADC1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // Enable GPIOA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);     // Enable DMA1 clock
	
	// Configure the ADC clock to be PCLK2 divided by 6
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                     // Set ADC clock to 12 MHz (72 MHz / 6)

	// Initialize GPIOA pins (PA0 to PA3) as analog input mode for ADC channels 0 to 3
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;          // Set pins as analog input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // Use PA0, PA1, PA2, PA3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // GPIO speed (not relevant for analog mode)
	GPIO_Init(GPIOA, &GPIO_InitStructure);                 // Initialize the GPIOA with the above configuration
	
	// Configure the ADC channels (0 to 3) with sample time of 55.5 cycles
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);  // Channel 0, Rank 1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);  // Channel 1, Rank 2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);  // Channel 2, Rank 3
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);  // Channel 3, Rank 4
		
	// Initialize the ADC1 configuration
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;     // Use independent mode (not multi-ADC)
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // Align the ADC data to the right (lower bits)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // No external trigger (software-controlled)
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;     // Enable continuous conversion mode (ADC keeps converting)
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;           // Enable scan mode to convert multiple channels in sequence
	ADC_InitStructure.ADC_NbrOfChannel = 4;                // Number of channels to scan (4 in this case)
	ADC_Init(ADC1, &ADC_InitStructure);                    // Initialize ADC1 with the above configuration
	
	// Initialize DMA to transfer ADC data to memory
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // Peripheral address: ADC1 data register
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // ADC data size is 16-bit (HalfWord)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // Peripheral address does not increment
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;       // Memory address: AD_Value array
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  // Memory data size is 16-bit (HalfWord)
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          // Enable memory address increment
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // Data direction: from peripheral (ADC) to memory
	DMA_InitStructure.DMA_BufferSize = 4;                            // Buffer size: 4 elements (for 4 channels)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                  // Circular mode: continuously repeat transfer
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                     // Disable memory-to-memory transfer
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;            // Set DMA priority to medium
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                     // Initialize DMA1 Channel1 with the above settings
	
	// Enable the DMA1 Channel1 to start receiving data
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	// Enable ADC1 DMA mode (ADC will use DMA to transfer data)
	ADC_DMACmd(ADC1, ENABLE);
	
	// Enable the ADC1
	ADC_Cmd(ADC1, ENABLE);
	
	// Reset ADC1 calibration (required before using the ADC)
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);  // Wait until reset calibration completes
	
	// Start ADC1 calibration
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);       // Wait until calibration is complete
	
	// Start ADC1 conversion in software mode (since no external trigger is set)
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
