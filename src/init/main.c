#include "../lib/lib.h"

int main(void)
{
    printf("Init program\r\n");
    int fd;
    int size;
    char buffer[100];

    printf("Opening file\r\n");
    fd = open_file("TEST.BIN");
    if (fd == -1)
    {
        printf("open file failed\r\n");
    }
    else
    {
        printf("File opened\r\n");

        printf("Getting file size\r\n");
        size = get_file_size(fd);
        printf("Size: %d\r\n", (int64_t)size);

        printf("Reading file\r\n");
        size = read_file(fd, buffer, size);
        buffer[size] = '\0';

        printf("%s\r\n", buffer);
        printf("Read %dbytes in total \r\n", (int64_t)size);
    }

    close_file(fd);

    return 0;
}