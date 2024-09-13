#include "stm32f10x.h"                  // Device header

uint16_t MyDMA_Size;                     // Global variable to store the size of the data to transfer

// Function to initialize the DMA transfer
// AddrA: Address of the source (peripheral or memory address)
// AddrB: Address of the destination (memory address)
// Size: Number of data elements to transfer
void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	MyDMA_Size = Size;                   // Store the size of data to transfer
	
	// Enable the clock for DMA1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	// Initialize DMA configuration
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;              // Source address (Peripheral or memory address)
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // Size of each peripheral data (Byte)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable; // Enable increment of the peripheral address after each transfer
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;                   // Destination address (Memory address)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // Size of each memory data (Byte)
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         // Enable increment of the memory address after each transfer
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;              // Direction: data flows from Peripheral (AddrA) to Memory (AddrB)
	DMA_InitStructure.DMA_BufferSize = Size;                        // Number of data units to transfer
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                   // Normal mode (not circular, transfer completes once)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;                     // Enable memory-to-memory transfer
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;           // Set DMA priority level to medium
	
	// Initialize DMA1 Channel1 with the specified settings
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	// Disable DMA channel (channel will be enabled when the transfer starts)
	DMA_Cmd(DMA1_Channel1, DISABLE);
}

// Function to start the DMA transfer
void MyDMA_Transfer(void)
{
	// Disable DMA channel before modifying data counter
	DMA_Cmd(DMA1_Channel1, DISABLE);
	
	// Set the number of data units to transfer based on the initialized size
	DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_Size);
	
	// Enable the DMA channel to start the transfer
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	// Wait for the transfer to complete (polling the transfer complete flag)
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	
	// Clear the transfer complete flag to be ready for the next transfer
	DMA_ClearFlag(DMA1_FLAG_TC1);
}
