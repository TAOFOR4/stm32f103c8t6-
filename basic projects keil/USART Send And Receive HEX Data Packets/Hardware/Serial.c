#include "stm32f10x.h"                  // Device header for STM32F10x family
#include <stdio.h>                       // For standard I/O functions
#include <stdarg.h>                      // For handling variable arguments (used in Serial_Printf())

/* Global variables for transmission and reception */
uint8_t Serial_TxPacket[4];              // Transmit packet buffer (contains 4 bytes of data)
uint8_t Serial_RxPacket[4];              // Receive packet buffer (will store 4 bytes of data)
uint8_t Serial_RxFlag;                   // Flag to indicate if a valid packet has been received

/* Function to initialize USART1 for serial communication */
void Serial_Init(void)
{
    /* Enable clocks for USART1 and GPIOA (TX and RX pins are on GPIOA) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* Initialize GPIOA Pin 9 (TX) for alternate function push-pull (AF_PP) */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       // TX pin in alternate function push-pull mode
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;             // TX Pin (PA9)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Initialize GPIOA Pin 10 (RX) for input pull-up (IPU) */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // RX pin in input pull-up mode
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // RX Pin (PA10)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART1 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;                        // Baud rate 9600 bps
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // No hardware flow control
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   // Enable both Transmit and Receive
    USART_InitStructure.USART_Parity = USART_Parity_No;               // No parity
    USART_InitStructure.USART_StopBits = USART_StopBits_1;            // 1 stop bit
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;       // 8-bit word length
    USART_Init(USART1, &USART_InitStructure);
    
    /* Enable the USART1 receive interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    /* Configure NVIC for USART1 interrupt handling */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                 // Set USART1 interrupt channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                   // Enable USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;         // Set preemption priority to 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                // Set sub-priority to 1
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
}

/* Function to send a single byte over USART */
void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1, Byte);                                     // Transmit data byte
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     // Wait until the transmit buffer is empty (TXE flag is set)
}

/* Function to send an array of bytes over USART */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)                                      // Loop through the array
    {
        Serial_SendByte(Array[i]);                                    // Send each byte
    }
}

/* Function to send a string over USART */
void Serial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)                               // Loop through the string until the null terminator
    {
        Serial_SendByte(String[i]);                                   // Send each character
    }
}

/* Helper function to compute X raised to the power of Y */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)                                                       // Multiply Result by X, Y times
    {
        Result *= X;
    }
    return Result;
}

/* Function to send a multi-digit number as a string of digits */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)                                      // Loop through each digit
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0'); // Extract and send each digit
    }
}

/* Redirect the standard output (fputc) to send characters over USART (enables printf functionality) */
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);                                              // Send the character
    return ch;                                                        // Return the character sent
}

/* Function to send a formatted string (similar to printf) */
void Serial_Printf(char *format, ...)
{
    char String[100];                                                 // Buffer to hold the formatted string
    va_list arg;                                                      // Argument list
    va_start(arg, format);                                            // Initialize the argument list
    vsprintf(String, format, arg);                                    // Format the string
    va_end(arg);                                                      // Clean up the argument list
    Serial_SendString(String);                                        // Send the formatted string
}

/* Function to send a custom packet over USART: Packet format [0xFF | 4 bytes of data | 0xFE] */
void Serial_SendPacket(void)
{
    Serial_SendByte(0xFF);                                            // Send start byte (0xFF)
    Serial_SendArray(Serial_TxPacket, 4);                             // Send the 4-byte data packet
    Serial_SendByte(0xFE);                                            // Send stop byte (0xFE)
}

/* Function to check if a valid packet has been received */
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag == 1)                                           // Check if the receive flag is set
    {
        Serial_RxFlag = 0;                                            // Clear the flag
        return 1;                                                     // Return 1 indicating packet received
    }
    return 0;                                                         // Return 0 if no packet received
}

/* Interrupt Service Routine for USART1 (handles received data) */
void USART1_IRQHandler(void)
{
    static uint8_t RxState = 0;                                       // State machine for receiving packets
    static uint8_t pRxPacket = 0;                                     // Packet pointer (index for received data)
    
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)              // Check if data is received
    {
        uint8_t RxData = USART_ReceiveData(USART1);                   // Read the received byte
        
        /* Packet reception state machine */
        if (RxState == 0)                                             // Waiting for start byte (0xFF)
        {
            if (RxData == 0xFF)                                       // If start byte received
            {
                RxState = 1;                                          // Move to next state
                pRxPacket = 0;                                        // Reset packet pointer
            }
        }
        else if (RxState == 1)                                        // Receiving data bytes
        {
            Serial_RxPacket[pRxPacket] = RxData;                      // Store received data byte
            pRxPacket++;                                              // Increment packet pointer
            if (pRxPacket >= 4)                                       // If all 4 bytes received
            {
                RxState = 2;                                          // Move to next state
            }
        }
        else if (RxState == 2)                                        // Waiting for stop byte (0xFE)
        {
            if (RxData == 0xFE)                                       // If stop byte received
            {
                RxState = 0;                                          // Reset state to start byte waiting
                Serial_RxFlag = 1;                                    // Set receive flag indicating a full packet received
            }
        }
        
        /* Clear the USART interrupt pending bit */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
