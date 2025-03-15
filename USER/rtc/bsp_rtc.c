#include <bsp_rtc.h>

RTC_HandleTypeDef rtc_handle_struct;

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

	hal_result = HAL_RTC_DeInit(&rtc_handle_struct);
	if(hal_result != HAL_OK) {
		return hal_result;
	}
	
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

HAL_StatusTypeDef rtc_alarm_set(void) {
	HAL_StatusTypeDef hal_result = HAL_OK;
	RTC_AlarmTypeDef RTC_AlarmStructure;

	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	RTC_AlarmStructure.Alarm = RTC_ALARM_A;
	RTC_AlarmStructure.AlarmTime.Hours = 0;
	RTC_AlarmStructure.AlarmTime.Minutes = 0;
	RTC_AlarmStructure.AlarmTime.Seconds = 0;
	RTC_AlarmStructure.AlarmMask = RTC_ALARMMASK_ALL & (~RTC_ALRMAR_MSK1);
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
	rtc_time_set(year, month, date, weekday, hours, minutes, seconds);
	hal_result = rtc_alarm_set();
	if(hal_result != HAL_OK) {
		return hal_result;
	}

	return hal_result;
}

void get_rtc_time(char* time_buffer) {
	RTC_TimeTypeDef rtc_time_struct;
	RTC_DateTypeDef rtc_date_struct;
	char num_map[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	HAL_RTC_GetTime(&rtc_handle_struct, &rtc_time_struct, RTC_FORMAT_BIN);
	time_buffer[13] = num_map[rtc_time_struct.Seconds%10];
	time_buffer[12] = num_map[rtc_time_struct.Seconds/10];
	time_buffer[11] = num_map[rtc_time_struct.Minutes%10];
	time_buffer[10] = num_map[rtc_time_struct.Minutes/10];
	time_buffer[9] = num_map[rtc_time_struct.Hours%10];
	time_buffer[8] = num_map[rtc_time_struct.Hours/10];
	
	HAL_RTC_GetDate(&rtc_handle_struct, &rtc_date_struct, RTC_FORMAT_BIN);
	time_buffer[7] = num_map[rtc_date_struct.WeekDay%10];
	time_buffer[6] = num_map[rtc_date_struct.WeekDay/10];
	time_buffer[5] = num_map[rtc_date_struct.Date%10];
	time_buffer[4] = num_map[rtc_date_struct.Date/10];
	time_buffer[3] = num_map[rtc_date_struct.Month%10];
	time_buffer[2] = num_map[rtc_date_struct.Month/10];
	time_buffer[1] = num_map[rtc_date_struct.Year%10];
	time_buffer[0] = num_map[rtc_date_struct.Year/10];
}
