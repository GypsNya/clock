#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include <stm32f4xx_hal.h>

#ifndef __CORE_CM7_H
#define __CORE_CM7_H
#endif

#include <stm32f429xx.h>

#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_rtc.h>
#include <stm32f4xx_hal_rtc_ex.h>

#define LSE					1
#define LSI					0

#define RTC_CLOCK_SOURCE	LSE

#define async_pre_div		0x7F
#define sync_pre_div		0xFF

//time
#define RTC_HOUR_MODE		RTC_HOURFORMAT_24
#define HOURS				9
#define MINUTES				50
#define SECONDS				00

//date
#define WEEKDAY				6
#define DATE				22
#define MONTH				2
#define YEAR				25

//#define RTC_FORMAT_BIN

//BACEUP
#define RTC_BKP_DATA		0x32F2

extern RTC_HandleTypeDef rtc_handle_struct;

HAL_StatusTypeDef rtc_clk_init(void);
HAL_StatusTypeDef rtc_init(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);

void rtc_time_set(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef rtc_alarm_set(void);
void get_rtc_time(char* time_buffer);

#endif
