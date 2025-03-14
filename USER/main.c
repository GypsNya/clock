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
	char* time;
	char sync_time[8];
	uint32_t client_id;
	
	HSE_SetSysClock(25, 360, 2, 8);
	systick_init();
	gpio_init();
	usart_init();
	sdram_init();
	//rtc_init(25, 3, 7, 5, 23, 59, 0);

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

	for(int i = 0; i<3; i++){
		if (fetch_time("TDS_5G", "1905391447", sync_time) != AT_OK) {
			printf_char("fetch time failed", 0, 500, 0xFFFFFFFF, 32);
			delay_ms(200);
			en_char_continue_clear(0, 500, 17, 32);
			printf_char("retry", 0, 500, 0xFFFFFFFF, 32);
		} else {
			rtc_init(sync_time[0], sync_time[1], sync_time[2], sync_time[3], sync_time[4], sync_time[5], sync_time[6]);
			hal_result = rtc_alarm_set();
			if(hal_result != HAL_OK) {
				while(1);
			}
			en_char_continue_clear(0, 500, 17, 32);
			HAL_RTC_AlarmAEventCallback(&rtc_handle_struct);
			break;
		}
		delay_ms(1000);
	}

	p_result = jpg_decode("1:picture/1.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	while (1) {
		ESP8266_RecvString(&client_id, &time);
		for(int i = 0; i<8; i++) {
			sync_time[i] = ((char*)time)[i];
		}
		ESP8266_SendString(0, "OK", client_id);
		rtc_init(sync_time[0], sync_time[1], sync_time[2], sync_time[3], sync_time[4], sync_time[5], sync_time[6]);
		hal_result = rtc_alarm_set();
		if(hal_result != HAL_OK) {
			while(1);
		}
	}
}
