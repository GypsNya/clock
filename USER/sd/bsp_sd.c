#include <bsp_sd.h>

SD_HandleTypeDef SD_HandleStrcut;
DMA_HandleTypeDef DMA_rx_HandleStrcut;
DMA_HandleTypeDef DMA_tx_HandleStrcut;
volatile uint8_t TX_Flag=0;
volatile uint8_t RX_Flag=0; 

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
	UNUSED(hsd);
	TX_Flag = 1;
}
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
	UNUSED(hsd);
	RX_Flag = 1;
}
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) {
	UNUSED(hsd);
}
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd) {
	UNUSED(hsd);
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	sdio_clk_en();
	sd_dma_clk_en();
	sd_port_clk_en();
	
	GPIO_InitStruct.Pin = sd_c_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(sd_c_port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = sd_d_pin;
	HAL_GPIO_Init(sd_d_port, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(SDIO_IRQn);
	
	DMA_rx_HandleStrcut.Instance = sd_dma_rx_stream;
	DMA_rx_HandleStrcut.Init.Channel = sd_dma_channel;
	DMA_rx_HandleStrcut.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_rx_HandleStrcut.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_rx_HandleStrcut.Init.MemInc = DMA_MINC_ENABLE;
	DMA_rx_HandleStrcut.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DMA_rx_HandleStrcut.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	DMA_rx_HandleStrcut.Init.Mode = DMA_PFCTRL;
	DMA_rx_HandleStrcut.Init.Priority = DMA_PRIORITY_HIGH;
	DMA_rx_HandleStrcut.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	DMA_rx_HandleStrcut.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DMA_rx_HandleStrcut.Init.MemBurst = DMA_MBURST_INC4;
	DMA_rx_HandleStrcut.Init.PeriphBurst = DMA_PBURST_INC4;
	
	__HAL_LINKDMA(hsd, hdmarx, DMA_rx_HandleStrcut);
	HAL_DMA_DeInit(&DMA_rx_HandleStrcut);
	HAL_DMA_Init(&DMA_rx_HandleStrcut);
	
	DMA_tx_HandleStrcut.Instance = sd_dma_tx_stream;
	DMA_tx_HandleStrcut.Init.Direction = DMA_MEMORY_TO_PERIPH;
	DMA_tx_HandleStrcut.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_tx_HandleStrcut.Init.MemInc = DMA_MINC_ENABLE;
	DMA_tx_HandleStrcut.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DMA_tx_HandleStrcut.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	DMA_tx_HandleStrcut.Init.Mode = DMA_PFCTRL;
	DMA_tx_HandleStrcut.Init.Priority = DMA_PRIORITY_HIGH;
	DMA_tx_HandleStrcut.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	DMA_tx_HandleStrcut.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DMA_tx_HandleStrcut.Init.MemBurst = DMA_MBURST_INC4;
	DMA_tx_HandleStrcut.Init.PeriphBurst = DMA_PBURST_INC4;
	__HAL_LINKDMA(hsd, hdmatx, DMA_tx_HandleStrcut);
	HAL_DMA_DeInit(&DMA_tx_HandleStrcut);
	HAL_DMA_Init(&DMA_tx_HandleStrcut);
	
	HAL_NVIC_SetPriority(sd_dma_rx_irq, 6, 0);
	HAL_NVIC_EnableIRQ(sd_dma_rx_irq);
	HAL_NVIC_SetPriority(sd_dma_tx_irq, 6, 0);
	HAL_NVIC_EnableIRQ(sd_dma_tx_irq);
}

HAL_StatusTypeDef sd_init(void) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	
	//be attention HAL_SD_Init wouldn't modify SDIO register to your set, it have default set.
	SD_HandleStrcut.Instance = SDIO;
	SD_HandleStrcut.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	SD_HandleStrcut.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	SD_HandleStrcut.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	SD_HandleStrcut.Init.BusWide = SDIO_BUS_WIDE_1B;
	SD_HandleStrcut.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	SD_HandleStrcut.Init.ClockDiv = SDIO_INIT_CLK_DIV;
	//HAL_SD_MspInit(&SD_HandleStrcut);
	hal_state = HAL_SD_Init(&SD_HandleStrcut);
	if (hal_state != HAL_OK) {
		return hal_state;
	}
	
	//if you want modify default set of SDIO register, use hal function like this.
	HAL_SD_ConfigWideBusOperation(&SD_HandleStrcut, SDIO_BUS_WIDE_4B);
	
	//hal_state = HAL_SD_Erase(&SD_HandleStrcut, 0, 5);
	return hal_state;
}

HAL_StatusTypeDef get_all_sd_card_info(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	
	sd_info_struct->state = HAL_SD_GetCardState(&SD_HandleStrcut);
	
	hal_state = HAL_SD_GetCardCID(&SD_HandleStrcut, &sd_info_struct->CID);
	if (hal_state != HAL_OK) {
		return hal_state;
	}
	
	hal_state = HAL_SD_GetCardCSD(&SD_HandleStrcut, &sd_info_struct->CSD);
	if (hal_state != HAL_OK) {
		return hal_state;
	}
	
	hal_state = HAL_SD_GetCardStatus(&SD_HandleStrcut, &sd_info_struct->status);
	if (hal_state != HAL_OK) {
		return hal_state;
	}
	
	hal_state = HAL_SD_GetCardInfo(&SD_HandleStrcut, &sd_info_struct->info);
	if (hal_state != HAL_OK) {
		return hal_state;
	}
	return hal_state;
}

HAL_StatusTypeDef get_sd_card_state(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	sd_info_struct->state = HAL_SD_GetCardState(&SD_HandleStrcut);
	return hal_state;
}
HAL_StatusTypeDef get_sd_card_CID(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	hal_state = HAL_SD_GetCardCID(&SD_HandleStrcut, &sd_info_struct->CID);
	return hal_state;
}
HAL_StatusTypeDef get_sd_card_CSD(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	hal_state = HAL_SD_GetCardCSD(&SD_HandleStrcut, &sd_info_struct->CSD);
	return hal_state;
}
HAL_StatusTypeDef get_sd_card_status(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	hal_state = HAL_SD_GetCardStatus(&SD_HandleStrcut, &sd_info_struct->status);
	return hal_state;
}
HAL_StatusTypeDef get_sd_card_info(sd_card_info* sd_info_struct) {
	HAL_StatusTypeDef hal_state = HAL_OK;
	hal_state = HAL_SD_GetCardInfo(&SD_HandleStrcut, &sd_info_struct->info);
	return hal_state;
}
