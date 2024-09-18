# FreeRTOS on STM32F103C8T6

This repository provides examples and methods to run FreeRTOS on the STM32F103C8T6 microcontroller. There are two different approaches to setting up and running FreeRTOS on this platform.

## 1. Using STM32CubeIDE Directly
In this method, you can generate the necessary configuration files and code for FreeRTOS using **STM32CubeMX**, and then directly work within **STM32CubeIDE** to build and run the project. (PS: STM32CubeIDE includes STM32CubeMX)

Steps:
- Configure your project in STM32CubeMX.
- Generate the code for FreeRTOS and peripherals.
- Open the project in STM32CubeIDE and continue development.

## 2. Using STM32CubeMX and Keil
This method involves generating code with **STM32CubeMX** and then importing it into another IDE such as **Keil** for further development. This is particularly useful if you're using a debugger like **DAPmini**, as Keil supports this debugger, while STM32CubeIDE does not.

Steps:
- Set up your project in STM32CubeMX.
- Choose Keil as the target IDE and generate the code.
- Open the project in **Keil** and continue development using the DAPmini debugger.

## Debugger Information
I am using **DAPmini** as the debugger instead of the default ST-Link. Since **Keil** supports DAPmini natively, I prefer to use STM32CubeMX with Keil for debugging purposes, as STM32CubeIDE does not currently support DAPmini.


