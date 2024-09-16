#include "stm32f10x.h"                  // Device header
#include "Delay.h"                      // For delay functions
#include "OLED.h"                       // OLED display functions
#include "W25Q64.h"                     // W25Q64 Flash memory functions

uint8_t MID;                            // Variable to store Manufacturer ID
uint16_t DID;                           // Variable to store Device ID

// Array of data to write to flash memory
uint8_t ArrayWrite[] = {0x01, 0x02, 0x03, 0x04};
// Array to hold the data read from flash memory
uint8_t ArrayRead[4];

int main(void)
{
	OLED_Init();                        // Initialize the OLED display
	W25Q64_Init();                      // Initialize the W25Q64 flash memory
	
	// Display static labels on the OLED for Manufacturer and Device IDs
	OLED_ShowString(1, 1, "MID:   DID:"); 
	// Display labels for written and read data on the OLED
	OLED_ShowString(2, 1, "W:");
	OLED_ShowString(3, 1, "R:");
	
	// Read the Manufacturer and Device IDs from the flash chip
	W25Q64_ReadID(&MID, &DID);
	// Display the Manufacturer ID (2 hex digits) at row 1, column 5
	OLED_ShowHexNum(1, 5, MID, 2);
	// Display the Device ID (4 hex digits) at row 1, column 12
	OLED_ShowHexNum(1, 12, DID, 4);
	
	// Erase a sector at memory address 0x000000 (4KB sector erase)
	W25Q64_SectorErase(0x000000);
	// Write the data in ArrayWrite[] to flash memory at address 0x000000
	W25Q64_PageProgram(0x000000, ArrayWrite, 4);
	
	// Read the data from flash memory at address 0x000000 into ArrayRead[]
	W25Q64_ReadData(0x000000, ArrayRead, 4);
	
	// Display the written data on the OLED at row 2 (Write data display)
	OLED_ShowHexNum(2, 3, ArrayWrite[0], 2);  // Display first byte
	OLED_ShowHexNum(2, 6, ArrayWrite[1], 2);  // Display second byte
	OLED_ShowHexNum(2, 9, ArrayWrite[2], 2);  // Display third byte
	OLED_ShowHexNum(2, 12, ArrayWrite[3], 2); // Display fourth byte
	
	// Display the read data on the OLED at row 3 (Read data display)
	OLED_ShowHexNum(3, 3, ArrayRead[0], 2);   // Display first byte
	OLED_ShowHexNum(3, 6, ArrayRead[1], 2);   // Display second byte
	OLED_ShowHexNum(3, 9, ArrayRead[2], 2);   // Display third byte
	OLED_ShowHexNum(3, 12, ArrayRead[3], 2);  // Display fourth byte
	
	while (1)
	{
		// Infinite loop to keep the program running and maintain the displayed data
	}
}
