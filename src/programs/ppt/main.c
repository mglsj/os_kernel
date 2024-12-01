#include "../../lib/lib.h"

int main(void)
{
    presentation_show();

    while (1)
    {
        char ch = keyboard_read();

        switch (ch)
        {
        case 'd':
            presentation_next();
            break;
        case 'a':
            presentation_prev();
            break;
        case 'c':
            presentation_close();
            return 0;
        default:
            break;
        }
    }
    return 0;
}