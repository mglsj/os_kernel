#include "../../lib/lib.h"

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
            buffer[buffer_size++] = ch[0];
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
        printf("\033[0;32mshell# \033[0m");
        memset(buffer, 0, sizeof(buffer));
        buffer_size = read_cmd(buffer);
        if (buffer_size == 0)
        {
            continue;
        }
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