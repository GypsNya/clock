#include <bsp_esp8266.h>
#include <stdarg.h>

struct Esp8266_Fram_TypeDef esp8266_fram_struct = { 0 };

// void sprintf(char* buf, ...) {
// 	uint32_t buf_idx = 0;
// 	char* str;
// 	uint32_t arg_num;
// 	va_list arg_list;
// 	va_start(arg_list, arg_num);
// 	for (int arg_idx = 0; arg_idx < arg_num; arg_idx++) {
// 		str = va_arg(arg_list, char*);
// 		for(int str_idx = 0; str[str_idx]!='\0'; str_idx++) {
// 			buf[buf_idx] = str[str_idx];
// 			buf_idx++;
// 		}
// 	}
// 	va_end(arg_list);
// }

CMP_RESULT str_cmp(char* src_str, char* tar_str) {
	char* src_str_addr = src_str;
	char* tar_str_addr = tar_str;
	while(1) {
		if (*src_str_addr != *tar_str_addr) {
			return INEQU;
		}
		if(*src_str_addr == "\0" && *tar_str_addr == "\0") {
			return EQU;
		}
		if(*src_str_addr == "\0" || *src_str_addr == "\0") {
			return INEQU;
		}
		src_str_addr++;
		tar_str_addr++;
	}
}

char* str_find(char* src_str, char* tar_str) {
	char* str_addr;
	char* src_str_addr = src_str;
	char* tar_str_addr = tar_str;
	while(1) {
		if(*src_str_addr == "\0") {
			return NULL;
		}
		if (*src_str_addr == *tar_str_addr) {
			str_addr = src_str_addr;
			while(1){
				if(*tar_str_addr == "\0"){
					return str_addr;
				}
				if(*src_str_addr == "\0"){
					return NULL;
				}
				if (*src_str_addr != *tar_str_addr) {
					tar_str_addr = tar_str;
					break;
				}
				src_str_addr++;
				tar_str_addr++;
			}
		}
		src_str_addr++;
	}
}

void ESP8266_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	usart_recv_buff = &esp8266_fram_struct.buff;
	esp_port_clk_en();
	
	GPIO_InitStruct.Pin = esp_rst_pin | esp_en_pin;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(esp_pin_port, &GPIO_InitStruct);

	esp_rst_set();
	esp_rst_reset();
}

void ESP8266_Rst(void) {
	esp_rst_reset();
	delay_ms(500);
	esp_rst_set();
}

AT_StatusTypeDef ESP8266_Cmd(char* cmd, char* expect_reply_1, char* expect_reply_2, uint32_t wiat_time) {
	AT_StatusTypeDef result = AT_OK;
	esp8266_fram_struct.length = 0;
	esp8266_fram_struct.idle = 0;
	usart_send_string((uint8_t*)cmd);
	delay_ms(wiat_time);
	esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
	if((expect_reply_1 != 0) && (expect_reply_2 != 0)) {
		return (str_find(expect_reply_1, esp8266_fram_struct.buff) || str_find(expect_reply_2, esp8266_fram_struct.buff));
	} else if (expect_reply_1 != 0) {
		return str_find(expect_reply_1, esp8266_fram_struct.buff);
	} else {
		return str_find(expect_reply_2, esp8266_fram_struct.buff);
	}
	return result;
}

AT_StatusTypeDef ESP8266_AT_Test(void) {
	AT_StatusTypeDef result = AT_OK;
	esp_rst_set();
	delay_ms(2000);
	for(int i = 0; i<10; i++) {
		if (ESP8266_Cmd("AT", "OK", NULL, 500)) {
			return result;
		}
		ESP8266_Rst();
	}
	result = AT_ERR;
	return result;
}

AT_StatusTypeDef ESP8266_Net_Mode_Choose(NetModeChoose mode) {
	switch (mode) {
		case STA:
			return ESP8266_Cmd("AT+CWMODE=1", "OK", "no change", 2500);
		case AP:
			return ESP8266_Cmd("AT+CWMODE=2", "OK", "no change", 2500);
		case STA_AP:
			return ESP8266_Cmd("AT+CWMODE=3", "OK", "no change", 2500);
		default:
			return AT_ERR;
	}
}

AT_StatusTypeDef ESP8266_JoinAP(char* name, char* pwd) {
	char cmd_buf[128];
	//sprintf(cmd_buf, "AT+CWJAP=\"", "name", "\",\"", "pwd", "\"");
	sprintf(cmd_buf, "AT+CWJAP=\"%s\",\"%s\"", name, pwd);
	return ESP8266_Cmd(cmd_buf, "OK", NULL, 5000);
}

AT_StatusTypeDef ESP8266_BuildAP(char* name, char* pwd, AP_PsdMode_TypeDef mode) {
	char cmd_buf[128];
	char mode_buf[10];
	itoa(mode, mode_buf, 10);
	sprintf(cmd_buf, "AT+CWSAP=\"%s\",\"%s\",1,%d", name, pwd, mode);
	//sprintf(cmd_buf, "AT+CWJAP=\"", "name", "\",\"", "pwd", "\",1,", mode_buf);
	return ESP8266_Cmd(cmd_buf, "OK", NULL, 5000);
}

AT_StatusTypeDef ESP8266_Link_Server(Net_Pro_TypeDef protocol, char* ip, char* port, ID_NO_TypeDef id) {
	char pre_buf[128];
	char cmd_buf[128];
	switch(protocol) {
		case TCP:
			sprintf(pre_buf, "\"%s\",\"%s\",%s", "TCP", ip, port);
			break;
		case UDP:
			sprintf(pre_buf, "\"%s\",\"%s\",%s", "UDP", ip, port);
			break;
		default:
			break;
	}
	if (id<5) sprintf(cmd_buf, "AT+CIPSTART=%d,%s", id, pre_buf);
	else sprintf(cmd_buf, "AT+CIPSTART=%s", pre_buf);
	return ESP8266_Cmd(cmd_buf, "OK", "ALREAY CONNECT", 4000);
}

AT_StatusTypeDef Enable_MultipleId(bool able) {
	char cmd_buf[16];
	sprintf(cmd_buf, "AT+CIPMUX=%d", able);
	return ESP8266_Cmd(cmd_buf, "OK", NULL, 500);
}

AT_StatusTypeDef ESP8266_UnvarnishSend(void) {
	if(!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 500)) return AT_ERR;
	return ESP8266_Cmd("AT+CIPSEND", "OK", ">", 500);
}

AT_StatusTypeDef ESP8266_ExitUnvarnishSend (void) {
	delay_ms(1000);
	usart_send_string((uint8_t*)"+++");
	delay_ms(500);
}

LINK_StatusTypeDef Get_LinkStatus(void) {
	if(ESP8266_Cmd("AT+CIPSTATUS", "OK", NULL, 500)) {
		if(str_find(esp8266_fram_struct.buff, "STATUS:2\r\n")) {
			return 2;
		} else if (str_find(esp8266_fram_struct.buff, "STATUS:3\r\n")) {
			return 3;
		} else if (str_find(esp8266_fram_struct.buff, "STATUS:4\r\n")) {
			return 4;
		} else {
			return 0;
		}
	}
	return 0;
}

AT_StatusTypeDef ESP8266_DHCP_CUR(void) {
	return ESP8266_Cmd("AT+CWDHCP_CUR=1,1", "OK", NULL, 500);
}

AT_StatusTypeDef ESP8266_SendString(bool able, char* str, uint32_t len, ID_NO_TypeDef id) {
	AT_StatusTypeDef result = AT_OK;
	char cmd_buf[32];
	if (able) {
		usart_send_string((uint8_t*)str);
	} else {
		if (id < 5) {
			sprintf(cmd_buf, "AT+CIPSEND=%d,%d", id, len+2);
		} else {
			sprintf(cmd_buf, "AT+CIPSEND=%d", len+2);
		}
		ESP8266_Cmd(cmd_buf, "> ", 0, 100);
		result = ESP8266_Cmd(str, "SEND OK", NULL, 500);
	}
	return result;
}

AT_StatusTypeDef ESP8266_SendString(bool EnUnvarnishTx, char* str, uint32_t len, ID_NO_TypeDef id) {
	AT_StatusTypeDef result = AT_OK;
	char cmd_buff[32];

	if (EnUnvarnishTx) {
		usart_send_string((uint8_t*)str);
	} else {
		if (id > 5) {
			sprintf(cmd_buff, "AT+CIPSEND=%d,%d", id, len+2);
		} else {
			sprintf(cmd_buff, "AT+CIPSEND=%d", len+2);
		}
		result = ESP8266_Cmd(cmd_buff, "SEND OK", 0, 500);
	}

	return result;
}
