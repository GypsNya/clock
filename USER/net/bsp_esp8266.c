#include <bsp_esp8266.h>
//#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <bsp_systick.h>

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
	usart_recv_buff = esp8266_fram_struct.buff;
	esp_port_clk_en();
	
	GPIO_InitStruct.Pin = esp_rst_pin | esp_en_pin;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(esp_pin_port, &GPIO_InitStruct);

	esp_rst_reset();
	esp_rst_set();
	esp_ch_set();
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
		return (strstr(esp8266_fram_struct.buff, expect_reply_1) || strstr(esp8266_fram_struct.buff, expect_reply_2));
	} else if (expect_reply_1 != 0) {
		return (strstr(esp8266_fram_struct.buff, expect_reply_1) != 0);
	} else {
		return (strstr(esp8266_fram_struct.buff, expect_reply_2) != 0);
	}
	return result;
}

AT_StatusTypeDef ESP8266_AT_Test(void) {
	AT_StatusTypeDef result = AT_OK;
	esp_rst_set();
	delay_ms(2000);
	for(int i = 0; i<10; i++) {
		if (ESP8266_Com("AT\r\n", "OK", "ERROR", 500)) {
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
			return ESP8266_Com("AT+CWMODE=1\r\n", "OK", "ERROR", 2500);
		case AP:
			return ESP8266_Com("AT+CWMODE=2\r\n", "OK", "ERROR", 2500);
		case STA_AP:
			return ESP8266_Com("AT+CWMODE=3\r\n", "OK", "ERROR", 2500);
		default:
			return AT_ERR;
	}
}

AT_StatusTypeDef ESP8266_JoinAP(char* name, char* pwd) {
	char cmd_buf[128];
	//sprintf(cmd_buf, "AT+CWJAP=\"", "name", "\",\"", "pwd", "\"");
	sprintf(cmd_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", name, pwd);
	return ESP8266_Com(cmd_buf, "OK", "ERROR", 10000);
}

AT_StatusTypeDef ESP8266_BuildAP(char* name, char* pwd, AP_PsdMode_TypeDef mode) {
	char cmd_buf[128];
	//char mode_buf[10];
	//sprintf(mode_buf, "%d", mode);
	sprintf(cmd_buf, "AT+CWSAP=\"%s\",\"%s\",1,%d\r\n", name, pwd, mode);
	//sprintf(cmd_buf, "AT+CWJAP=\"", "name", "\",\"", "pwd", "\",1,", mode_buf);
	return ESP8266_Com(cmd_buf, "OK", "ERROR", 5000);
}
AT_StatusTypeDef ESP8266_get_self_ip(void) {
	return ESP8266_Com("AT+CIFSR\r\n", "OK", "ERROR", 5000);
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
	if (id<5) sprintf(cmd_buf, "AT+CIPSTART=%d,%s\r\n", id, pre_buf);
	else sprintf(cmd_buf, "AT+CIPSTART=%s\r\n", pre_buf);
	return ESP8266_Com(cmd_buf, "OK", "ERROR", 4000);
}

AT_StatusTypeDef ESP8266_Close_Link(void) {
	char cmd_buff[128];
	AT_StatusTypeDef result = AT_OK;
	result = ESP8266_Com("AT+CIPCLOSE\r\n", "OK", "ERROR", 100);
	return result;
}

AT_StatusTypeDef ESP8266_as_Server(bool able, uint32_t port) {
	char cmd_buff[128];
	AT_StatusTypeDef result = AT_OK;
	if (able == 1) {
		sprintf(cmd_buff, "AT+CIPSERVER=1,%d\r\n", port);
		result = ESP8266_Com(cmd_buff, "OK", "ERROR", 1000);
	} else {
		result = ESP8266_Com("AT+CIPSERVER=0\r\n", "OK", "ERROR", 100);
	}
	return result;
}

AT_StatusTypeDef Enable_MultipleId(bool able) {
	char cmd_buf[16];
	sprintf(cmd_buf, "AT+CIPMUX=%d\r\n", able);
	return ESP8266_Com(cmd_buf, "OK", "ERROR", 500);
}

AT_StatusTypeDef ESP8266_UnvarnishSend(void) {
	if(!ESP8266_Cmd("AT+CIPMODE=1\r\n", "OK", 0, 500)) return AT_ERR;
	return ESP8266_Cmd("AT+CIPSEND\r\n", "OK", ">", 500);
}

AT_StatusTypeDef ESP8266_ExitUnvarnishSend (void) {
	delay_ms(1000);
	usart_send_string((uint8_t*)"+++");
	delay_ms(500);
}

LINK_StatusTypeDef Get_LinkStatus(void) {
	if(ESP8266_Com("AT+CIPSTATUS\r\n", "OK", "ERROR", 500)) {
		if(strstr(esp8266_fram_struct.buff, "STATUS:2\r\n")) {
			return 2;
		} else if (strstr(esp8266_fram_struct.buff, "STATUS:3\r\n")) {
			return 3;
		} else if (strstr(esp8266_fram_struct.buff, "STATUS:4\r\n")) {
			return 4;
		} else {
			return 0;
		}
	}
	return 0;
}

AT_StatusTypeDef ESP8266_DHCP_CUR(void) {
	return ESP8266_Com("AT+CWDHCP_CUR=1,1\r\n", "OK", "ERROR", 500);
}

/* AT_StatusTypeDef ESP8266_SendString(bool able, char* str, uint32_t len, ID_NO_TypeDef id) {
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
} */

AT_StatusTypeDef ESP8266_SendString(bool EnUnvarnishTx, char* str, ID_NO_TypeDef id) {
	AT_StatusTypeDef result = AT_OK;
	char cmd_buff[32];
	uint32_t len = strlen(str);

	if (EnUnvarnishTx) {
		usart_send_string((uint8_t*)str);
	} else {
		if (id < 5) {
			sprintf(cmd_buff, "AT+CIPSEND=%d,%d\r\n", id, len);
		} else {
			sprintf(cmd_buff, "AT+CIPSEND=%d\r\n", len);
		}
		result = ESP8266_Com(cmd_buff, ">", "ERROR", 5000);
		result = ESP8266_Com(str, "SEND OK", "SEND FAIL", 5000);
		/* result = ESP8266_Cmd(cmd_buff, ">", 0, 5000);
		result = ESP8266_Cmd(str, "SEND OK", 0, 2000); */
	}
	return result;
}

AT_StatusTypeDef ESP8266_RecvString(uint8_t* id, char** data_star_pos) {
	AT_StatusTypeDef result = AT_OK;
	esp8266_fram_struct.length = 0;
	esp8266_fram_struct.idle = 0;
	esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
	while(1) {
		if(esp8266_fram_struct.idle == 1) {
			*data_star_pos = strstr(esp8266_fram_struct.buff, "+IPD");
			if(*data_star_pos == 0) {
				return AT_ERR;
			} else {
				*data_star_pos+=4;
				esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
				*id = (uint8_t)((*data_star_pos)[1]) - 0x30;
				*data_star_pos = strstr(*data_star_pos, ":") + 1;
				if(*data_star_pos == 0) {
					return AT_ERR;
				}
				return AT_OK;
			}
		}
		delay_ms(50);
	}
	/* if(result == AT_OK) {
		//wait sever data.
		for(int i = 0; i<60; i++) {
			if (esp8266_fram_struct.length>esp8266_fram_struct.length+10) {
				data_star_pos = strstr(esp8266_fram_struct.buff, "+IPD");
				return (data_star_pos != 0);
			}
			delay_ms(500);
		}
		return AT_ERR;
	} */
}

AT_StatusTypeDef ESP8266_DNS(DNSHandle_TypeDef dns_handle) {
	char cmd_buff[128];
	AT_StatusTypeDef result = AT_OK;
	switch (dns_handle) {
		case DNS_AUTOMATIC_SET:
			result = ESP8266_Com("AT+CIPDNS_DEF=0\r\n", "OK", "ERROR", 2500);
			break;
		case DNS_USER_SET:
			sprintf(cmd_buff, "AT+CIPDNS_DEF=1,\"%s\",\"%s\"\r\n", DNS1, DNS2);
			result = ESP8266_Com(cmd_buff, "OK", "ERROR", 2500);
			break;
		case DNS_CURRENT_SET:
			result = ESP8266_Com("AT+CIPDNS_DEF?\r\n", "OK", "ERROR", 2500);
			break;
		default:
			result = AT_ERR;
			break;
	}
	return result;
}



AT_StatusTypeDef ESP8266_Com(char* cmd, char* ok_msg, char* err_msg, uint32_t wiat_time) {
	//AT_StatusTypeDef result = AT_OK;
	esp8266_fram_struct.length = 0;
	esp8266_fram_struct.idle = 0;
	esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
	usart_send_string((uint8_t*)cmd);
	
	for(int i = 0; i<wiat_time; i++){
		if(esp8266_fram_struct.idle == 1) {
			esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
			if(strstr(esp8266_fram_struct.buff, ok_msg) == 0) {
				if(strstr(esp8266_fram_struct.buff, err_msg) == 0) {
					esp8266_fram_struct.length = 0;
					esp8266_fram_struct.idle = 0;
					esp8266_fram_struct.buff[esp8266_fram_struct.length] = '\0';
				} else {
					return AT_ERR;
				}
			} else {
				return AT_OK;
			}
		}
		delay_ms(10);
	}
	
	return AT_ERR;
}
