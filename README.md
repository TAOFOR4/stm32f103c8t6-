# STM32F103C8T6

This repo is STM32F103C8T6 microcontroller with FreeRTOS using Keil and STM32CubeMX.

Both methods (HAL and Register-based) are viable for STM32 development, with HAL providing ease of use and register manipulation offering greater control.

## Table of Contents

- [STM32F103C8T6 Development using Keil](#stm32f103c8t6-development-using-keil)
  - [1. Development Using HAL (Hardware Abstraction Layer)](#1-development-using-hal-hardware-abstraction-layer)
  - [2. Development Using Direct Register Manipulation](#2-development-using-direct-register-manipulation)
- [Debugging Methods for Embedded Systems](#debugging-methods-for-embedded-systems)

## STM32F103C8T6 Development using Keil

This part contains two examples of developing applications for the STM32F103C8T6 microcontroller using Keil. One example is based on the **HAL (Hardware Abstraction Layer)** library, while the other uses **direct register manipulation** for more control over the hardware.

### 1. Development Using HAL (Hardware Abstraction Layer)

The **HAL** library is a higher-level abstraction provided by STM32CubeMX and STMicroelectronics, which simplifies working with STM32 peripherals.

### Steps:

1. **Install STM32CubeMX and Keil uVision**.
2. **Create a New Project Using STM32CubeMX**:
   - Select STM32F103C8T6 and configure peripherals.
   - Set the toolchain to MDK-ARM (Keil).
   - Generate code.
3. **Open the Project in Keil** and modify `main.c`.
4. **Compile and Flash the Code** to the STM32F103C8T6.

#### Example Code (HAL):

```c
#include "stm32f1xx_hal.h"

int main(void)
{
    HAL_Init();  // Initialize the HAL Library

    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable the GPIOA clock

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Toggle the LED
        HAL_Delay(500);  // 500 ms delay
    }
}
```

### 2. Development Using Direct Register Manipulation

Direct register programming offers finer control and faster execution by directly manipulating the hardware registers.

#### Steps:

1. **Create a New Keil Project** and select STM32F103C8T6 as the target. (Using [CMSIS-DAP](https://github.com/ARM-software/CMSIS-DAP))
2. **Add CMSIS and Startup Files**.
3. **Enable GPIOA Clock and Configure GPIO Registers**.
4. **Write the Main Application Code**.

#### Example Code (Register Programming):

```c
#include "stm32f10x.h"

void delay_ms(uint32_t ms);

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // Enable GPIOA clock

    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);  // Clear mode and CNF bits
    GPIOA->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0);  // Output mode, max speed 50 MHz
    GPIOA->CRL |= GPIO_CRL_CNF5_0;  // General purpose output push-pull

    while (1)
    {
        GPIOA->BSRR = GPIO_BSRR_BS5;  // Set Pin 5 high
        delay_ms(500);  // Delay 500 ms

        GPIOA->BSRR = GPIO_BSRR_BR5;  // Set Pin 5 low
        delay_ms(500);  // Delay 500 ms
    }
}

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();  // No operation
    }
}
```

## Debugging Methods for Embedded Systems

- **Serial Debugging**:

  - Send debug information to the computer via serial communication.
  - Use serial assistant tools (e.g., Tera Term, PuTTY) to display data.

- **Display Debugging**:

  - Connect a display (LCD, OLED) directly to the microcontroller.
  - Print debug information on the display in real-time.

- **Keil Debugging Mode**:

  - Use Keil's debug features: step-by-step execution, breakpoints, register/variable inspection.

- **JTAG/SWD Debugging**:

  - Use JTAG/SWD interfaces for real-time access to internal states.
  - Enables advanced debugging with breakpoints and tracing.

- **Logic Analyzer**:

  - Monitor communication protocols like I2C, SPI, or UART.
  - Capture and analyze electrical signals to debug communication issues.

- **On-Chip Debugging (OCD)**:

  - Use microcontroller’s built-in debugging hardware for real-time debugging.
  - Useful for power-sensitive applications.

- **Simulator Debugging**:
  - Simulate microcontroller behavior in IDEs (e.g., Keil) without hardware.
  - Perform unit testing and debugging in a virtual environment.
