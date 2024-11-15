#include "../lib/lib.h"

int main(void)
{

    char *p = (char *)0xffff000000001000;
    *p = 'a';

    printf("User Process \r\n");
    sleepu(100);

    return 0;
}