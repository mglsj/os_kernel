#include "../gpu/common.h"
#include "../gpu/terminal.h"
#include "../gpu/video.h"
#include "images/Slide1.h"
#include "images/Slide2.h"
#include "images/Slide3.h"
#include "images/Slide4.h"

#define SLIDES 4

static u32 *slides[SLIDES] = {0};
static int current_slide = 0;

void init_presentation()
{
    slides[0] = (u32 *)Slide1;
    slides[1] = (u32 *)Slide2;
    slides[2] = (u32 *)Slide3;
    slides[3] = (u32 *)Slide4;
}

void presentation_show()
{
    terminal_initialized = false;
    video_draw_image(slides[current_slide], 1024 * 768 * 4);
}

void presentation_close()
{
    video_clear_screen();
    terminal_initialized = true;
}

void presentation_next()
{
    current_slide = (current_slide + 1) % SLIDES;
    presentation_show();
}
void presentation_prev()
{
    current_slide = (--current_slide) < 0 ? SLIDES - 1 : current_slide;
    presentation_show();
}