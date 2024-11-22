#include "keyboard.h"
#include "print.h"
#include "process.h"
#include "uart.h"

static struct KeyboardBuffer key_buffer = {{0}, 0, 0, 500};

static void write_key_buffer(char ch)
{
    int front = key_buffer.front;
    int end = key_buffer.end;
    int size = key_buffer.size;

    if ((end + 1) % size == front)
    {
        return;
    }
    key_buffer.buffer[end++] = ch;
    key_buffer.end = end % size;
}

char read_key_buffer(void)
{
    int front = key_buffer.front;

    if (front == key_buffer.end)
    {
        sleep(-2);
    }

    key_buffer.front = (key_buffer.front + 1) % key_buffer.size;
    return key_buffer.buffer[front];
}

static char keyboard_read(void)
{
    return read_char();
}

void keyboard_handler(void)
{
    char ch = keyboard_read();

    write_key_buffer(ch);
    wake_up(-2);
}
