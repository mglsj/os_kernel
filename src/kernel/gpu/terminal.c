#include "terminal.h"
#include "../print.h"
#include "colors.h"
#include "dma.h"
#include "fonts.h"
#include "mem.h"
#include "video.h"

bool terminal_initialized = false;

u32 terminal_background_color = SCREEN_BACKGROUND;
u32 terminal_text_color = SCREEN_FOREGROUND;

#define TAB_WIDTH 8

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define COLOR_BITS 32
#define FRAME_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * (COLOR_BITS >> 3))

#define PADD_Y 4
#define PADD_X 2

#define OFFSET_TOP (PADD_Y + 2)
#define OFFSET_BOTTOM (PADD_Y - 2)

#define OFFSET_LEFT (PADD_X + 1)
#define OFFSET_RIGHT (PADD_X - 1)

static u32 char_height; // 16
static u32 char_width;  // 8

#define LINE_HEIGHT 20
#define COLUMN_WIDTH 10

#define LINES_PER_SCREEN ((SCREEN_HEIGHT - 2 * PADD_Y) / LINE_HEIGHT)
#define COLUMNS_PER_LINE ((SCREEN_WIDTH - 2 * PADD_X) / COLUMN_WIDTH)

static u32 *terminal_pixel_buffer;

#define TEXT_BUFFER_SIZE (4 * 1024)
static struct
{
    uint32_t start;
    uint32_t end;
    uint32_t cursor;
    char buffer[TEXT_BUFFER_SIZE]; // 4 KB
} text_buffer = {0, 0, 0, {0}};
static void text_buffer_putc(const char c);
static char text_buffer_getc();

static u32 get_top_offest_bytes()
{
    return OFFSET_TOP * SCREEN_WIDTH * (COLOR_BITS >> 3);
}

static u32 get_bottom_offest_bytes()
{
    return OFFSET_BOTTOM * SCREEN_WIDTH * (COLOR_BITS >> 3);
}

static void write_pixel_buffer(void *buffer_start, u32 size)
{
    u32 top_offest_bytes = get_top_offest_bytes();

    video_write_buffer(
        top_offest_bytes,
        max((void *)terminal_pixel_buffer, (void *)buffer_start),
        size);
}

static void draw_pixel(u32 x, u32 y, u32 color)
{
    u32 pixel_offset = (y * SCREEN_WIDTH + x) * (COLOR_BITS >> 3);

    if (COLOR_BITS == 32)
    {
        terminal_pixel_buffer[(pixel_offset / 4)] = color;
    }
    else if (COLOR_BITS == 8)
    {
        ((u8 *)terminal_pixel_buffer)[pixel_offset] = (color & 0xFF);
    }
}

static void draw_char(char c, u32 pos_x, u32 pos_y, u32 text_color, u32 background_color)
{
    if (COLOR_BITS == 8)
    {
        text_color = text_color & 0xFF;
        background_color = background_color & 0xFF;
    }

    for (int y = 0; y < char_height; y++)
    {
        for (int x = 0; x < char_width; x++)
        {
            bool yes = font_get_pixel(c, x, y); // gets whether there is a pixel for the font at this pos...
            draw_pixel(
                pos_x + x,
                pos_y + y,
                yes ? text_color : background_color);
        }
    }
}

static void clean_line(int y)
{
    for (int column = 0, x = OFFSET_LEFT; column < COLUMNS_PER_LINE; column++, x += COLUMN_WIDTH)
        draw_char(
            ' ', x, y,
            terminal_background_color,
            terminal_background_color);
}

static u32 draw_pixel_buffer()
{
    text_buffer.cursor = text_buffer.start;

    u32 text_color = terminal_text_color;
    u32 bg_color = terminal_background_color;

    bool ansi_escape_sequence = false;
    bool ansi_control_sequence = false;
    int ansi_control_code = -1;

    uint32_t lines_written = 0;
    for (int y = 0; lines_written < 512; lines_written++, y += LINE_HEIGHT)
    {
        clean_line(y);
        for (int column = 0, x = OFFSET_LEFT; column < COLUMNS_PER_LINE;)
        {
            char ch = text_buffer_getc();
            switch (ch)
            {
            case '\0':
                return column == 0 ? lines_written : lines_written + 1;
                break;
            case '\r':
                column = 0;
                x = OFFSET_LEFT;
                break;
            case '\n':
                column = COLUMNS_PER_LINE;
                break;
            case '\b':
                column--;
                x -= COLUMN_WIDTH;
                break;
            case '\t':
            {
                int factor = (column % TAB_WIDTH) ? column % TAB_WIDTH : TAB_WIDTH;
                column += factor;
                x += factor * COLUMN_WIDTH;
            }
            break;
            case '\033':
                ansi_escape_sequence = true;
                break;
            default:
                if (ansi_escape_sequence == true)
                {
                    if (ansi_control_sequence == true)
                    {
                        if ('0' <= ch && ch <= '9')
                        {
                            if (ansi_control_code == -1)
                                ansi_control_code = ch - '0';
                            else
                                ansi_control_code = ansi_control_code * 10 + ch - '0';
                        }
                        if (ch == 'm' || ch == ';')
                        {
                            switch (ansi_control_code)
                            {
                            case -1:
                                break;
                            case 0:
                                text_color = terminal_text_color;
                                bg_color = terminal_background_color;
                                break;
                            case 30:
                                text_color = DARK_BLACK;
                                break;
                            case 31:
                                text_color = DARK_RED;
                                break;
                            case 32:
                                text_color = DARK_GREEN;
                                break;
                            case 33:
                                text_color = DARK_YELLOW;
                                break;
                            case 34:
                                text_color = DARK_BLUE;
                                break;
                            case 35:
                                text_color = DARK_MAGENTA;
                                break;
                            case 36:
                                text_color = DARK_CYAN;
                                break;
                            case 37:
                                text_color = DARK_WHITE;
                                break;
                            case 90:
                                text_color = BRIGHT_BLACK;
                                break;
                            case 91:
                                text_color = BRIGHT_RED;
                                break;
                            case 92:
                                text_color = BRIGHT_GREEN;
                                break;
                            case 93:
                                text_color = BRIGHT_YELLOW;
                                break;
                            case 94:
                                text_color = BRIGHT_BLUE;
                                break;
                            case 95:
                                text_color = BRIGHT_MAGENTA;
                                break;
                            case 96:
                                text_color = BRIGHT_CYAN;
                                break;
                            case 97:
                                text_color = BRIGHT_WHITE;
                                break;
                            default:
                                break;
                            }
                            ansi_control_code = -1;
                        }
                        if (ch == 'm')
                        {
                            ansi_control_sequence = false;
                            ansi_escape_sequence = false;
                        }
                    }
                    if (ch == '[')
                    {
                        ansi_control_sequence = true;
                        ansi_control_code = -1;
                    }
                }
                else if ((0x20 <= ch && ch <= 0x7F) || (0xA0 <= ch && ch <= 0xFF))
                {
                    draw_char(
                        ch, x, y,
                        text_color,
                        bg_color);
                    column++;
                    x += COLUMN_WIDTH;
                }
            }
        }
    }

    return lines_written;
}

void terminal_draw()
{
    u32 lines_written = draw_pixel_buffer();

    u32 bytes_per_line = SCREEN_WIDTH * LINE_HEIGHT * (COLOR_BITS >> 3);
    u32 bytes_written = lines_written * bytes_per_line;
    u32 max_bytes = FRAME_BUFFER_SIZE - get_top_offest_bytes() - get_bottom_offest_bytes();

    if (bytes_written <= max_bytes)
    {
        write_pixel_buffer(
            (void *)terminal_pixel_buffer,
            bytes_written);
    }
    else
    {
        write_pixel_buffer(
            (void *)terminal_pixel_buffer + (bytes_written - max_bytes),
            max_bytes);
    }
}

void terminal_set_bg(uint32_t color)
{
    terminal_background_color = color;
    video_set_bg(color);

    if (COLOR_BITS == 32)
    {
        for (int i = 0; i < TERMINAL_BUFFER_SIZE / 4; i++)
        {
            terminal_pixel_buffer[i] = color;
        }
    }
    else
    {
        memset((void *)terminal_pixel_buffer, color & 0xFF, TERMINAL_BUFFER_SIZE);
    }
}

void terminal_clear()
{
    video_clear_screen();
}

static void text_buffer_putc(const char c)
{
    text_buffer.buffer[text_buffer.end] = c;
    text_buffer.end = (text_buffer.end + 1) % TEXT_BUFFER_SIZE;
    if (text_buffer.end == text_buffer.start)
    {
        text_buffer.start = (text_buffer.start + 1) % TEXT_BUFFER_SIZE;
    }
}

static char text_buffer_getc()
{
    if (text_buffer.cursor == text_buffer.end)
        return '\0';

    char c = text_buffer.buffer[text_buffer.cursor];
    text_buffer.cursor = (text_buffer.cursor + 1) % TEXT_BUFFER_SIZE;
    return c;
}

void terminal_print(const char *buffer, uint32_t size)
{
    for (int i = 0; i < size; i++)
    {
        text_buffer_putc(buffer[i]);
    }
    terminal_draw();

    // DO NOT REMOVE
    write_pixel_buffer(0, 0);
}

void init_terminal(void)
{
    terminal_pixel_buffer = (u32 *)TERMINAL_BUFFER_LOCATION;

    char_height = font_get_height();
    char_width = font_get_width();

    video_init();
    video_set_dma(true);
    terminal_set_bg(terminal_background_color);
    video_set_resolution(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BITS);

    terminal_print("Hello from \033[1;31mRaspberry\033[0m \033[1;32mpi\033[0m\r\n\r\n", 49);

    terminal_initialized = true;
}
