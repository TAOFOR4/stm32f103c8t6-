#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// Write a bit value to the SCL (Clock) line
void MyI2C_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);  // Write the bit value to PB10 (SCL line)
    Delay_us(10);  // Small delay to ensure proper timing
}

// Write a bit value to the SDA (Data) line
void MyI2C_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);  // Write the bit value to PB11 (SDA line)
    Delay_us(10);  // Small delay to ensure proper timing
}

// Read the value from the SDA (Data) line
uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  // Read the value from PB11 (SDA line)
    Delay_us(10);  // Small delay to ensure proper timing
    return BitValue;  // Return the read value (0 or 1)
}

// Initialize GPIO pins for I2C (SCL and SDA)
void MyI2C_Init(void)
{
    // Enable clock for GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // Configure PB10 (SCL) and PB11 (SDA) as open-drain output
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;       // Open-drain mode for I2C communication
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // Select pins PB10 (SCL) and PB11 (SDA)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // Set speed to 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 // Apply configuration
    
    // Set SCL and SDA lines high (idle state)
    GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

// Generate I2C Start condition
void MyI2C_Start(void)
{
    MyI2C_W_SDA(1);  // Set SDA high
    MyI2C_W_SCL(1);  // Set SCL high
    MyI2C_W_SDA(0);  // Pull SDA low while SCL is high (Start condition)
    MyI2C_W_SCL(0);  // Pull SCL low
}

// Generate I2C Stop condition
void MyI2C_Stop(void)
{
    MyI2C_W_SDA(0);  // Pull SDA low
    MyI2C_W_SCL(1);  // Set SCL high
    MyI2C_W_SDA(1);  // Set SDA high while SCL is high (Stop condition)
}

// Send a byte of data via I2C
void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SDA(Byte & (0x80 >> i));  // Send the MSB first by shifting the byte bit by bit
        MyI2C_W_SCL(1);  // Pulse the SCL line to clock the bit out
        MyI2C_W_SCL(0);  // Pull SCL low after each bit
    }
}

// Receive a byte of data via I2C
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    MyI2C_W_SDA(1);  // Release the SDA line (set high) so it can be driven by the slave
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SCL(1);  // Pulse the SCL line to clock in the bit
        if (MyI2C_R_SDA() == 1)
        {
            Byte |= (0x80 >> i);  // Read the bit from SDA and add it to the byte
        }
        MyI2C_W_SCL(0);  // Pull SCL low after each bit
    }
    return Byte;  // Return the received byte
}

// Send an acknowledgment (Ack or Nack) after receiving a byte
void MyI2C_SendAck(uint8_t AckBit)
{
    MyI2C_W_SDA(AckBit);  // Send ACK (0) or NACK (1)
    MyI2C_W_SCL(1);  // Pulse the SCL line to send the Ack
    MyI2C_W_SCL(0);  // Pull SCL low after sending
}

// Receive an acknowledgment (Ack or Nack) after sending a byte
uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t AckBit;
    MyI2C_W_SDA(1);  // Release the SDA line to allow slave to send Ack
    MyI2C_W_SCL(1);  // Pulse the SCL line
    AckBit = MyI2C_R_SDA();  // Read the Ack bit from SDA (0 = ACK, 1 = NACK)
    MyI2C_W_SCL(0);  // Pull SCL low
    return AckBit;  // Return the Ack status
}
