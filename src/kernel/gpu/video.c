#include "video.h"
#include "dma.h"
#include "mailbox.h"
#include "mem.h"

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
static u32 *bg32_buffer;
static u32 *bg8_buffer;
static u8 *vid_buffer;

static bool use_dma = false;

#define BUS_ADDR(x) (((u64)x | 0x40000000) & ~0xC0000000)

#define FRAMEBUFFER ((volatile u8 *)P2V(BUS_ADDR(fb_req.buff.base)))
#define DMABUFFER ((volatile u8 *)vid_buffer)
#define DRAWBUFFER (use_dma ? DMABUFFER : FRAMEBUFFER)

void video_set_bg(u32 bg_color)
{
    for (int i = 0; i < (10 * MB) / 4; i++)
    {
        bg32_buffer[i] = bg_color;
    }

    // for (int i = 0; i < (4 * MB) / 4; i++)
    // {
    //     bg8_buffer[i] = 0x01010101;
    // }
    memset(bg8_buffer, bg_color & 0xFF, 4 * MB);
}

void video_init()
{
    dma = dma_open_channel(CT_NORMAL);
    vid_buffer = (u8 *)VB_MEM_LOCATION;
    bg32_buffer = (u32 *)BG32_MEM_LOCATION;
    bg8_buffer = (u32 *)BG8_MEM_LOCATION;
}

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
    if (use_dma)
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

void video_write_buffer(u32 dst_offset, void *src, u32 size)
{
    size = min(size, fb_req.buff.screen_size);

    if (use_dma)
    {
        do_dma((void *)BUS_ADDR(vid_buffer) + dst_offset, src, size);
        video_dma();
    }
    else
    {
        memcpy((void *)FRAMEBUFFER + dst_offset, src, size);
    }
}

void video_clear_screen()
{
    if (fb_req.depth.bpp == 32)
    {
        video_write_buffer(0, bg32_buffer, fb_req.buff.screen_size);
    }
    else if (fb_req.depth.bpp == 8)
    {
        video_write_buffer(0, bg8_buffer, fb_req.buff.screen_size);
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
    // else if (fb_req.depth.bpp == 16)
    // {
    //     u16 *buff = (u16 *)DRAWBUFFER;
    //     buff[pixel_offset / 2] = color & 0xFFFF;
    // }
    else if (fb_req.depth.bpp == 8)
    {
        // DRAWBUFFER[pixel_offset++] = (color & 0xFF);
        DRAWBUFFER[pixel_offset] = (color & 0xFF);
    }
}