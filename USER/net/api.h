#ifndef __API_H
#define __API_H

#include <bsp_esp8266.h>

AT_StatusTypeDef fetch_time(char* wifi_name, char* wifi_pwd, char* time_buf);
//AT_StatusTypeDef weather_api_init();

//AT_StatusTypeDef fetch_weather();

#endif
