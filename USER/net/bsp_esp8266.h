#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

//#include <stm32f4xx_hal.h>
#include <bsp_usart.h>
#include <bsp_led.h>

#define DNS1	"114.114.114.114"
#define DNS2	"223.5.5.5"
#define DNS3	"8.8.8.8"

typedef enum{
	INEQU = 0,
	EQU,
} CMP_RESULT;

#define esp_port_clk_en()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define esp_pin_port		GPIOA
#define esp_rst_pin			GPIO_PIN_0
#define esp_en_pin			GPIO_PIN_3
#define ENABLE		1
#define DISABLE		0

#define esp_high	led_on
#define esp_low		led_off
#define esp_ch_set()		digital(esp_pin_port, esp_en_pin, esp_high)
#define esp_rst_set()		digital(esp_pin_port, esp_rst_pin, esp_high)
#define esp_ch_reset()		digital(esp_pin_port, esp_en_pin, esp_low)
#define esp_rst_reset()		digital(esp_pin_port, esp_rst_pin, esp_low)

#define FRAME_LENGTH		2048
extern struct Esp8266_Fram_TypeDef {
	char buff[FRAME_LENGTH];
	uint32_t length;
	uint32_t idle;
} esp8266_fram_struct;

typedef uint8_t bool;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ID_NO_TypeDef;

typedef enum{
	TCP = 0,
	UDP,
} Net_Pro_TypeDef;

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} AP_PsdMode_TypeDef;

typedef enum {
	STA,
	AP,
	STA_AP
}NetModeChoose;

typedef enum {
	AT_ERR = 0,
	AT_OK,
}AT_StatusTypeDef;

typedef enum {
	GET_STATUS_ERR = 0,
	GOT_IP = 2,
	CONNECT_ESTABLISH = 3,
	DISCONNECTED = 4,
}LINK_StatusTypeDef;

typedef enum {
	DNS_AUTOMATIC_SET = 0,
	DNS_USER_SET,
	DNS_CURRENT_SET,
} DNSHandle_TypeDef;

void ESP8266_Init(void);
void ESP8266_Rst(void);
AT_StatusTypeDef ESP8266_Cmd(char* cmd, char* expect_reply_1, char* expect_reply_2, uint32_t wiat_time);

AT_StatusTypeDef ESP8266_AT_Test(void);
AT_StatusTypeDef ESP8266_Net_Mode_Choose(NetModeChoose mode);
AT_StatusTypeDef ESP8266_JoinAP(char* name, char* pwd);
AT_StatusTypeDef ESP8266_BuildAP(char* name, char* pwd, AP_PsdMode_TypeDef mode);
AT_StatusTypeDef ESP8266_Link_Server(Net_Pro_TypeDef protocol, char* ip, char* port, ID_NO_TypeDef id);
AT_StatusTypeDef ESP8266_Close_Link(void);
AT_StatusTypeDef Enable_MultipleId(bool able);
AT_StatusTypeDef ESP8266_UnvarnishSend(void);
AT_StatusTypeDef ESP8266_ExitUnvarnishSend (void);
LINK_StatusTypeDef Get_LinkStatus(void);
AT_StatusTypeDef ESP8266_SendString(bool EnUnvarnishTx, char* str, ID_NO_TypeDef id);
AT_StatusTypeDef ESP8266_DHCP_CUR(void);
AT_StatusTypeDef ESP8266_DNS(DNSHandle_TypeDef dns_handle);

//bool str_cmp(char* src_str, char* tar_str);
//void ESP8266_SendData(void);

#endif
