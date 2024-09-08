#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// RCC (Reset and Clock Control) Library
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Push-pull output
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // Pin 0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // set the speed
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIO (General Purpose Input/Output) Library
	// Configure GPIOA Pin 0 as output
	
	// Blink the LED in a loop
	while (1)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		Delay_ms(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		Delay_ms(500);
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
		Delay_ms(500);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
		Delay_ms(500);
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)0);
		Delay_ms(500);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)1);
		Delay_ms(500);
	}
}
