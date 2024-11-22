#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "stdint.h"

struct KeyboardBuffer
{
    char buffer[500];
    int front;
    int end;
    int size;
};

char read_key_buffer(void);
void keyboard_handler(void);

#endif