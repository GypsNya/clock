#ifndef __BSP_SD_H
#define __BSP_SD_H

#include <stm32f4xx_hal.h>

#ifndef __CORE_CM4_H
#define __CORE_CM4_H
#endif

#ifndef __STM32F4xx_HAL_SD_H
#define __STM32F4xx_HAL_SD_H
#endif

//#include <stm32f429xx.h>
//#include <stm32f4xx_hal_rcc.h>
//#include <stm32f4xx_hal_rcc_ex.h>
//#include <stm32f4xx_hal_gpio.h>
#define sd_port_clk_en()	__HAL_RCC_GPIOC_CLK_ENABLE();\
							__HAL_RCC_GPIOD_CLK_ENABLE()
#define sd_c_port		GPIOC
#define sd_d_port		GPIOD
#define sd_c_pin		GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12
#define sd_d_pin		GPIO_PIN_2

extern DMA_HandleTypeDef DMA_rx_HandleStrcut;
extern DMA_HandleTypeDef DMA_tx_HandleStrcut;
#define sd_dma_clk_en() 		__HAL_RCC_DMA2_CLK_ENABLE()
#define sd_dma_rx_stream		DMA2_Stream3
#define sd_dma_tx_stream		DMA2_Stream6
#define sd_dma_channel			DMA_CHANNEL_4
#define sd_dma_rx_irq			DMA2_Stream3_IRQn
#define sd_dma_tx_irq			DMA2_Stream6_IRQn
#define sd_dma_rx_irq_handler	DMA2_Stream3_IRQHandler
#define sd_dma_tx_irq_handler	DMA2_Stream6_IRQHandler

extern SD_HandleTypeDef SD_HandleStrcut;
extern volatile uint8_t TX_Flag;
extern volatile uint8_t RX_Flag;
#define SD_TIMEOUT ((uint32_t)0x00100000U)
#define sdio_clk_en()	__HAL_RCC_SDIO_CLK_ENABLE()

typedef struct {
	uint32_t					state;
	HAL_SD_CardCIDTypeDef		CID;
	HAL_SD_CardCSDTypeDef		CSD;
	HAL_SD_CardStatusTypeDef	status;
	HAL_SD_CardInfoTypeDef		info;
} sd_card_info;

HAL_StatusTypeDef sd_init(void);

HAL_StatusTypeDef get_all_sd_card_info(sd_card_info* sd_info_struct);
HAL_StatusTypeDef get_sd_card_state(sd_card_info* sd_info_struct);
HAL_StatusTypeDef get_sd_card_CID(sd_card_info* sd_info_struct);
HAL_StatusTypeDef get_sd_card_CSD(sd_card_info* sd_info_struct);
HAL_StatusTypeDef get_sd_card_status(sd_card_info* sd_info_struct);
HAL_StatusTypeDef get_sd_card_info(sd_card_info* sd_info_struct);

#endif
