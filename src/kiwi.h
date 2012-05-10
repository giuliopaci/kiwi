#ifndef _KIWI_H
#define _KIWI_H

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include "bstrlib.h"
typedef void Kw;
#include "list.h"
#include "stack.h"

#define KBYTE 1024
#define MBYTE ( KBYTE * 1024 )

#define PRESERVE_TEMPLATES 1

#define IMAGE_FRAME 1
#define IMAGE_THUMB ( 1 << 1 )
#define IMAGE_NOLINK ( 1 << 2 )
#define IMAGE_CUSTOMLINK ( 1 << 3 )
#define IMAGE_HAS_CAPTION ( 1 << 4 )
#define IMAGE_BORDER ( 1 << 5 )

#define TOC_NOTOC ( 1 << 1 )
#define TOC_FORCETOC ( 1 << 2 )
#define TOC_RELOC ( 1 << 3 )

Kw* kw_init(void);
void kw_cleanup(Kw* k);
int kw_low_level_parse(Kw* k);
void kw_parse(Kw* k);
void kw_set_base_url(Kw* k, char *str);
void kw_set_image_base_url(Kw* k, char *str);

bstring kw_get_output_buffer(Kw *k);
char *kw_get_output_buffer_cstr(Kw *k);
bstring kw_get_input_buffer(Kw* k);

int get_template_count(Kw* k);
void reset_template_iter(Kw* k);
struct node *get_next_template(Kw* k);

#endif
