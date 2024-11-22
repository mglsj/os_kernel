#include "../../lib/lib.h"

int main(void)
{
    int pid;

    pid = fork();

    if (pid == 0)
    {
        exec("CONSOLE.BIN");
    }
    else
    {
        waitu(pid);
    }
    return 0;
}