#ifndef __BSP_LED_H
#define __BSP_LED_H

#include <stm32f4xx_hal.h>

#ifndef __CORE_CM4_H
#define __CORE_CM4_H
#endif

#include <stm32f429xx.h>
#include <stm32f4xx_hal_rcc.h>
//#include <stm32f4xx_hal_rcc_ex.h>
#include <stm32f4xx_hal_gpio.h>
//#include <stm32f4xx_hal_gpio_ex.h>

#define digital(p, i, d)			{p->BSRR |= (uint32_t)i << d;}
#define digital_toggle(p, i)		{p->ODR ^= i;}
#define led_on		0
#define led_off		16

#define led_port_clk_en()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define led_port_clk_dis()			__HAL_RCC_GPIOB_CLK_DISABLE()

#define led_port	GPIOB
#define led1		GPIO_PIN_0
#define led2		GPIO_PIN_1

#define led(p, s)			digital(led_port, p, s)
#define led_toggle(p)		digital_toggle(led_port, p)

void gpio_init(void);

#endif
