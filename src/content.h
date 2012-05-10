
#ifndef __CONTENT_H
#define __CONTENT_H
#include "bstrlib.h"
#include <stdbool.h>
#include <stdint.h>
#include "kiwi.h"

// Number of entries on one line of the valid tag lookup table
#define HTML_MAX_TAG_ENTRIES 13
// Longest allowed tag length
#define HTML_MAX_LENGTH 11

void assemble_toc(struct node *item, bstring toc_buffer);
void insert_reloc_toc(Kw *k, bstring toc_buffer);
void handle_toc(Kw *k);
void open_tag(Kw *k, char *tag, char *args);
void close_tag(Kw *k, char *tag);
void append_to_tag_content(Kw* k, char *fmt, ...);
void init_tag_vars(Kw* k);
void repeat_append(bstring buffer, char chr, int count);
void remove_parentheticals(bstring str);
void strip_tags(bstring str);
void urlencode(bstring b);
void strip_html_markup(bstring str);
bool valid_html_tag(char *html_tag, size_t orig_len);
bool tag_self_closing(char *tag);
int validate_tag_attributes(Kw* k, struct node *item);
bool close_needed_tags(Kw* k);
uint64_t hash(char *str);
#endif
