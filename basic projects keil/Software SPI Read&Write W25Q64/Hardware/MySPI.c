#include "stm32f10x.h"                  // Device header

// Function to set the state of the Slave Select (SS) pin
// SS pin is GPIO_Pin_4 (PA4) and this function controls its value
void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}

// Function to set the state of the Serial Clock (SCK) pin
// SCK pin is GPIO_Pin_5 (PA5) and this function controls its value
void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)BitValue);
}

// Function to set the state of the Master Output, Slave Input (MOSI) pin
// MOSI pin is GPIO_Pin_7 (PA7) and this function controls its value
void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);
}

// Function to read the state of the Master Input, Slave Output (MISO) pin
// MISO pin is GPIO_Pin_6 (PA6) and this function returns its value
uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

// Initialization function for the SPI interface
void MySPI_Init(void)
{
    // Enable clock for GPIOA peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

    // Initialize SS, SCK, and MOSI as output pins (Push-Pull)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // Set pin speed to 50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Initialize MISO as an input pin with a pull-up resistor
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // Set pin speed to 50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Set SS high and SCK low to start in idle state
	MySPI_W_SS(1);
	MySPI_W_SCK(0); // CPOL
}

// Function to start the SPI communication by pulling SS low
void MySPI_Start(void)
{
	MySPI_W_SS(0);
}

// Function to stop the SPI communication by pulling SS high
void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}

// Function to transmit and receive a byte over SPI
// ByteSend is the byte to be sent, and the function returns the byte received
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;  // Initialize the received byte to 0
	
	// Loop through each bit (MSB first) and transfer it
	for (i = 0; i < 8; i++)
	{
		// Set MOSI according to the bit in ByteSend
		MySPI_W_MOSI(ByteSend & (0x80 >> i));
		
		// Generate clock pulse (set SCK high)
		MySPI_W_SCK(1); //data is sampled (CPHA)
		
		// Read the value of MISO and set the corresponding bit in ByteReceive
		if (MySPI_R_MISO() == 1) {
			ByteReceive |= (0x80 >> i);  // Set the corresponding bit in the received byte
		}
		
		// Set SCK low (end clock pulse)
		MySPI_W_SCK(0);
	}
	
	return ByteReceive;  // Return the byte received from the slave
}
