#include "stm32f10x.h"                  // Device header
#include <stdio.h>                       // For standard I/O functions
#include <stdarg.h>                      // For variable argument list (used in Serial_Printf())

/* Function to initialize USART1 for serial communication */
void Serial_Init(void)
{
    // Enable clocks for USART1 and GPIOA (USART1 uses GPIOA pins)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// Initialize GPIOA pin 9 (TX) for alternate function push-pull (AF_PP) mode
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       // Alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;             // TX Pin (PA9)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Initialize USART1 settings
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;                        // Set baud rate to 9600 bps
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // No flow control
	USART_InitStructure.USART_Mode = USART_Mode_Tx;                   // Enable only Transmit mode
	USART_InitStructure.USART_Parity = USART_Parity_No;               // No parity
	USART_InitStructure.USART_StopBits = USART_StopBits_1;            // 1 stop bit
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;       // 8-bit data length
	USART_Init(USART1, &USART_InitStructure);
	
	// Enable USART1
	USART_Cmd(USART1, ENABLE);
}

/* Function to send a single byte over USART */
void Serial_SendByte(uint8_t Byte)
{
	// Transmit data byte
	USART_SendData(USART1, Byte);
	// Wait until the transmit buffer is empty (TXE flag is set)
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/* Function to send an array of bytes over USART */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	// Loop through each byte in the array and send it
	for (i = 0; i < Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
}

/* Function to send a null-terminated string over USART */
void Serial_SendString(char *String)
{
	uint8_t i;
	// Loop through each character in the string until the null character ('\0') is encountered
	for (i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
	}
}

/* Function to calculate the power of X raised to Y */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	// Multiply Result by X, Y times
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/* Function to send a number with a specific length (digits) over USART */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	// Loop through each digit of the number and send it
	for (i = 0; i < Length; i++)
	{
		// Extract and send each digit from most significant to least significant
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/* Redirect the fputc function to send data over USART for printf-style usage */
int fputc(int ch, FILE *f)
{
	// Send the character via USART
	Serial_SendByte(ch);
	return ch;  // Return the character that was sent
}

/* Function to print formatted strings using variable arguments */
void Serial_Printf(char *format, ...)
{
	char String[100];                     // Buffer to hold the formatted string
	va_list arg;                          // Argument list
	va_start(arg, format);                // Initialize argument list
	vsprintf(String, format, arg);        // Format the string using vsprintf()
	va_end(arg);                          // Clean up argument list
	Serial_SendString(String);            // Send the formatted string over USART
}
