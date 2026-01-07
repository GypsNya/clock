#include <bsp_rtc.h>

RTC_HandleTypeDef rtc_handle_struct;
char sync_time[8];
uint8_t mounth_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint8_t sync_time_flag = 0;

HAL_StatusTypeDef rtc_clk_init(void) {
	HAL_StatusTypeDef hal_result = HAL_OK;
	RCC_OscInitTypeDef rcc_osc_init_struct;
	RCC_PeriphCLKInitTypeDef rcc_periphclk_init_struct;
	
	rtc_handle_struct.Instance = RTC;
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	
	#if RTC_CLOCK_SOURCE == LSI
		rcc_osc_init_struct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
		rcc_osc_init_struct.PLL.PLLState = RCC_PLL_NONE;
		rcc_osc_init_struct.LSIState = RCC_LSI_ON;
		rcc_osc_init_struct.LSEState = RCC_LSE_OFF;
		HAL_RCC_OscConfig(&rcc_osc_init_struct);
		rcc_periphclk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		rcc_periphclk_init_struct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
		HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_struct);
	#else
		rcc_osc_init_struct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
		rcc_osc_init_struct.PLL.PLLState = RCC_PLL_NONE;
		rcc_osc_init_struct.LSIState = RCC_LSI_OFF;
		rcc_osc_init_struct.LSEState = RCC_LSE_ON;
		HAL_RCC_OscConfig(&rcc_osc_init_struct);
		rcc_periphclk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		rcc_periphclk_init_struct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
		HAL_RCCEx_PeriphCLKConfig(&rcc_periphclk_init_struct);
	#endif
	
	__HAL_RCC_RTC_ENABLE();
	HAL_RTC_WaitForSynchro(&rtc_handle_struct);
	
	rtc_handle_struct.Init.AsynchPrediv = async_pre_div;
	rtc_handle_struct.Init.SynchPrediv = sync_pre_div;
	rtc_handle_struct.Init.HourFormat = RTC_HOURFORMAT_24;
	hal_result = HAL_RTC_Init(&rtc_handle_struct);
	if(hal_result != HAL_OK) {
		return hal_result;
	}
	return hal_result;
}

void rtc_time_set(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
	RTC_TimeTypeDef rtc_time_struct;
	RTC_DateTypeDef rtc_date_struct;
	uint8_t am_pm;

	if(hours > 12) {
		am_pm = RTC_HOURFORMAT12_PM;
	} else {
		am_pm = RTC_HOURFORMAT12_AM;
	}
	
	rtc_time_struct.Hours = hours;
	rtc_time_struct.Minutes = minutes;
	rtc_time_struct.Seconds = seconds;
	rtc_time_struct.TimeFormat = am_pm;
	HAL_RTC_SetTime(&rtc_handle_struct, &rtc_time_struct, RTC_FORMAT_BIN);
	
	rtc_date_struct.WeekDay = weekday;
	rtc_date_struct.Month = month;
	rtc_date_struct.Date = date;
	rtc_date_struct.Year = year;
	HAL_RTC_SetDate(&rtc_handle_struct, &rtc_date_struct, RTC_FORMAT_BIN);
	
	HAL_RTCEx_BKUPWrite(&rtc_handle_struct, RTC_BKP_DR0, RTC_BKP_DATA);
}

HAL_StatusTypeDef rtc_alarm_set(uint8_t sec) {
	HAL_StatusTypeDef hal_result = HAL_OK;
	RTC_AlarmTypeDef RTC_AlarmStructure;

	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	RTC_AlarmStructure.Alarm = RTC_ALARM_A;
	RTC_AlarmStructure.AlarmTime.Hours = 0;
	RTC_AlarmStructure.AlarmTime.Minutes = 0;
	RTC_AlarmStructure.AlarmTime.Seconds = sec;
	RTC_AlarmStructure.AlarmMask = RTC_ALARMMASK_ALL & (~RTC_ALARMMASK_SECONDS);
	RTC_AlarmStructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	RTC_AlarmStructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
	hal_result = HAL_RTC_SetAlarm_IT(&rtc_handle_struct, &RTC_AlarmStructure, RTC_FORMAT_BIN);
	if (hal_result != HAL_OK) {
		return hal_result;
	}
	return hal_result;
}

HAL_StatusTypeDef rtc_init(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
	HAL_StatusTypeDef hal_result = HAL_OK;
	
	hal_result = rtc_clk_init();
	if(hal_result != HAL_OK) {
		return hal_result;
	}
	hal_result = rtc_alarm_set(seconds);
	if(hal_result != HAL_OK) {
		return hal_result;
	}
	rtc_time_set(year, month, date, weekday, hours, minutes, seconds);

	return hal_result;
}

void get_rtc_time(char* time_buffer) {
	//RTC_TimeTypeDef rtc_time_struct;
	//RTC_DateTypeDef rtc_date_struct;
	char num_map[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	//HAL_RTC_GetTime(&rtc_handle_struct, &rtc_time_struct, RTC_FORMAT_BIN);
	//get time offset.
	/* rtc_time_struct.Hours = (rtc_time_struct.Hours+sync_time[4])%24;
	rtc_time_struct.Minutes = (rtc_time_struct.Minutes+sync_time[5])%60;
	rtc_time_struct.Seconds = (rtc_time_struct.Seconds+sync_time[6])%60; */
	sync_time_flag = 1;
	if ((sync_time[5]+1) == 60) {
		sync_time[4]=(sync_time[4]+1)%24;
		sync_time[5]=0;
	} else {
		sync_time[5]+=1;
	}

	//time_buffer[13] = num_map[rtc_time_struct.Seconds%10];
	//time_buffer[12] = num_map[rtc_time_struct.Seconds/10];
	time_buffer[11] = num_map[sync_time[5]%10];
	time_buffer[10] = num_map[sync_time[5]/10];
	time_buffer[9] = num_map[sync_time[4]%10];
	time_buffer[8] = num_map[sync_time[4]/10];
	
	//HAL_RTC_GetDate(&rtc_handle_struct, &rtc_date_struct, RTC_FORMAT_BIN);
	if (sync_time[4] == 0 && sync_time[5]==0) {
		if (sync_time[2] + 1 > mounth_day[(uint8_t)sync_time[1]]) {
			if (sync_time[1] + 1 > 12) {
				sync_time[0] += 1;
				sync_time[1] = 1;
				sync_time[2] = 1;
				sync_time[3] = (sync_time[3]+1)%7;
				if (sync_time[0] % 4 == 0) {
					mounth_day[2] = 29;
				}
			} else {
				sync_time[1] += 1;
				sync_time[2] = 1;
				sync_time[3] = (sync_time[3]+1)%7;
			}
		} else {
			sync_time[2] += 1;
			sync_time[3] = (sync_time[3]+1)%7;
		}
	}
	sync_time_flag = 0;
	//get date.
	time_buffer[7] = num_map[sync_time[3]%10];
	time_buffer[6] = num_map[sync_time[3]/10];
	time_buffer[5] = num_map[sync_time[2]%10];
	time_buffer[4] = num_map[sync_time[2]/10];
	time_buffer[3] = num_map[sync_time[1]%10];
	time_buffer[2] = num_map[sync_time[1]/10];
	time_buffer[1] = num_map[sync_time[0]%10];
	time_buffer[0] = num_map[sync_time[0]/10];
}

void set_rtc_offset_time(void) {
	RTC_TimeTypeDef rtc_time_struct;
	uint8_t sec_offset;
	if (sync_time[0] % 4 == 0) {
		mounth_day[2] = 29;
	} else {
		mounth_day[2] = 28;
	}
	if (sync_time[3] == 7) {
		sync_time[3] = 0;
	}
	HAL_RTC_GetTime(&rtc_handle_struct, &rtc_time_struct, RTC_FORMAT_BIN);
	rtc_alarm_set(rtc_time_struct.Seconds);
}
