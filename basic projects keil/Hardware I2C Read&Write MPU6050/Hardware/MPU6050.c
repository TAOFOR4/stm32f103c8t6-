#include "stm32f10x.h"                  // Device header for STM32F10x family
#include "MPU6050_Reg.h"                // MPU6050 register definitions

#define MPU6050_ADDRESS		0xD0        // MPU6050 I2C address (for write operations)

// Wait for a specific I2C event to complete (used to check for acknowledgment and data transmission)
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;                     // Set timeout value to avoid getting stuck in case of failure
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout--;                       // Decrease timeout counter
		if (Timeout == 0)                // If timeout reaches zero, break the loop to avoid blocking
		{
			break;
		}
	}
}

// Function to write a byte of data to a specified MPU6050 register
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	// Generate I2C Start condition to initiate communication
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	// Send the MPU6050 address with the write bit (0)
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	// Send the register address to write to
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	// Send the data byte to be written to the register
	I2C_SendData(I2C2, Data);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	// Generate I2C Stop condition to end the communication
	I2C_GenerateSTOP(I2C2, ENABLE);
}

// Function to read a byte of data from a specified MPU6050 register
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	// Generate I2C Start condition
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	// Send the MPU6050 address with write bit to specify the register to read from
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	// Send the register address from which to read
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	// Generate a repeated Start condition to switch to read mode
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	// Send the MPU6050 address with the read bit (1)
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	// Disable acknowledgment to read only one byte
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	// Generate Stop condition immediately after the byte is received
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	// Wait for the data byte to be received
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data = I2C_ReceiveData(I2C2);        // Read the received data byte
	
	// Re-enable acknowledgment for future operations
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	return Data;                         // Return the read data byte
}

// Function to initialize the MPU6050 by configuring its registers
void MPU6050_Init(void)
{
	// Enable I2C2 and GPIOB clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// Configure GPIO pins for I2C (PB10 = SCL, PB11 = SDA)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;    // Set mode to open-drain for I2C
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  // Set PB10 (SCL) and PB11 (SDA)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // Set GPIO speed
	GPIO_Init(GPIOB, &GPIO_InitStructure);             // Apply the configuration to GPIOB
	
	// Configure I2C2 settings
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                  // Set mode to I2C
	I2C_InitStructure.I2C_ClockSpeed = 50000;                   // Set clock speed to 50kHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;          // Set duty cycle (standard mode)
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                 // Enable acknowledgment
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  // Use 7-bit addressing
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;                   // Set own address (not relevant for master)
	I2C_Init(I2C2, &I2C_InitStructure);                         // Apply the I2C configuration to I2C2
	
	// Enable the I2C2 peripheral
	I2C_Cmd(I2C2, ENABLE);
	
	// Initialize the MPU6050 registers for proper configuration
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  // Wake up the MPU6050 and set clock source
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);  // Enable accelerometer and gyroscope
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);  // Set sample rate divider
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);      // Set digital low-pass filter (DLPF) configuration
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); // Set gyroscope sensitivity to ±2000 deg/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);// Set accelerometer sensitivity to ±16g
}

// Function to get the MPU6050 device ID by reading the WHO_AM_I register
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);    // Return the device ID from WHO_AM_I register
}

// Function to get accelerometer and gyroscope data from the MPU6050
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
					 int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	// Read accelerometer data for X-axis (combine high and low bytes)
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
	
	// Read accelerometer data for Y-axis
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
	
	// Read accelerometer data for Z-axis
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
	
	// Read gyroscope data for X-axis
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
	
	// Read gyroscope data for Y-axis
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
	
	// Read gyroscope data for Z-axis
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
}
