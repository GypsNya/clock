#include <bsp_lcd.h>

//#include <bsp_fsmc.h>
//#include <bsp_led.h>

LTDC_HandleTypeDef ltdc_HandleStruct;

uint32_t get_lcd_id(void) {
	GPIO_InitTypeDef gpio_InitStruct;
	uint32_t idx;
	
	gpio_InitStruct.Pin = GPIO_PIN_6;
	gpio_InitStruct.Mode = GPIO_MODE_INPUT;
	gpio_InitStruct.Pull = GPIO_PULLUP;
	gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &gpio_InitStruct);
	
	gpio_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_7;
	HAL_GPIO_Init(GPIOI, &gpio_InitStruct);
	
	idx = (GPIOG -> IDR & GPIO_PIN_6) >> 6;
	idx |= ((GPIOI -> IDR & GPIO_PIN_2) >> 2) << 1;
	idx |= ((GPIOI -> IDR & GPIO_PIN_7) >> 7) << 2;
	
	return idx;
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc) {
	GPIO_InitTypeDef gpio_InitStruct;
	
	UNUSED(hltdc);

	gpio_clk_en();
	
	gpio_InitStruct.Pin = lcd_gpio_b;
	gpio_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_InitStruct.Pull = GPIO_PULLUP;
	gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &gpio_InitStruct);
	
	gpio_InitStruct.Pin = lcd_gpio_f;
	gpio_InitStruct.Mode = GPIO_MODE_AF_PP;
	gpio_InitStruct.Pull = GPIO_PULLUP;
	gpio_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_InitStruct.Alternate = lcd_af_mod;
	HAL_GPIO_Init(GPIOF, &gpio_InitStruct);
	
	gpio_InitStruct.Pin = lcd_gpio_g;
	HAL_GPIO_Init(GPIOG, &gpio_InitStruct);
	
	gpio_InitStruct.Pin = lcd_gpio_h;
	HAL_GPIO_Init(GPIOH, &gpio_InitStruct);
	
	gpio_InitStruct.Pin = lcd_gpio_i;
	HAL_GPIO_Init(GPIOI, &gpio_InitStruct);
}

HAL_StatusTypeDef lcd_init(void) {
	HAL_StatusTypeDef hal_result;
	
	dma2d_clk_en();
	lcd_clk_en();
	
	hal_result = lcd_clk_init();
	if (hal_result != HAL_OK) {
		return hal_result;
	}
	
	ltdc_HandleStruct.Instance = LTDC;
	ltdc_HandleStruct.Init.HorizontalSync = HSW - 1;
	ltdc_HandleStruct.Init.VerticalSync = VSW - 1;
	ltdc_HandleStruct.Init.AccumulatedHBP = HSW + HBP - 1;
	ltdc_HandleStruct.Init.AccumulatedVBP = VSW + VBP - 1;
	ltdc_HandleStruct.Init.AccumulatedActiveH = VSW + VBP + LCD_PIXEL_HEIGHT - 1;
	ltdc_HandleStruct.Init.AccumulatedActiveW = HSW + HBP + LCD_PIXEL_WIDTH - 1;
	ltdc_HandleStruct.Init.TotalWidth = HSW + HBP + LCD_PIXEL_WIDTH + HFP - 1;
	ltdc_HandleStruct.Init.TotalHeigh = VSW + VBP + LCD_PIXEL_HEIGHT + VFP - 1;
	
	ltdc_HandleStruct.LayerCfg->ImageWidth = LCD_PIXEL_WIDTH;
	ltdc_HandleStruct.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;
	
	ltdc_HandleStruct.Init.Backcolor.Red = 0;
	ltdc_HandleStruct.Init.Backcolor.Green = 0;
	ltdc_HandleStruct.Init.Backcolor.Blue = 0;
	
	ltdc_HandleStruct.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	ltdc_HandleStruct.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	ltdc_HandleStruct.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	ltdc_HandleStruct.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	
	hal_result = HAL_LTDC_Init(&ltdc_HandleStruct);
	if (hal_result != HAL_OK) {
		return hal_result;
	}
	
	hal_result = lcd_layer_init(LTDC_LAYER_1, lcd_layer1_buffer, PIXEL_FORMAT);
	if (hal_result != HAL_OK) {
		return hal_result;
	}
	
	hal_result = lcd_layer_init(LTDC_LAYER_2, lcd_layer2_buffer, PIXEL_FORMAT);
	if (hal_result != HAL_OK) {
		return hal_result;
	}
	lcd_on();
	return HAL_OK;
}

//this 1024*600 screen just support 25mhz.
//if use two layers it just support 10mhz.
HAL_StatusTypeDef lcd_clk_init(void) {
	RCC_PeriphCLKInitTypeDef rcc_PreiphCLKInitStruct;
	
	rcc_PreiphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	rcc_PreiphCLKInitStruct.PLLSAI.PLLSAIN = 88;
	rcc_PreiphCLKInitStruct.PLLSAI.PLLSAIR = 4;
	rcc_PreiphCLKInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;//
	return HAL_RCCEx_PeriphCLKConfig(&rcc_PreiphCLKInitStruct);
	/*HAL_StatusTypeDef hal_result = HAL_OK;
	int retry = 0;
	uint32_t pllsain = 360;
	uint32_t pllsair = 2;
	uint32_t pllsaidivr = 1;
	RCC -> CR &= ~(uint32_t)(1 << 28);
	while( (RCC -> CR & (1 << 29)) && (retry < 0x1FFF) ) retry++;	//wait sai clk unlock.
	if (retry == 0x1FFF) hal_result =  HAL_TIMEOUT;
	else {
		RCC -> PLLSAICFGR &= 0x8FFF803F;	//clear pllsain bits and pllsair bits.
		RCC -> PLLSAICFGR |= ((pllsain << 6) | (pllsair << 28));
		
		RCC -> DCKCFGR &= ~(uint32_t)(3 << 16);		//clear pllsaidivr bits.
		RCC -> DCKCFGR |= pllsaidivr << 16;
		
		RCC -> CR |= 1 << 28;
		//wait sai clk lock.
		retry = 0;
		while( (RCC -> CR & (1 << 29)) && (retry < 0x1FFF) ) retry++;
		if(retry == 0x1FFF) hal_result =  HAL_TIMEOUT;
	}
	
	return HAL_OK;*/
}

HAL_StatusTypeDef lcd_layer_init(uint32_t layer_index, uint32_t address, uint32_t pixel_format) {
	LTDC_LayerCfgTypeDef ltdc_LayerCfgStruct;
	
	ltdc_LayerCfgStruct.WindowX0 = 0;
	ltdc_LayerCfgStruct.WindowX1 = LCD_PIXEL_WIDTH;
	ltdc_LayerCfgStruct.WindowY0 = 0;
	ltdc_LayerCfgStruct.WindowY1 = LCD_PIXEL_HEIGHT;
	ltdc_LayerCfgStruct.PixelFormat = pixel_format;
	//ltdc_LayerCfgStruct.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
	ltdc_LayerCfgStruct.FBStartAdress = address;
	//ltdc_LayerCfgStruct.FBStartAdress = sdram_addr;
	ltdc_LayerCfgStruct.Alpha = 255;
	ltdc_LayerCfgStruct.Alpha0 = 0;
	ltdc_LayerCfgStruct.Backcolor.Red = 0;
	ltdc_LayerCfgStruct.Backcolor.Green = 0;
	ltdc_LayerCfgStruct.Backcolor.Blue = 0;
	ltdc_LayerCfgStruct.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	ltdc_LayerCfgStruct.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	ltdc_LayerCfgStruct.ImageWidth = LCD_PIXEL_WIDTH;
	ltdc_LayerCfgStruct.ImageHeight = LCD_PIXEL_HEIGHT;
	//HAL_LTDC_ConfigLayer(&ltdc_HandleStruct, &ltdc_LayerCfgStruct, LTDC_LAYER_1);
	return HAL_LTDC_ConfigLayer(&ltdc_HandleStruct, &ltdc_LayerCfgStruct, layer_index);
	
	//__HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&ltdc_HandleStruct);
}

void lcd_on(void) {
	__HAL_LTDC_ENABLE(&ltdc_HandleStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}



HAL_StatusTypeDef lcd_fill_rect(uint32_t layer_address, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	//UNUSED(layer_address);
	
	//stop dma2d.
	DMA2D -> CR &= ~(uint32_t)1;
	//set dma2d mode.
	DMA2D -> CR &= ~(uint32_t)(3 << 16);
	DMA2D -> CR |= DMA2D_R2M;
	//pixel mode.
	DMA2D -> OPFCCR = DMA2D_PIXEL_FORMAT;
	//calculate every line offset size for dma2d auto offset.
	//DMA2D -> OOR = LCD_PIXEL_WIDTH - (LCD_PIXEL_WIDTH);
	DMA2D -> OOR = LCD_PIXEL_WIDTH - width;
	//calculate rect start address in memory.
	DMA2D -> OMAR = layer_address + PIXEL_SIZE * (y * LCD_PIXEL_WIDTH + x);
	//set line number in low 16bit and each line pixel number in high 16bit.
	//DMA2D -> NLR = (LCD_PIXEL_WIDTH << 16) | LCD_PIXEL_HEIGHT;
	DMA2D -> NLR = (width << 16) | height;
	//set color register value for register to memory mode.
	DMA2D -> OCOLR = color;
	//start dma2d.
	DMA2D -> CR  |= 0x00000001;
	for (int i = 0; (DMA2D -> ISR & 0x00000002) == 0; i++) {
		if (i > 0xFFFFFF) {
			return HAL_TIMEOUT;
		}
	}
	DMA2D -> IFCR |= 0x00000002;
	return HAL_OK;
}

void lcd_draw_point(uint32_t layer_address, uint32_t x, uint32_t y, uint32_t color) {
	uint32_t addr = layer_address + PIXEL_SIZE * (y * LCD_PIXEL_WIDTH + x);
	
	VRAM[addr] = color;
}

void display_char(char* char_buffer, uint32_t size, uint32_t x, uint32_t y, uint32_t color) {
	//uint32_t layer_address = lcd_layer2_buffer;
	uint32_t char_code;
	//uint32_t char_bitmap;
	uint32_t addr = y * LCD_PIXEL_WIDTH + x;
	char* font_hub;
	char* font_en_hub;
	
	if (size == 32) {
		font_hub = FONT;
		font_en_hub = FONT_EN;
	} else if (size == 72) {
		//font_hub = FONT_72;
		//font_en_hub = FONT_EN_72;
	}
	
	char_code = char_code_pos(char_buffer, size);
	//char_code = (*char_buffer > 0x7f ? (((*char_buffer-0xa1)*94+*(char_buffer+1)-0xa1)*size*size/8):*char_buffer);
	if(char_code < 0x3F80) {
		//font_hub = FONT_EN;
		for(int row = 0; row < size/2*size; row++) {
			if(((font_en_hub[char_code] << (row % 8))&0x80) > 0) {
				VRAM2[addr] = color;
			}
			if(row%8 == 7) {
				char_code += 1;
			}
			if(row % (size/2) == size/2 - 1) {
				addr += LCD_PIXEL_WIDTH - size/2;
			}
			addr+=1;
		}
	} else {
		//font_hub = FONT;
		for(int row = 0; row < size*size; row++) {
			if(((font_hub[char_code] << (row % 8))&0x80) > 0) {
				VRAM2[addr] = color;
			}
			if(row%8 == 7) {
				char_code += 1;
			}
			if(row % size == size - 1) {
				addr += LCD_PIXEL_WIDTH - size;
			}
			addr+=1;
		}
	}
	/*for(int row = 0; row < size; row++) {
		for(int column = 0; column < size; column++) {
			if(((FONT[char_code] << (column % 8))&0x80) > 0) {
				*(uint32_t*)vram_addr = color;
			}
			if(column%8 == 8) {
				char_code += 1;
			}
			vram_addr+=1;
		}
		vram_addr += LCD_PIXEL_WIDTH - size;
	}*/
}

int display_ttf_char(TTF_TypeDef* ttf_struct, char* char_buffer, uint32_t x, uint32_t y, uint32_t color) {
	uint32_t pixel;
	uint32_t vram_addr = y * LCD_PIXEL_WIDTH + x;
	int result = 1;
	int height;int width;
	int str = 0;
	//uint16_t* ttf_cache = (uint16_t*)FONT_TTF_CACHE;

	if (is_chinese(char_buffer[0])) {
		str = (int)char_buffer[1] << 8 | (int)char_buffer[0];
	} else {
		str = (int)char_buffer[0];
	}
	
	ret_str_bitmap(ttf_struct, FONT_TTF_CACHE, str);
	height = ttf_struct->y_max - ttf_struct->y_min;
	width = ttf_struct->x_max - ttf_struct->x_min;
	vram_addr += ttf_struct->y_min*LCD_PIXEL_WIDTH + ttf_struct->x_min;
	
	for(int font_y = 0; font_y<height; font_y++) {
		for(int font_x = 0; font_x<width; font_x++) {
			pixel = FONT_TTF_CACHE[font_y * width + font_x];
			//pixel = (pixel & 0x1F << 16)|(pixel & 0x7E0 << 3)|(pixel & 0xF800 >> 11) | 0xFF000000;
			if (pixel == 0xff) {
				pixel = color;
			} else if (pixel > 0) {
				pixel = (uint32_t)(pixel << 24) | (color & 0x00FFFFFF);
			}
			VRAM2[vram_addr] = pixel;
			vram_addr += 1;
		}
		vram_addr = vram_addr+LCD_PIXEL_WIDTH-width;
	}
	return result;
}

LCD_StatusTypeDef printf_char(char* char_buffer, uint32_t x, uint32_t y, uint32_t color, uint32_t size) {
	//uint32_t char_size = 256;
	TTF_TypeDef ttf_struct;
	int result = 1;
	uint32_t i = 0;
	y = y+size-size/3;
	result = init_ttf(&ttf_struct, FONT_EN_TTF, size);
	if (result == 0) {
		return LCD_DISPLAY_ERR;
	}

	while(1) {
		if(is_str_end(char_buffer[i])) {
			return LCD_DISPLAY_OK;
		}
		
		//display_char(&char_buffer[i], char_size, x, y, color);
		display_ttf_char(&ttf_struct, &char_buffer[i], x, y, color);
		
		if(is_chinese(char_buffer[i])) {
			if((x + size) > LCD_PIXEL_WIDTH) {
				return LCD_DISPLAY_FLOW;
			}
			i += 2;
			x = x + size;
		} else {
			if((x + size/2) > LCD_PIXEL_WIDTH) {
				return LCD_DISPLAY_FLOW;
			}
			i++;
			x = x + size/2;
		}
	}
	return LCD_DISPLAY_OK;
}

LCD_StatusTypeDef printf_time(char* time_buffer, uint32_t x, uint32_t y, uint32_t color, uint32_t size) {
	//uint32_t char_size = 32;
	uint32_t time_index = 8;
	TTF_TypeDef ttf_struct;
	int result = 1;
	y = y+size-size/3;
	result = init_ttf(&ttf_struct, FONT_EN_TTF, size);
	if (result == 0) {
		return LCD_DISPLAY_ERR;
	}
	while(1) {
		for(int num = 0; num < 2; num++) {
			if((x + size/2) > LCD_PIXEL_WIDTH) {
				return LCD_DISPLAY_FLOW;
			}
			display_ttf_char(&ttf_struct, &time_buffer[time_index], x, y, color);
			time_index++;
			x = x + size/2;
		}
		if((x + size/2) > LCD_PIXEL_WIDTH) {
			return LCD_DISPLAY_FLOW;
		}
		// if(is_str_end(time_buffer[time_index])) {
		// 	return LCD_DISPLAY_OK;
		// }
		if(time_index == 12) {
			return LCD_DISPLAY_OK;
		}
		display_ttf_char(&ttf_struct, ":", x, y, color);
		x = x + size/2;
	}
	return LCD_DISPLAY_OK;
}

LCD_StatusTypeDef printf_date(char* time_buffer, uint32_t x, uint32_t y, uint32_t color) {
	char unit_char[6] = "������";
	uint32_t char_size = 32;
	uint32_t time_index = 0;
	
	for(uint32_t unit_index = 0; unit_index < 6; unit_index+=2) {
		for(int num = 0; num < 2; num++) {
			if((x + char_size/2) > LCD_PIXEL_WIDTH) {
				return LCD_DISPLAY_FLOW;
			}
			display_char(&time_buffer[time_index], char_size, x, y, color);
			time_index++;
			x = x + char_size/2;
		}
		if((x + char_size) > LCD_PIXEL_WIDTH) {
			return LCD_DISPLAY_FLOW;
		}
		display_char(&unit_char[unit_index], char_size, x, y, color);
	}
	return LCD_DISPLAY_OK;
}
LCD_StatusTypeDef printf_week(char* time_buffer, uint32_t x, uint32_t y, uint32_t color) {
	uint32_t char_size = 32;
	uint32_t time_index = 8;

	if(printf_char("����", x, y, color, 128) != LCD_DISPLAY_OK) {
		return LCD_DISPLAY_FLOW;
	}
	for(int num = 0; num < 2; num++) {
		if((x + char_size/2) > LCD_PIXEL_WIDTH) {
			return LCD_DISPLAY_FLOW;
		}
		display_char(&time_buffer[time_index], char_size, x, y, color);
		time_index++;
		x = x + char_size/2;
	}
	return LCD_DISPLAY_OK;
}
