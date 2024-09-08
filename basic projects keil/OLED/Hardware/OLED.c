#include "stm32f10x.h"
#include "OLED_Font.h"

/* Pin Configuration */
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))  // SCL pin
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))  // SDA pin

/* Pin Initialization */
void OLED_I2C_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // Enable GPIOB clock
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      // Set to Open-drain output mode for I2C
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;             // Configure SCL pin
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;             // Configure SDA pin
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);  // Set SCL high
	OLED_W_SDA(1);  // Set SDA high
}

/* I2C Start Condition */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);  // Set SDA high
	OLED_W_SCL(1);  // Set SCL high
	OLED_W_SDA(0);  // Pull SDA low
	OLED_W_SCL(0);  // Pull SCL low
}

/* I2C Stop Condition */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);  // Pull SDA low
	OLED_W_SCL(1);  // Set SCL high
	OLED_W_SDA(1);  // Set SDA high
}

/* I2C Send Byte */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));  // Send each bit of the byte
		OLED_W_SCL(1);                   // Set SCL high
		OLED_W_SCL(0);                   // Set SCL low
	}
	OLED_W_SCL(1);	// Extra clock pulse, no ACK response
	OLED_W_SCL(0);
}

/* OLED Write Command */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();                    // Send start condition
	OLED_I2C_SendByte(0x78);             // OLED slave address
	OLED_I2C_SendByte(0x00);             // Write command
	OLED_I2C_SendByte(Command);          // Send command
	OLED_I2C_Stop();                     // Send stop condition
}

/* OLED Write Data */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();                    // Send start condition
	OLED_I2C_SendByte(0x78);             // OLED slave address
	OLED_I2C_SendByte(0x40);             // Write data
	OLED_I2C_SendByte(Data);             // Send data
	OLED_I2C_Stop();                     // Send stop condition
}

/* Set Cursor Position */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);                    // Set Y position
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    // Set higher 4 bits of X position
	OLED_WriteCommand(0x00 | (X & 0x0F));           // Set lower 4 bits of X position
}

/* Clear OLED Screen */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)  // Iterate over Y positions
	{
		OLED_SetCursor(j, 0);  // Set cursor at the beginning of the row
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);  // Write 0x00 (clear) to all columns
		}
	}
}

/* Display a Character */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);   // Set cursor to the top part
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);      // Display top part of the character
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);  // Set cursor to the bottom part
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);  // Display bottom part of the character
	}
}

/* Display a String */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)  // Loop through the string
	{
		OLED_ShowChar(Line, Column + i, String[i]);  // Display each character
	}
}

/* Power Function */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)  // Multiply X by itself Y times
	{
		Result *= X;
	}
	return Result;
}

/* Display Unsigned Decimal Number */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');  // Display each digit
	}
}

/* Display Signed Decimal Number */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');  // Display '+' for positive numbers
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');  // Display '-' for negative numbers
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');  // Display each digit
	}
}

/* Display Hexadecimal Number */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');  // Display 0-9
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');  // Display A-F
		}
	}
}

/* Display Binary Number */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');  // Display each bit
	}
}

/* OLED Initialization */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)  // Power-on delay
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();  // Initialize I2C ports
	
	OLED_WriteCommand(0xAE);  // Turn off display
	
	OLED_WriteCommand(0xD5);  // Set display clock divide ratio/oscillator frequency
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);  // Set multiplex ratio
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);  // Set display offset
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);  // Set display start line
	
	OLED_WriteCommand(0xA1);  // Set segment re-map (left-right direction)

	OLED_WriteCommand(0xC8);  // Set COM output scan direction (up-down direction)

	OLED_WriteCommand(0xDA);  // Set COM pins hardware configuration
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);  // Set contrast control
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);  // Set pre-charge period
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);  // Set VCOMH deselect level
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);  // Enable entire display
	
	OLED_WriteCommand(0xA6);  // Set normal display mode

	OLED_WriteCommand(0x8D);  // Enable charge pump regulator
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);  // Turn on display
		
	OLED_Clear();  // Clear OLED screen
}
