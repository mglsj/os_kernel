#pragma once

#include "common.h"

void video_set_resolution(u32 xres, u32 yres, u32 bpp);
void video_draw_char(char c, u32 pox_x, u32 pos_y);
void video_draw_string(char *s, u32 pos_x, u32 pos_y);
void video_draw_pixel(u32 x, u32 y, u32 color);

void video_dma();
void video_init();
void video_set_dma(bool b);
void video_clear_screen();
void video_set_bg(u32 bg_color);

// comes from fontData.c
u32 font_get_height();
u32 font_get_width();
bool font_get_pixel(char ch, u32 x, u32 y);
