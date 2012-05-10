#ifndef __IO_H
#define __IO_H

#include "kiwi.h"

void bprintf(Kw* k, const char *fmt, ...);
void handle_input(Kw* k, char *buf, int *result, size_t max_size);
void file_get_contents(bstring buffer, char *filename);
void stdin_get_contents(bstring buffer);
void str_get_contents(Kw* k, const char *str);
#endif
