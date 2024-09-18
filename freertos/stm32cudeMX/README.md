# FreeRTOS on STM32F103C8T6 with Keil ARM-MDK

This part is about using **STM32CubeMX** to create a FreeRTOS project for STM32F1 series microcontrollers and then executing the project using **Keil ARM-MDK**.

However, there is an [issue](https://github.com/STMicroelectronics/stm32_mw_freertos/issues/5) related to the STM32CubeMX FreeRTOS middleware for STM32F1 devices. This issue might affect the successful execution of FreeRTOS on STM32F1 series microcontrollers when using the STM32CubeMX-generated code.

Hopefully, the release of **STM32CubeMX 6.13.0** will address and fix this issue.
