#include <main.h>

FATFS sd_drive;

int main(void) {
	UINT load_success_num;
	FRESULT result;
	JRESULT p_result;
	MemAllocStatusTypeDef mem_alloc_result;
	uint32_t mem_size;
	uint16_t jpg_handle[5];
	char current_time[14];
	
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
	rtc_init();
	
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
	/*result = load("1:/CascadiaCode.ttf", (uint32_t*)FONT_EN_TTF_BASE);
	if (result != FR_OK) {
		while(1);
	}*/

	if (lcd_init() != HAL_OK) {
		while(1);
	}
	if (lcd_fill(lcd_layer1_buffer, 0xFF000000) != HAL_OK) {
		while(1);
	}
	if (lcd_fill(lcd_layer2_buffer, 0x00000000) != HAL_OK) {
		while(1);
	}
	
	mem_page_alloc(&jpg_handle[0], 0x258000);
	mem_page_alloc(&jpg_handle[1], 0x258000);
	mem_page_alloc(&jpg_handle[2], 0x258000);
	mem_size = size_of(jpg_handle[0]);
	jpg_load_buff = jpg_handle[0];
	p_result = jpg_decode("1:picture/56.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	mov_in(jpg_handle[0], (char*)VRAM_BASE, (char*)0x00000000, 0x258000);
	p_result = jpg_decode("1:picture/21.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	mov_in(jpg_handle[1], (char*)VRAM_BASE, (char*)0x00000000, 0x258000);
	p_result = jpg_decode("1:picture/43.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	mov_in(jpg_handle[2], (char*)VRAM_BASE, (char*)0x00000000, 0x258000);
	for(uint32_t i = 0; ;i++) {
		get_rtc_time(current_time);
		char_continue_clear(100, 100, 7, 32);
		printf_time(current_time, 100, 100, 0xFF000000);
		mov_out(jpg_handle[i%3], (char*)0x00000000, (char*)VRAM_BASE, 0x258000);
		delay_ms(3000);
	}
}
