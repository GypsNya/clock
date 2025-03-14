#include <api.h>
#include <bsp_lcd.h>

//https://quan.suning.com/getSysTime.do
AT_StatusTypeDef fetch_time(char* wifi_name, char* wifi_pwd, char* time_buf) {
	AT_StatusTypeDef result = AT_OK;
	uint32_t client_id;
	char* time;

	ESP8266_Rst();

	result = ESP8266_AT_Test();
	//printf_char(esp8266_fram_struct.buff, 0, 12*2, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	result = ESP8266_Net_Mode_Choose(STA);
	//printf_char(esp8266_fram_struct.buff, 0, 12*3, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	result = ESP8266_JoinAP(wifi_name, wifi_pwd);
	//printf_char(esp8266_fram_struct.buff, 0, 12*4, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	result = ESP8266_get_self_ip();
	//printf_char(esp8266_fram_struct.buff, 0, 12*5, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	/* result = ESP8266_Link_Server(TCP, "192.168.2.23", "80", Single_ID_0);
	printf_char(esp8266_fram_struct.buff, 0, 12*5, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	} */

	result = Enable_MultipleId(1);
	//printf_char(esp8266_fram_struct.buff, 0, 12*6, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	result = ESP8266_as_Server(1, 80);
	//printf_char(esp8266_fram_struct.buff, 0, 12*7, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}

	lcd_fill_rect(lcd_layer2_buffer, 0, 12*1, 3, 3, 0xFF000000);

	result = ESP8266_RecvString(&client_id, &time);
	//printf_char(esp8266_fram_struct.buff, 0, 12*8, 0xFFFFFFFF, 12);
	if(result != AT_OK){
		return result;
	}
	for(int i = 0; i<8; i++) {
		time_buf[i] = ((char*)time)[i];
	}
	//printf_char(time_buf, 0, 12*12, 0xFFFFFFFF, 18);
	lcd_fill_rect(lcd_layer2_buffer, 0, 12*1, 3, 3, 0x00000000);

	ESP8266_SendString(0, "OK", client_id);
	//printf_char(esp8266_fram_struct.buff, 0, 12*9, 0xFFFFFFFF, 12);

	return result;
}
