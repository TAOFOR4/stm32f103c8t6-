#include "stm32f10x.h"                  // Device header for STM32F10x family
#include <stdio.h>                       // For standard I/O functions
#include <stdarg.h>                      // For handling variable argument lists (used in Serial_Printf())

/* Variables to store received data and flag to indicate reception */
uint8_t Serial_RxData;                   // Variable to store received byte
uint8_t Serial_RxFlag;                   // Flag to indicate if data was received

/* Function to initialize the serial communication (USART1) */
void Serial_Init(void)
{
    // Enable clocks for USART1 and GPIOA (for the TX and RX pins)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // Initialize GPIOA Pin 9 (TX) for alternate function push-pull mode
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       // TX pin in alternate function push-pull mode
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;             // TX Pin (PA9)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Initialize GPIOA Pin 10 (RX) for input pull-up mode
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // RX pin in input pull-up mode
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // RX Pin (PA10)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz (though speed is less relevant for input)
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Initialize USART1 configuration
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;                        // Set baud rate to 9600 bps
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // No hardware flow control
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   // Enable both Transmit and Receive mode
    USART_InitStructure.USART_Parity = USART_Parity_No;               // No parity
    USART_InitStructure.USART_StopBits = USART_StopBits_1;            // 1 stop bit
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;       // 8-bit data length
    USART_Init(USART1, &USART_InitStructure);
    
    // Enable USART1 receive interrupt (USART_IT_RXNE: Receive Data Register Not Empty)
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // Set up interrupt priority group (group 2, which separates preemption and sub-priority)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // Initialize NVIC (Nested Vectored Interrupt Controller) for USART1 interrupts
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                 // USART1 interrupt channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                   // Enable the USART1 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;         // Set preemption priority to 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                // Set sub-priority to 1
    NVIC_Init(&NVIC_InitStructure);
    
    // Enable USART1 peripheral
    USART_Cmd(USART1, ENABLE);
}

/* Function to send a single byte over USART */
void Serial_SendByte(uint8_t Byte)
{
    // Send a data byte
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

/* Helper function to calculate X^Y (X raised to the power of Y) */
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

/* Function to send a multi-digit number over USART */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    // Send each digit of the number starting from the most significant digit
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/* Redirects the standard output function fputc() to USART, enabling printf functionality */
int fputc(int ch, FILE *f)
{
    // Send the character via USART
    Serial_SendByte(ch);
    return ch;  // Return the character that was sent
}

/* Function to send formatted strings using variable arguments */
void Serial_Printf(char *format, ...)
{
    char String[100];                     // Buffer to hold the formatted string
    va_list arg;                          // Variable argument list
    va_start(arg, format);                // Initialize argument list
    vsprintf(String, format, arg);        // Format the string using vsprintf()
    va_end(arg);                          // Clean up the argument list
    Serial_SendString(String);            // Send the formatted string over USART
}

/* Function to check if data has been received */
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag == 1)               // If a byte was received
    {
        Serial_RxFlag = 0;                // Clear the flag
        return 1;                         // Return 1 indicating data was received
    }
    return 0;                             // Return 0 indicating no data received
}

/* Function to get the received data byte */
uint8_t Serial_GetRxData(void)
{
    return Serial_RxData;                 // Return the received byte
}

/* Interrupt Service Routine for USART1 (handles incoming data) */
void USART1_IRQHandler(void)
{
    // Check if the interrupt was caused by a received byte
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        Serial_RxData = USART_ReceiveData(USART1);    // Read the received byte
        Serial_RxFlag = 1;                           // Set the flag indicating data reception
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // Clear the interrupt flag
    }
}
