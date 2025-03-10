#include <main.h>

FATFS sd_drive;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	char current_time[14];
	get_rtc_time(current_time);
	char_continue_clear(192, 200, 3, 256);
	printf_time(current_time, 192, 200, 0xFFFFFFFF, 256);
}

int main(void) {
	FRESULT result;
	JRESULT p_result;
	HAL_StatusTypeDef hal_result;
<<<<<<< HEAD
	MemAllocStatusTypeDef mem_alloc_result;
=======
>>>>>>> 01d07c1 (feat: connect internet.)
	
	/*uint32_t page_num = PAGE_NUM;
	uint32_t theory_page_num = THEORY_PAGE_NUM;
	uint32_t page_map_start = PAGE_MAP_START;
	uint32_t page_info_map_start = PAGE_INFO_MAP_START;
	uint32_t page_info_start = PAGE_INFO_TAB_START;
	uint32_t page_start = PAGE_START;
	
	uint32_t page_map_size = PAGE_MAP_SIZE;
	uint32_t page_info_map_size = PAGE_INFO_MAP_SIZE;
	uint32_t page_info_tab_size = PAGE_INFO_TAB_SIZE;*/
	
	HSE_SetSysClock(25, 360, 2, 8);
	systick_init();
	gpio_init();
	usart_init();
	sdram_init();
<<<<<<< HEAD
	rtc_init(25, 3, 7, 5, 15, 59, 0);
	//rtc_time_set(25, 3, 7, 5, 15, 59, 0);
	delay_ms(1000*60*2);
	rtc_init(25, 3, 7, 5, 23, 59, 0);
=======
	rtc_init();
	rtc_time_set(25, 3, 4, 2, 15, 25, 0);
>>>>>>> 01d07c1 (feat: connect internet.)
	
	mem_page_init();
	
	result = f_mount(&sd_drive, "1:", 1);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/pixel_font.FON", (uint32_t*)FONT_BASE);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/pixel_font_en.FON", (uint32_t*)FONT_EN_BASE);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/pixel_font_72.FON", (uint32_t*)FONT_72_BASE);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/CascadiaCode.ttf", (uint32_t*)FONT_EN_TTF_BASE);
	if (result != FR_OK) {
		while(1);
	}

	if (lcd_init() != HAL_OK) {
		while(1);
	}
	if (lcd_fill(lcd_layer1_buffer, 0xFF000000) != HAL_OK) {
		while(1);
	}
	if (lcd_fill(lcd_layer2_buffer, 0x00000000) != HAL_OK) {
		while(1);
	}
	// if (display_ttf_char("A", 128, 100, 100, 0xFF000000) == 0) {
	// 	lcd_fill_rect(lcd_layer2_buffer, 0, 0, 10, 10, 0xFFFFFFFF);
	// }
	//printf_char("1234", 100, 100, 0xFF000000, 128);
	p_result = jpg_decode("1:picture/1.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	hal_result = rtc_alarm_set();
	if(hal_result != HAL_OK) {
		while(1);
	}
	//delay_ms(3000);
	while(1);
}
