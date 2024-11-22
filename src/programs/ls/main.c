#include "../../lib/lib.h"

struct DirEntry buffer[1024];

int main(void)
{

    int count;
    char name[10] = {0};

    count = read_root_directory(buffer);

    if (count != 0)
    {
        int _ = printf("NAME    \tDIR\tFILE SIZE\r\n");
        for (int i = 0; i < _; i++)
        {
            printf("-");
        }
        printf("\r\n");
        for (int i = 0; i < count; i++)
        {
            if (buffer[i].name[0] == ENTRY_EMPTY ||
                buffer[i].name[0] == ENTRY_DELETED ||
                buffer[i].attributes == 0xf ||
                buffer[i].attributes == 8)
            {
                continue;
            }

            memcpy(name, buffer[i].name, 8);
            if ((buffer[i].attributes & 0x10) == 0x10)
            {
                printf("%s\tYES\t%ukb", name, (uint64_t)buffer[i].file_size / 1024);
            }
            else
            {
                printf("%s\tNO\t%ukb", name, (uint64_t)buffer[i].file_size / 1024);
            }
            printf("\r\n");
        }
    }

    return 0;
}