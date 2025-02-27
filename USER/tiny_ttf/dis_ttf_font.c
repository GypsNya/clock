#include "dis_ttf_font.h"
#include <stdlib.h>
//stbtt_fontinfo info;
//TTF_TypeDef ttf_struct;

__attribute__((weak,noreturn))
void __aeabi_assert (const char *expr, const char *file, int line) {
	abort();
}

void init_ttf(TTF_TypeDef* ttf_struct, uint8_t* font, uint32_t font_size) {
	ttf_struct->font = font;
	ttf_struct->font_size = font_size;
	stbtt_InitFont(ttf_struct->info, (uint8_t*)font, 0);
	ttf_struct->scale = stbtt_ScaleForPixelHeight(ttf_struct->info, font_size);
	stbtt_GetFontVMetrics(ttf_struct->info, &ttf_struct->ascent, &ttf_struct->descent, &ttf_struct->lineGap);
	ttf_struct->ascent = (int)(ttf_struct->ascent * ttf_struct->scale);
	ttf_struct->descent = (int)(ttf_struct->descent * ttf_struct->scale);
}

void ret_str_bitmap(TTF_TypeDef* ttf_struct, uint8_t* bitmap, char* str) {
	int advance_width;
	int left_side_bearing;
	int x_min; int y_min; int x_max; int y_max;
	int y_board;
	int code_point = stbtt_FindGlyphIndex(ttf_struct->info, (int)str);
	stbtt_GetCodepointHMetrics(ttf_struct->info, code_point, &advance_width, &left_side_bearing);
	stbtt_GetCodepointBitmapBox(ttf_struct->info, code_point, ttf_struct->scale, ttf_struct->scale, &ttf_struct->x_min, &ttf_struct->y_min, &ttf_struct->x_max, &ttf_struct->y_max);
	y_board = y_min + ttf_struct->scale + y_max;
	stbtt_MakeCodepointBitmap(ttf_struct->info, (uint8_t*)bitmap, ttf_struct->x_max-ttf_struct->x_min, ttf_struct->y_max-ttf_struct->y_min, ttf_struct->x_max-ttf_struct->x_min, ttf_struct->scale, ttf_struct->scale, code_point);
}
