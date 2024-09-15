#include "stm32f10x.h"                  // Device header for STM32F10x family
#include "MyI2C.h"                      // Custom I2C functions header
#include "MPU6050_Reg.h"                // MPU6050 register definitions

#define MPU6050_ADDRESS		0xD0        // MPU6050 I2C device address (write mode)

// Function to write a byte of data to a specified MPU6050 register
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    MyI2C_Start();                      // Send I2C Start condition
    MyI2C_SendByte(MPU6050_ADDRESS);    // Send MPU6050 address with write bit (0)
    MyI2C_ReceiveAck();                 // Receive ACK from MPU6050
    MyI2C_SendByte(RegAddress);         // Send the register address to write to
    MyI2C_ReceiveAck();                 // Receive ACK
    MyI2C_SendByte(Data);               // Send the data byte
    MyI2C_ReceiveAck();                 // Receive ACK
    MyI2C_Stop();                       // Send I2C Stop condition
}

// Function to read a byte of data from a specified MPU6050 register
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;
    
    MyI2C_Start();                      // Send I2C Start condition
    MyI2C_SendByte(MPU6050_ADDRESS);    // Send MPU6050 address with write bit (0)
    MyI2C_ReceiveAck();                 // Receive ACK
    MyI2C_SendByte(RegAddress);         // Send the register address to read from
    MyI2C_ReceiveAck();                 // Receive ACK
    
    MyI2C_Start();                      // Send repeated Start condition
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01); // Send MPU6050 address with read bit (1)
    MyI2C_ReceiveAck();                 // Receive ACK
    Data = MyI2C_ReceiveByte();         // Receive the data byte
    MyI2C_SendAck(1);                   // Send NACK after the last byte (no more data to read)
    MyI2C_Stop();                       // Send I2C Stop condition
    
    return Data;                        // Return the read data byte
}

// Function to initialize the MPU6050 by configuring its registers
void MPU6050_Init(void)
{
    MyI2C_Init();                       // Initialize I2C communication
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  // Wake up MPU6050 (set clock source)
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);  // Enable accelerometer and gyroscope
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);  // Set sample rate divider
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);      // Set low-pass filter (DLPF) config
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); // Set gyroscope sensitivity to ±2000 deg/s
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);// Set accelerometer sensitivity to ±16g
}

// Function to get the MPU6050 device ID
uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);    // Read the WHO_AM_I register and return device ID
}

// Function to get accelerometer and gyroscope data from MPU6050
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
                     int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t DataH, DataL;
    
    // Read and combine high and low bytes for each accelerometer axis (X, Y, Z)
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *AccX = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *AccY = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *AccZ = (DataH << 8) | DataL;                // Combine high and low bytes into 16-bit value
    
    // Read and combine high and low bytes for each gyroscope axis (X, Y, Z)
    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *GyroX = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
    
    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *GyroY = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
    
    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *GyroZ = (DataH << 8) | DataL;               // Combine high and low bytes into 16-bit value
}
