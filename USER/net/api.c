#include <api.h>
#include <bsp_lcd.h>

//https://quan.suning.com/getSysTime.do
AT_StatusTypeDef fetch_time(char* wifi_name, char* wifi_pwd) {
	AT_StatusTypeDef result = AT_OK;

	result = ESP8266_Net_Mode_Choose(STA_AP);
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*0, 0, 0xFF000000, 12);

	result = ESP8266_DNS(DNS_USER_SET);
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*1, 0, 0xFF000000, 12);

	result = ESP8266_JoinAP(wifi_name, wifi_pwd);
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*2, 0, 0xFF000000, 12);

	result = ESP8266_Link_Server(TCP, "quan.suning.com", "80", Single_ID_0);
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*3, 0, 0xFF000000, 12);

	result = ESP8266_SendString(0, "GET /getSysTime.do HTTP/1.1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\nCache-Control: max-age=0\r\nConnection: keep-alive", Single_ID_0);
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*4, 0, 0xFF000000, 12);

	result = ESP8266_Close_Link();
	if(result != AT_OK){
		return result;
	}
	printf_char(esp8266_fram_struct.buff, 12*5, 0, 0xFF000000, 12);

	return result;
}
