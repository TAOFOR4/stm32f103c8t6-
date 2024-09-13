#include "stm32f10x.h"
#include "OLED_Font.h"

/* Pin Configuration */
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

/* Initialize I2C pins */
void OLED_I2C_Init(void)
{
    // Enable clock for GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    // Configure GPIO pins as open-drain outputs
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);  // Set SCL high
	OLED_W_SDA(1);  // Set SDA high
}

/**
  * @brief  Start I2C communication
  * @param  None
  * @retval None
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);  // SDA goes low while SCL is high to start communication
	OLED_W_SCL(0);  // SCL goes low after starting
}

/**
  * @brief  Stop I2C communication
  * @param  None
  * @retval None
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);  // SDA goes low before stopping
	OLED_W_SCL(1);  // SCL goes high to signal stop
	OLED_W_SDA(1);  // SDA goes high to complete the stop condition
}

/**
  * @brief  Send a byte via I2C
  * @param  Byte: The byte to send
  * @retval None
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));  // Send one bit at a time
		OLED_W_SCL(1);  // Clock high
		OLED_W_SCL(0);  // Clock low
	}
	OLED_W_SCL(1);	// Additional clock pulse, ignoring the acknowledgment
	OLED_W_SCL(0);
}

/**
  * @brief  Send command to OLED display
  * @param  Command: The command to send
  * @retval None
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// Send device address (OLED)
	OLED_I2C_SendByte(0x00);		// Command mode
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  Send data to OLED display
  * @param  Data: The data byte to send
  * @retval None
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// Send device address (OLED)
	OLED_I2C_SendByte(0x40);		// Data mode
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  Set the cursor position on OLED display
  * @param  Y: Vertical position (0 to 7)
  * @param  X: Horizontal position (0 to 127)
  * @retval None
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					// Set page address (Y)
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	// Set column high nibble
	OLED_WriteCommand(0x00 | (X & 0x0F));			// Set column low nibble
}

/**
  * @brief  Clear the OLED display
  * @param  None
  * @retval None
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)  // Iterate through pages
	{
		OLED_SetCursor(j, 0);  // Set cursor to the start of each page
		for (i = 0; i < 128; i++)  // Clear all columns in the page
		{
			OLED_WriteData(0x00);  // Write 0x00 to clear the display
		}
	}
}

/**
  * @brief  Display a single character on OLED
  * @param  Line: Line number (1 to 4)
  * @param  Column: Column number (1 to 16)
  * @param  Char: ASCII character to display
  * @retval None
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);  // Set cursor to upper half
	for (i = 0; i < 8; i++)  // Display upper half of the character
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);  // Set cursor to lower half
	for (i = 0; i < 8; i++)  // Display lower half of the character
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
	}
}

/**
  * @brief  Display a string on the OLED
  * @param  Line: Starting line (1 to 4)
  * @param  Column: Starting column (1 to 16)
  * @param  String: String to display
  * @retval None
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)  // Iterate through the string
	{
		OLED_ShowChar(Line, Column + i, String[i]);  // Display each character
	}
}

/**
  * @brief  Power function (X^Y)
  * @retval Returns X raised to the power of Y
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  Display a decimal number (positive) on OLED
  * @param  Line: Starting line (1 to 4)
  * @param  Column: Starting column (1 to 16)
  * @param  Number: The number to display (0 to 4294967295)
  * @param  Length: Number of digits to display (1 to 10)
  * @retval None
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  Display a signed decimal number on OLED
  * @param  Line: Starting line (1 to 4)
  * @param  Column: Starting column (1 to 16)
  * @param  Number: The signed number to display (-2147483648 to 2147483647)
  * @param  Length: Number of digits to display (1 to 10)
  * @retval None
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');  // Display positive sign
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');  // Display negative sign
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  Display a hexadecimal number on OLED
  * @param  Line: Starting line (1 to 4)
  * @param  Column: Starting column (1 to 16)
  * @param  Number: The number to display (0 to 0xFFFFFFFF)
  * @param  Length: Number of hex digits to display (1 to 8)
  * @retval None
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  Display a binary number on OLED
  * @param  Line: Starting line (1 to 4)
  * @param  Column: Starting column (1 to 16)
  * @param  Number: The number to display (0 to 1111 1111 1111 1111)
  * @param  Length: Number of binary digits to display (1 to 16)
  * @retval None
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  Initialize the OLED display
  * @param  None
  * @retval None
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)  // Power-on delay
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();  // Initialize I2C pins
	
	OLED_WriteCommand(0xAE);  // Turn off display
	
	OLED_WriteCommand(0xD5);  // Set display clock divide ratio/oscillator frequency
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);  // Set multiplex ratio
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);  // Set display offset
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);  // Set start line
	
	OLED_WriteCommand(0xA1);  // Set segment re-map (A1 = normal, A0 = reversed)
	
	OLED_WriteCommand(0xC8);  // Set COM output scan direction (C8 = normal, C0 = reversed)

	OLED_WriteCommand(0xDA);  // Set COM pins hardware configuration
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);  // Set contrast control
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);  // Set pre-charge period
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);  // Set VCOMH deselect level
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);  // Enable entire display (A4 = output follows RAM, A5 = entire display on)

	OLED_WriteCommand(0xA6);  // Set normal/inverse display (A6 = normal, A7 = inverse)

	OLED_WriteCommand(0x8D);  // Enable charge pump
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);  // Turn on display
		
	OLED_Clear();  // Clear the OLED display
}
