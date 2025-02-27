#ifndef __DIS_TTF_FONT_H
#define __DIS_TTF_FONT_H

#include <stdint.h>
#include <stb_truetype_htcw.h>

//typedef unsigned char uint8_t;
//typedef unsigned int uint32_t;

typedef struct {
	stbtt_fontinfo* info;
	
	uint8_t* font;		//ttf_file
	uint32_t font_size;

	float scale;		//垂直方向上的度量
	int ascent;	// ascent：字体从基线到顶部的高度；
	int descent;	// descent：基线到底部的高度，通常为负值；
	int lineGap;	// lineGap：两个字体之间的间距； // 行间距为：ascent - descent + lineGap。

	int x_min; int y_min; int x_max; int y_max;
	int y_board;
}TTF_TypeDef;

void init_ttf(TTF_TypeDef* ttf_struct, uint8_t* font, uint32_t font_size);
void ret_str_bitmap(TTF_TypeDef* ttf_struct, uint8_t* bitmap, char* str);

#endif
