#pragma once

#include "common.h"

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

void init_terminal(void);
void terminal_print(const char *buffer, uint32_t size);

extern bool terminal_initialized;
#endif