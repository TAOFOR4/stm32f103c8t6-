#include "stm32f10x.h"                  // Device header for STM32F10x family
#include "Delay.h"                       // For delay functions
#include "OLED.h"                        // OLED display library
#include "Serial.h"                      // Serial communication library

/* Variable to store the received byte */
uint8_t RxData;

int main(void)
{
    /* Initialize the OLED display */
    OLED_Init();
    /* Display the initial label "RxData:" on the OLED at position (row=1, column=1) */
    OLED_ShowString(1, 1, "RxData:");
    
    /* Initialize USART1 for serial communication */
    Serial_Init();
    
    /* Main loop */
    while (1)
    {
        /* Check if a new byte has been received */
        if (Serial_GetRxFlag() == 1)
        {
            /* Get the received byte and store it in RxData */
            RxData = Serial_GetRxData();
            /* Send the received byte back via USART (echo the data) */
            Serial_SendByte(RxData);
            /* Display the received byte as a hexadecimal number on the OLED screen at position (row=1, column=8) */
            OLED_ShowHexNum(1, 8, RxData, 2);
        }
    }
}
