#ifndef __BSP_USART_H
#define __BSP_USART_H

#include <stm32f4xx_hal.h>

#ifndef __CORE_CM7_H
#define __CORE_CM7_H
#endif

#include <stm32f429xx.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_gpio.h>

//#include <stdio.h>
#include <stm32f4xx_hal_usart.h>

extern char* usart_recv_buff;

#define usart_port_clk_en()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define usart_pin_port		GPIOA
#define tx_pin				GPIO_PIN_9
#define rx_pin				GPIO_PIN_10
#define af_mod				GPIO_AF7_USART1

extern USART_HandleTypeDef USART_HandleStruct;

#define usart_clk_en()		__HAL_RCC_USART1_CLK_ENABLE()
#define usart_clk_dis()		__HAL_RCC_USART1_CLK_DISABLE()

void usart_pin_init(void);
void usart_init(void);

void usart_send_string(uint8_t* str);

#endif
