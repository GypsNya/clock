#ifndef __BSP_FSMC_H
#define __BSP_FSMC_H

#include <stm32f4xx_hal.h>
#include <stm32f429xx.h>

#ifndef __CORE_CM7_H
#define __CORE_CM7_H
#endif

#include <bsp_systick.h>

#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_rcc_ex.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_sdram.h>

#define sdram_clk_en()		__HAL_RCC_FMC_CLK_ENABLE()
#define sdram_port_clk_en()	__HAL_RCC_GPIOC_CLK_ENABLE(); \
							__HAL_RCC_GPIOD_CLK_ENABLE(); \
							__HAL_RCC_GPIOE_CLK_ENABLE(); \
							__HAL_RCC_GPIOF_CLK_ENABLE(); \
							__HAL_RCC_GPIOG_CLK_ENABLE()

#define sdram_c_port_pin	GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3
#define sdram_d_port_pin	GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15
#define sdram_e_port_pin	GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
#define sdram_f_port_pin	GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
#define sdram_g_port_pin	GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15

#define gpio_pin_mod		GPIO_AF12_FMC

//SDRAM config parameter
#define SDRAM_MODEREG_BURST_LENGTH_1				((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2				((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4				((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8				((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL			((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED		((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2					((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3					((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD		((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED	((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE		((uint16_t)0x0200)

extern SDRAM_HandleTypeDef sdram_handler;

#define sdram_addr	0XC0000000UL

//void sdram_gpio_init(void);
void sdram_init(void);
void sdram_init_seq(SDRAM_HandleTypeDef *hsdram);
HAL_StatusTypeDef sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval);

#endif
