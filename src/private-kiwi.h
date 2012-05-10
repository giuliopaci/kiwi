#include <stdlib.h>

typedef struct _kw
{
	int current_header_level;
	int current_bullet_list_level;
	int current_numbered_list_level;
	int current_definition_list_level;

	int start_of_line;
	char protocol[5];

// Images
	int image_attributes;
	bstring image_url;
	bstring image_variables;
	bstring image_link_url;
	bstring image_caption;

// Links
	bstring link_path;
	bstring link_text;

// HTML tags
	bstring tag_name;
	bstring tag_attribute;
	bstring tag_attributes_validated;
	struct list tag_attributes_list;
	stack tag_stack;

// Tables
	int tr_found;

// ToC
	int toc_attributes;
	struct list toc_list;

// Wikitext
	int in_tag;
	bstring tag_content;
	unsigned int tag_content_size;

// Templates
	struct list template_list;
	struct node *template_list_iter;
	int template_noinclude;

// General
	bstring output_buffer;
	bstring input_buffer;
	long input_buffer_pos;
	bstring base_url;
	bstring image_base_url;

// GREG Parser
	void* g;
} _kw_t;
