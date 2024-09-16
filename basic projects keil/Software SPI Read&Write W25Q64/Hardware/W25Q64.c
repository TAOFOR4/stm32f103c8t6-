#include "stm32f10x.h"                  // Device header
#include "MySPI.h"                      // Custom SPI functions header
#include "W25Q64_Ins.h"                 // Instruction set for W25Q64

// Function to initialize the W25Q64 memory chip by setting up SPI
void W25Q64_Init(void)
{
    // Initialize the SPI interface (sets up GPIO pins for SPI communication)
	MySPI_Init();
}

// Function to read the manufacturer and device ID of the W25Q64 chip
// MID: Manufacturer ID (8 bits), DID: Device ID (16 bits)
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
    // Start SPI communication by pulling SS low
	MySPI_Start();
    
    // Send the JEDEC ID command to the chip
	MySPI_SwapByte(W25Q64_JEDEC_ID);
    
    // Read Manufacturer ID and Device ID
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);   // Read Manufacturer ID
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);   // Read the first byte of Device ID
	*DID <<= 8;                                 // Shift the first byte to the higher 8 bits
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);  // Read the second byte of Device ID and combine
	MySPI_Stop();                               // Stop SPI communication by setting SS high
}

// Function to send the Write Enable command to the W25Q64 chip
void W25Q64_WriteEnable(void)
{
	MySPI_Start();                              // Start SPI communication
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);        // Send Write Enable command
	MySPI_Stop();                               // Stop SPI communication
}

// Function to wait until the chip is no longer busy (polling the status register)
// The function checks the busy flag (bit 0 of the status register)
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	MySPI_Start();                              // Start SPI communication
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1); // Send command to read status register
	Timeout = 100000;                           // Set a timeout value
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01) // Check if busy flag (bit 0) is set
	{
		Timeout --;
		if (Timeout == 0)                       // Break if timeout is reached
		{
			break;
		}
	}
	MySPI_Stop();                               // Stop SPI communication
}

// Function to write data to a page in W25Q64 memory (Page Program command)
// Address: 24-bit memory address to write to
// DataArray: pointer to the data to write
// Count: number of bytes to write (max 256 for page size)
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q64_WriteEnable();                       // Enable writing to memory
	
	MySPI_Start();                              // Start SPI communication
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);        // Send Page Program command
	MySPI_SwapByte(Address >> 16);              // Send the higher 8 bits of the address
	MySPI_SwapByte(Address >> 8);               // Send the middle 8 bits of the address
	MySPI_SwapByte(Address);                    // Send the lower 8 bits of the address
	for (i = 0; i < Count; i++)                 // Loop through the data array
	{
		MySPI_SwapByte(DataArray[i]);           // Write each byte of data
	}
	MySPI_Stop();                               // Stop SPI communication
	
	W25Q64_WaitBusy();                          // Wait for the chip to finish the write operation
}

// Function to erase a 4KB sector in W25Q64 memory (Sector Erase command)
// Address: 24-bit memory address within the sector to be erased
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();                       // Enable writing (necessary for erase)

	MySPI_Start();                              // Start SPI communication
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);    // Send Sector Erase command
	MySPI_SwapByte(Address >> 16);              // Send the higher 8 bits of the address
	MySPI_SwapByte(Address >> 8);               // Send the middle 8 bits of the address
	MySPI_SwapByte(Address);                    // Send the lower 8 bits of the address
	MySPI_Stop();                               // Stop SPI communication
	
	W25Q64_WaitBusy();                          // Wait until the sector erase operation is complete
}

// Function to read data from the W25Q64 memory (Read Data command)
// Address: 24-bit memory address to start reading from
// DataArray: pointer to where the read data should be stored
// Count: number of bytes to read
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	MySPI_Start();                              // Start SPI communication
	MySPI_SwapByte(W25Q64_READ_DATA);           // Send Read Data command
	MySPI_SwapByte(Address >> 16);              // Send the higher 8 bits of the address
	MySPI_SwapByte(Address >> 8);               // Send the middle 8 bits of the address
	MySPI_SwapByte(Address);                    // Send the lower 8 bits of the address
	for (i = 0; i < Count; i++)                 // Loop to read the requested number of bytes
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE); // Read each byte and store in DataArray
	}
	MySPI_Stop();                               // Stop SPI communication
}
