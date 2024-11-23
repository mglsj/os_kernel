#include "video.h"
#include "colors.h"
#include "dma.h"
#include "mailbox.h"

extern char reserved_mem_start;

typedef struct
{
    mailbox_tag tag;
    u32 xres;
    u32 yres;
} mailbox_fb_size;

typedef struct
{
    mailbox_tag tag;
    u32 bpp;
} mailbox_fb_depth;

typedef struct
{
    mailbox_tag tag;
    u32 pitch;
} mailbox_fb_pitch;

typedef struct
{
    mailbox_tag tag;
    u32 base;
    u32 screen_size;
} mailbox_fb_buffer;

typedef struct
{
    mailbox_fb_size res;
    mailbox_fb_size vres; // virtual resolution..
    mailbox_fb_depth depth;
    mailbox_fb_buffer buff;
    mailbox_fb_pitch pitch;
} mailbox_fb_request;

static mailbox_fb_request fb_req;

static dma_channel *dma;
static u8 *vid_buffer;

static u32 *bg32_buffer;
static u32 *bg8_buffer;

#define MB (1024 * 1024)

// hack for not having an allocate function yet...
#define BG32_MEM_LOCATION ((&reserved_mem_start) + (10 * MB))
#define BG8_MEM_LOCATION (BG32_MEM_LOCATION + (10 * MB))
#define VB_MEM_LOCATION (BG8_MEM_LOCATION + (4 * MB))

void video_set_bg(u32 bg_color)
{
    for (int i = 0; i < (10 * MB) / 4; i++)
    {
        bg32_buffer[i] = bg_color;
    }

    for (int i = 0; i < (4 * MB) / 4; i++)
    {
        bg8_buffer[i] = 0x01010101;
    }
}

void video_init()
{
    dma = dma_open_channel(CT_NORMAL);
    vid_buffer = (u8 *)VB_MEM_LOCATION;

    // printk("DMA CHANNEL: %d\n", dma->channel);
    // printk("VID BUFF: %x\n", vid_buffer);

    bg32_buffer = (u32 *)BG32_MEM_LOCATION;
    bg8_buffer = (u32 *)BG8_MEM_LOCATION;

    video_set_bg(BACK_COLOR);
}

static bool use_dma = false;

#define BUS_ADDR(x) (((u64)x | 0x40000000) & ~0xC0000000)

#define FRAMEBUFFER ((volatile u8 *)P2V(BUS_ADDR(fb_req.buff.base)))
#define DMABUFFER ((volatile u8 *)vid_buffer)
#define DRAWBUFFER (use_dma ? DMABUFFER : FRAMEBUFFER)

void video_set_dma(bool b)
{
    use_dma = b;
}

void do_dma(void *dest, void *src, u32 total)
{

    u32 start = 0;

    while (total > 0)
    {
        int num_bytes = total;

        if (num_bytes > 0xFFFFFF)
        {
            num_bytes = 0xFFFFFF;
        }

        dma_setup_mem_copy(dma, dest + start, src + start, num_bytes, 2);

        dma_start(dma);

        dma_wait(dma);

        start += num_bytes;
        total -= num_bytes;
    }
}

void video_dma()
{
    do_dma((void *)FRAMEBUFFER, (void *)DMABUFFER, fb_req.buff.screen_size);
}

typedef struct
{
    mailbox_tag tag;
    u32 offset;
    u32 num_entries;
    u32 entries[8];
} mailbox_set_palette;

void video_set_resolution(u32 xres, u32 yres, u32 bpp)
{

    fb_req.res.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
    fb_req.res.tag.buffer_size = 8;
    fb_req.res.tag.value_length = 8;
    fb_req.res.xres = xres;
    fb_req.res.yres = yres;

    fb_req.vres.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
    fb_req.vres.tag.buffer_size = 8;
    fb_req.vres.tag.value_length = 8;
    fb_req.vres.xres = xres;
    fb_req.vres.yres = yres;

    fb_req.depth.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH;
    fb_req.depth.tag.buffer_size = 4;
    fb_req.depth.tag.value_length = 4;
    fb_req.depth.bpp = bpp;

    fb_req.buff.tag.id = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
    fb_req.buff.tag.buffer_size = 8;
    fb_req.buff.tag.value_length = 4;
    fb_req.buff.base = 16;
    fb_req.buff.screen_size = 0;

    fb_req.pitch.tag.id = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH;
    fb_req.pitch.tag.buffer_size = 4;
    fb_req.pitch.tag.value_length = 4;
    fb_req.pitch.pitch = 0;

    mailbox_set_palette palette;
    palette.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_PALETTE;
    palette.tag.buffer_size = 40;
    palette.tag.value_length = 0;
    palette.offset = 0;
    palette.num_entries = 8;
    palette.entries[0] = 0;
    palette.entries[1] = 0xFFBB5500;
    palette.entries[2] = 0xFFFFFFFF;
    palette.entries[3] = 0xFFFF0000;
    palette.entries[4] = 0xFF00FF00;
    palette.entries[5] = 0xFF0000FF;
    palette.entries[6] = 0x55555555;
    palette.entries[7] = 0xCCCCCCCC;

    // sets the actual resolution
    mailbox_process((mailbox_tag *)&fb_req, sizeof(fb_req));

    // printk("Allocated Buffer: %X - %d - %d\n", fb_req.buff.base, fb_req.buff.screen_size, fb_req.depth.bpp);

    if (bpp == 8)
    {
        mailbox_process((mailbox_tag *)&palette, sizeof(palette));
    }
    video_clear_screen();
    // draw some text showing what resolution is...
}

void video_clear_screen()
{
    if (!use_dma)
    {

        if (fb_req.depth.bpp == 32)
        {

            u32 *buff = (u32 *)FRAMEBUFFER;
            for (int i = 0; i < fb_req.buff.screen_size / 4; i++)
            {
                buff[i] = bg32_buffer[i];
            }
        }
        else if (fb_req.depth.bpp == 8)
        {

            u32 *buff = (u32 *)FRAMEBUFFER;
            for (int i = 0; i < fb_req.buff.screen_size / 4; i++)
            {
                buff[i] = bg8_buffer[i];
            }
        }
    }
    else
    {
        if (fb_req.depth.bpp == 32)
            do_dma((void *)BUS_ADDR(vid_buffer), bg32_buffer, fb_req.buff.screen_size);
        else if (fb_req.depth.bpp == 8)
            do_dma((void *)BUS_ADDR(vid_buffer), bg8_buffer, fb_req.buff.screen_size);
        video_dma();
    }
}

void video_draw_pixel(u32 x, u32 y, u32 color)
{

    u32 pixel_offset = (x * (fb_req.depth.bpp >> 3)) + (y * fb_req.pitch.pitch);

    if (fb_req.depth.bpp == 32)
    {
        u32 *buff = (u32 *)DRAWBUFFER;
        buff[pixel_offset / 4] = color;
    }
    else if (fb_req.depth.bpp == 16)
    {
        u16 *buff = (u16 *)DRAWBUFFER;
        buff[pixel_offset / 2] = color & 0xFFFF;
    }
    else
    {
        DRAWBUFFER[pixel_offset++] = (color & 0xFF);
    }
}

void video_draw_char(char c, u32 pos_x, u32 pos_y)
{
    u32 text_color = TEXT_COLOR;
    u32 back_color = BACK_COLOR;

    if (fb_req.depth.bpp == 8)
    {
        text_color = 2;
        back_color = 1;
    }

    for (int y = 0; y < font_get_height(); y++)
    {
        for (int x = 0; x < font_get_width(); x++)
        {
            bool yes = font_get_pixel(c, x, y); // gets whether there is a pixel for the font at this pos...
            video_draw_pixel(pos_x + x, pos_y + y, yes ? text_color : back_color);
        }
    }
}

void video_draw_string(char *s, u32 pos_x, u32 pos_y)
{
    for (int i = 0; s[i] != 0; pos_x += (font_get_width() + 2), i++)
    {
        video_draw_char(s[i], pos_x, pos_y);
    }
}
