#include "stm32f10x.h"                  // Device header for STM32F10x family
#include "Delay.h"                       // Delay functions (likely used elsewhere, though not here)
#include "OLED.h"                        // OLED display library
#include "Serial.h"                      // Serial communication library

int main(void)
{
    // Initialize the OLED display
    OLED_Init();
    
    // Initialize the USART1 serial communication
    Serial_Init();
    
    // Send a single byte (0x41, which is 'A' in ASCII) over the serial port
    Serial_SendByte(0x41);
    
    // Create an array of bytes and send the array over the serial port
    uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};  // Corresponding ASCII values are 'B', 'C', 'D', 'E'
    Serial_SendArray(MyArray, 4);  // Send 4 bytes (0x42, 0x43, 0x44, 0x45)
    
    // Send a string "Num1=" followed by a newline and carriage return over the serial port
    Serial_SendString("\r\nNum1=");
    
    // Send the number 111 as a 3-digit string over the serial port (prints as "111")
    Serial_SendNumber(111, 3);
    
    // Use printf to send a formatted string with the number 222
    printf("\r\nNum2=%d", 222);  // Prints: Num2=222
    
    // Create a string using sprintf and send it over the serial port
    char String[100];                          // Buffer for the string
    sprintf(String, "\r\nNum3=%d", 333);       // Format the string as "Num3=333"
    Serial_SendString(String);                 // Send the formatted string over USART
    
    // Use Serial_Printf to send a formatted string with the number 444
    Serial_Printf("\r\nNum4=%d", 444);         // Prints: Num4=444
    Serial_Printf("\r\n");                     // Send a new line
    
    // Enter an infinite loop to keep the program running
    while (1)
    {
        // Infinite loop - Add any application logic or periodic tasks here if needed
    }
}
