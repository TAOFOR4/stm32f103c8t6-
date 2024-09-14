#include "stm32f10x.h"                  // Device header for STM32F10x family
#include "Delay.h"                       // Delay library (likely for creating small delays)
#include "OLED.h"                        // OLED display library
#include "Serial.h"                      // Serial communication library
#include "Key.h"                         // Key input (button) library

/* Variable to store the key (button) number */
uint8_t KeyNum;

int main(void)
{
    /* Initialize the OLED display */
    OLED_Init();
    /* Initialize the button input */
    Key_Init();
    /* Initialize the serial communication (USART1) */
    Serial_Init();
    
    /* Display static labels on the OLED for TX and RX packets */
    OLED_ShowString(1, 1, "TxPacket");   // Display "TxPacket" on row 1, column 1
    OLED_ShowString(3, 1, "RxPacket");   // Display "RxPacket" on row 3, column 1
    
    /* Initialize the serial transmission packet with default values */
    Serial_TxPacket[0] = 0x01;           // First byte in TX packet
    Serial_TxPacket[1] = 0x02;           // Second byte in TX packet
    Serial_TxPacket[2] = 0x03;           // Third byte in TX packet
    Serial_TxPacket[3] = 0x04;           // Fourth byte in TX packet
    
    /* Main loop */
    while (1)
    {
        /* Check if the button is pressed and get the key number */
        KeyNum = Key_GetNum();
        
        /* If the first button is pressed (KeyNum == 1) */
        if (KeyNum == 1)
        {
            /* Increment all bytes in the TX packet */
            Serial_TxPacket[0] ++;
            Serial_TxPacket[1] ++;
            Serial_TxPacket[2] ++;
            Serial_TxPacket[3] ++;
            
            /* Send the TX packet over serial (USART1) */
            Serial_SendPacket();
            
            /* Display the updated TX packet values on the OLED (hex format) */
            OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);    // Display byte 1 in TX packet
            OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);    // Display byte 2 in TX packet
            OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);    // Display byte 3 in TX packet
            OLED_ShowHexNum(2, 10, Serial_TxPacket[3], 2);   // Display byte 4 in TX packet
        }
        
        /* Check if a valid RX packet has been received */
        if (Serial_GetRxFlag() == 1)
        {
            /* Display the received RX packet values on the OLED (hex format) */
            OLED_ShowHexNum(4, 1, Serial_RxPacket[0], 2);    // Display byte 1 in RX packet
            OLED_ShowHexNum(4, 4, Serial_RxPacket[1], 2);    // Display byte 2 in RX packet
            OLED_ShowHexNum(4, 7, Serial_RxPacket[2], 2);    // Display byte 3 in RX packet
            OLED_ShowHexNum(4, 10, Serial_RxPacket[3], 2);   // Display byte 4 in RX packet
        }
    }
}
