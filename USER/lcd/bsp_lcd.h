#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include <stm32f4xx_hal.h>
#include <stm32f429xx.h>

#ifndef __CORE_CM4_H
#define __CORE_CM4_H
#endif

// #include <stm32f4xx_hal_dma2d.h>
// #include <stm32f4xx_hal_ltdc.h>
// #include <stm32f4xx_hal_rcc.h>
// #include <stm32f4xx_hal_rcc_ex.h>
// #include <stm32f4xx_hal_gpio.h>
// #include <stm32f4xx_hal_gpio_ex.h>
#include <bsp_geek_429.h>
#include "dis_ttf_font.h"

#define lcd_clk_en()	__HAL_RCC_LTDC_CLK_ENABLE()
#define dma2d_clk_en()	__HAL_RCC_DMA2D_CLK_ENABLE()

#define gpio_clk_en()	__HAL_RCC_GPIOB_CLK_ENABLE();\
						__HAL_RCC_GPIOF_CLK_ENABLE();\
						__HAL_RCC_GPIOG_CLK_ENABLE();\
						__HAL_RCC_GPIOH_CLK_ENABLE();\
						__HAL_RCC_GPIOI_CLK_ENABLE()
						

#define lcd_gpio_b		GPIO_PIN_5
#define lcd_gpio_f		GPIO_PIN_10
#define lcd_gpio_g		GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11
#define lcd_gpio_h		GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
#define lcd_gpio_i		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10

#define lcd_af_mod		GPIO_AF14_LTDC

//get from lcd info.
#define HSW		20
#define VSW		3
#define HBP		140
#define VBP		20
#define HFP		160
#define VFP		12
#define LCD_PIXEL_WIDTH		1024
#define LCD_PIXEL_HEIGHT	600

#define PIXEL_FORMAT	DMA2D_OUTPUT_ARGB8888

#if PIXEL_FORMAT == LTDC_PIXEL_FORMAT_RGB888
	#define PIXEL_SIZE	4
	#define DMA2D_PIXEL_FORMAT	DMA2D_OUTPUT_RGB888
#elif PIXEL_FORMAT == LTDC_PIXEL_FORMAT_ARGB8888
	#define PIXEL_SIZE	4
	#define DMA2D_PIXEL_FORMAT	DMA2D_OUTPUT_ARGB8888
#elif PIXEL_FORMAT == LTDC_PIXEL_FORMAT_RGB565
	#define PIXEL_SIZE	2
	#define DMA2D_PIXEL_FORMAT	DMA2D_OUTPUT_RGB565
#endif

#define lcd_layer1_buffer	VRAM_BASE
#define lcd_layer2_buffer	(VRAM_BASE + (LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT * PIXEL_SIZE))

extern LTDC_HandleTypeDef ltdc_HandleStruct;

//init function
uint32_t get_lcd_id(void);

HAL_StatusTypeDef lcd_init(void);
HAL_StatusTypeDef lcd_clk_init(void);
HAL_StatusTypeDef lcd_layer_init(uint32_t layer_index, uint32_t address, uint32_t pixel_format);
void lcd_on(void);



//handler function
#define lcd_fill(layer_index, color)		lcd_fill_rect(layer_index, 0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT,color)

HAL_StatusTypeDef lcd_fill_rect(uint32_t layer_index, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void lcd_draw_point(uint32_t layer_address, uint32_t x, uint32_t y, uint32_t color);

//display chinese character.
//#define FONT_SIZE	32			//character size.
#define FONT_CODE	GB2312		//character code. ref font_code.

#if FONT_CODE == GB2312
	#define char_code_pos(first_byte_addr, font_size)		(*(char*)first_byte_addr > 0x7f ? (((*(char*)first_byte_addr-0xa1)*94+*(first_byte_addr+1)-0xa1)*font_size*font_size/8):(*(char*)first_byte_addr)*font_size*font_size/2/8)
	//#define char_code_pos(first_byte_addr, font_size)		(((*(char*)first_byte_addr-0xa1)*94+*(first_byte_addr+1)-0xa1)*font_size*font_size/8)
#elif FONT_CODE == BIG5
	#define char_code_pos(high_byte, low_byte)		((high_byte - 0x81) * 157 + (low_byte - (0x40 < low_byte < 0x7E? 0x40 : 0x40+63)))
#elif FONT_CODE == UTF8
	#define char_code_pos(first_byte)				*first_byte & 0x80 == 0 ? 1:\
			/*char len 2*/							(*first_byte & 0xE0 < 0xE0)?(*first_byte&0x1f + *(first_byte+1)&0x3f):\
			/*char len 3*/							(*first_byte & 0xF0 < 0xF0)?(*first_byte&0x0f + *(first_byte+1)&0x3f + *(first_byte+2)&0x3f):\
			/*char len 4*/							(*first_byte & 0xF8 < 0xF8)?(*first_byte&0x7 + *(first_byte+1)&0x3f + *(first_byte+2)&0x3f + *(first_byte+3)&0x3f):\
			/*char len 5*/							(*first_byte & 0xFC < 0xFC)?(*first_byte&0x3 + *(first_byte+1)&0x3f + *(first_byte+2)&0x3f + *(first_byte+3)&0x3f + *(first_byte+4)&0x3f):\
			/*char len 6*/							(*first_byte&0x1 + *(first_byte+1)&0x3f + *(first_byte+2)&0x3f + *(first_byte+3)&0x3f + *(first_byte+4)&0x3f + *(first_byte+5)&0x3f)
												/*	if(first_byte & 0x80 == 0) return 1;
												\	else if (first_byte & 0xE0 < 0xE0) return 2;
												\	else if (first_byte & 0xF0 < 0xF0) return 3;
												\	else if (first_byte & 0xF8 < 0xF8) return 4;
												\	else if (first_byte & 0xFC < 0xFC) return 5;
												\	else return 6;
												*/
#endif

#define is_chinese(first_byte_addr)		((char)first_byte_addr > 0x7f)
#define is_str_end(byte_addr)				((char)byte_addr == 0x00)
	
//font_code.
#define GB2312
#define BIG5
#define UTF8

typedef enum {
	LCD_DISPLAY_OK = 0,
	LCD_DISPLAY_FLOW,
}LCD_StatusTypeDef;

#define char_clear(x, y, size)						lcd_fill_rect(lcd_layer2_buffer, x, y, size, size, 0x00000000)
#define char_continue_clear(x, y, char_num, size)	lcd_fill_rect(lcd_layer2_buffer, x, y, char_num*size, size, 0x00000000)

void display_char(char* char_buffer, uint32_t size, uint32_t x, uint32_t y, uint32_t color);
void display_ttf_char(char* char_buffer, uint32_t size, uint32_t x, uint32_t y, uint32_t color);
LCD_StatusTypeDef printf_char(char* char_buffer, uint32_t x, uint32_t y, uint32_t color);

//time
LCD_StatusTypeDef printf_time(char* time_buffer, uint32_t x, uint32_t y, uint32_t color);
LCD_StatusTypeDef printf_date(char* time_buffer, uint32_t x, uint32_t y, uint32_t color);
LCD_StatusTypeDef printf_week(char* time_buffer, uint32_t x, uint32_t y, uint32_t color);

#endif
