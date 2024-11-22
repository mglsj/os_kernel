#include "../../lib/lib.h"

static char to_upper_case(const char c)
{
    if ('a' <= c && c <= 'z')
        return c & ~32;
    return c;
}

static int add_extension(char *buffer, int buffer_size)
{
    if (buffer[buffer_size - 4] == '.' &&
        buffer[buffer_size - 3] == 'B' &&
        buffer[buffer_size - 2] == 'I' &&
        buffer[buffer_size - 1] == 'N')
    {
        return buffer_size;
    }

    buffer[buffer_size++] = '.';
    buffer[buffer_size++] = 'B';
    buffer[buffer_size++] = 'I';
    buffer[buffer_size++] = 'N';
    return buffer_size;
}

static int read_cmd(char *buffer)
{
    char ch[4] = {0};
    int buffer_size = 0;

    while (1)
    {
        ch[0] = keyboard_read();
        if (ch[0] == '\r' || buffer_size > 80)
        {
            if (ch[0] == '\r')
            {
                ch[0] = '\r';
                ch[1] = '\n';
            }
            printf("%s", ch);
            ch[1] = 0;
            break;
        }
        else if (ch[0] == 127)
        {
            if (buffer_size > 0)
            {
                buffer[--buffer_size] = 0;
                ch[0] = '\b';
                ch[1] = ' ';
                ch[2] = '\b';
                printf("%s", ch);
                ch[1] = 0;
                ch[2] = 0;
            }
        }
        else
        {
            buffer[buffer_size++] = to_upper_case(ch[0]);
            printf("%s", ch);
        }
    }

    return buffer_size;
}

int main(void)
{

    char buffer[100] = {0};
    int buffer_size = 0;

    while (1)
    {
        printf("\033[1;32mshell\033[0m:\033[1;34m/\033[0m$ ");
        memset(buffer, 0, sizeof(buffer));
        buffer_size = read_cmd(buffer);
        if (buffer_size == 0)
        {
            continue;
        }
        buffer_size = add_extension(buffer, buffer_size);
        int fd = open_file(buffer);
        if (fd == -1)
        {
            printf("Command Not Found\r\n");
        }
        else
        {
            close_file(fd);
            int pid = fork();
            if (pid == 0)
            {
                exec(buffer);
            }
            else
            {
                waitu(pid);
            }
        }
    }

    return 0;
}