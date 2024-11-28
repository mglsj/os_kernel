#pragma once

#include "common.h"

void video_set_resolution(u32 xres, u32 yres, u32 bpp);
void video_draw_pixel(u32 x, u32 y, u32 color);
void video_write_buffer(u32 dst_offset, void *src, u32 size);
void video_dma();
void video_init();
void video_set_dma(bool b);
void video_clear_screen();
void video_set_bg(u32 bg_color);
