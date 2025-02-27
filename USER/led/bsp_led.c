#include <bsp_led.h>

void gpio_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	led_port_clk_en();
	
	GPIO_InitStruct.Pin = led1 | led2;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(led_port, &GPIO_InitStruct);
}
