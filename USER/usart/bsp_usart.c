#include <bsp_usart.h>

UART_HandleTypeDef USART_HandleStruct;

void HAL_UART_MspInit (UART_HandleTypeDef *husart) {
	UNUSED(husart);
	
	usart_pin_init();
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void usart_pin_init() {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	usart_port_clk_en();
	
	GPIO_InitStruct.Pin = tx_pin | rx_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = af_mod;
	HAL_GPIO_Init(usart_pin_port, &GPIO_InitStruct);
}

void usart_init() {
	usart_clk_en();
	
	USART_HandleStruct.Instance = USART1;
	USART_HandleStruct.Init.BaudRate = 115200;
	USART_HandleStruct.Init.WordLength = UART_WORDLENGTH_8B;
	USART_HandleStruct.Init.StopBits = UART_STOPBITS_1;
	USART_HandleStruct.Init.Parity = UART_PARITY_NONE;
	USART_HandleStruct.Init.Mode = UART_MODE_TX_RX;
	//USART_HandleStruct.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&USART_HandleStruct) != HAL_OK) {
		while(1);
	}
	
	__HAL_UART_ENABLE_IT(&USART_HandleStruct, UART_IT_RXNE|UART_IT_IDLE);
}

void usart_send_string(uint8_t* str) {
	unsigned int i = 0;
	do{
		while(__HAL_UART_GET_FLAG(&USART_HandleStruct, UART_FLAG_TXE) == RESET);
		//HAL_USART_Transmit(&USART_HandleStruct, (uint8_t *)(str + i), 1, 1000);
		USART_HandleStruct.Instance->DR = (uint8_t)(*(str+i) & (uint8_t)0xFF);
		i++;
	}while(*(str + i) != '\0');
}

// int _write(int ch, char* pBuffer, int len) {
// 	UNUSED(ch);
// 	HAL_USART_Transmit(&USART_HandleStruct, (uint8_t *)pBuffer, len, 0xFFFF);
// 	return(len);
// }
