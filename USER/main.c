#include <main.h>

FATFS sd_drive;
char refresh_buffer[8];
uint32_t refresh_times = 0;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	char current_time[15];
	get_rtc_time(current_time);
	if((uint8_t)(current_time[8]) < 0x33) {
		en_char_continue_clear(192, 200, 5, 256);
		printf_time(current_time, 192, 200, 0xFFFFFFFF, 256);
		en_char_continue_clear(0,0,20,72);
		printf_date(current_time, 0, 0, 0xFFFFFFFF);
		printf_week(sync_time, 500, 0, 0xFFFFFFFF);
	}
	refresh_times += 1;
	sprintf(refresh_buffer, "%d", refresh_times);
	printf_char(refresh_buffer, 0, 500, 0xFFFFFFFF, 32);
}

int main(void) {
	FRESULT result;
	JRESULT p_result;
	HAL_StatusTypeDef hal_result;
	char* time;
	uint8_t client_id;
	char num_buffer[8];
	uint32_t recv_times = 0;
	
	HSE_SetSysClock(25, 360, 2, 8);
	systick_init();
	gpio_init();
	usart_init();
	sdram_init();
	rtc_init(24, 1, 1, 1, 0, 0, 1);

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
			//rtc_init(sync_time[0], sync_time[1], sync_time[2], sync_time[3], sync_time[4], sync_time[5], sync_time[6]);
			en_char_continue_clear(0, 500, 17, 32);
			while(sync_time_flag == 1);
			sync_time_flag = 1;
			set_rtc_offset_time();
			sync_time_flag = 0;
			//HAL_RTC_AlarmAEventCallback(&rtc_handle_struct);
			break;
		}
		delay_ms(1000);
	}

	result = f_mount(&sd_drive, "1:", 1);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/pixel_font_72.FON", (uint32_t*)FONT_BASE);
	if (result != FR_OK) {
		while(1);
	}
	result = load("1:/CascadiaCode.ttf", (uint32_t*)FONT_EN_TTF_BASE);
	if (result != FR_OK) {
		while(1);
	}

	p_result = jpg_decode("1:picture/1.jpg");
	if (p_result != JDR_OK) {
		while(1);
	}
	while (1) {
		ESP8266_RecvString(&client_id, &time);
		while(sync_time_flag == 1);
		sync_time_flag = 1;
		if ((char*)time[0] == 'T' && (char*)time[1] == 'I') {
			time += 2;
			for(int i = 0; i<8; i++) {
				sync_time[i] = ((char*)time)[i];
			}
			set_rtc_offset_time();
			sync_time_flag = 0;
			recv_times += 1;
			sprintf(num_buffer, "%d", recv_times);
			printf_char(num_buffer, 0, 532, 0xFFFFFFFF, 32);
			ESP8266_SendString(0, "OK", client_id);
		}
		//rtc_init(sync_time[0], sync_time[1], sync_time[2], sync_time[3], sync_time[4], sync_time[5], sync_time[6]);
/* 		hal_result = rtc_alarm_set();
		if(hal_result != HAL_OK) {
			while(1);
		} */
	}
}
