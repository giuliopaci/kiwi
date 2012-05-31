#include "bstrlib.h"
#include "kiwi.h"
#include "content.h"
#include "list.h"
#include "io.h"
#include <inttypes.h>
#include "parse.h"
#include "stack.h"
#include "private-kiwi.h"

bstring kw_get_input_buffer(Kw* k) {
  return ((_kw_t*)k)->input_buffer;
}

bstring kw_get_output_buffer(Kw* k) {
  return ((_kw_t*)k)->output_buffer;
}

char *kw_get_output_buffer_cstr(Kw* k) {
  return bdata(((_kw_t*)k)->output_buffer);
}

void kw_set_base_url(Kw* k, char *str) {
  btrunc(((_kw_t*)k)->base_url, 0);
  bcatcstr(((_kw_t*)k)->base_url, str);
}

void kw_set_image_base_url(Kw* k, char *str) {
  btrunc(((_kw_t*)k)->image_base_url, 0);
  bcatcstr(((_kw_t*)k)->image_base_url, str);
}

int get_template_count(Kw* k) {
  return ((_kw_t*)k)->template_list.size;
}

void reset_template_iter(Kw* k) {
  ((_kw_t*)k)->template_list_iter = ((_kw_t*)k)->template_list.head;
}

struct node *get_next_template(Kw* k) {
	if(!((_kw_t*)k)->template_list_iter) {
		((_kw_t*)k)->template_list_iter = ((_kw_t*)k)->template_list.head;
  }

  if(((_kw_t*)k)->template_list_iter->next) {
    ((_kw_t*)k)->template_list_iter = ((_kw_t*)k)->template_list_iter->next;
    return ((_kw_t*)k)->template_list_iter;
  }

  return NULL;
}

KIWI_ACTION(heading_action_1) {
  char tag[4];
  bprintf(k, "\n  ");
  sprintf(tag, "h%d", ((_kw_t*)k)->current_header_level);
  open_tag(k, tag, NULL);
}

KIWI_ACTION(heading_action_2) {
  brtrimws(((_kw_t*)k)->tag_content);
  bstring tmp = bstrcpy(((_kw_t*)k)->tag_content);
  strip_html_markup(tmp);
  urlencode(tmp);
  bprintf(k, "<span class=\"editsection\">[<a href=\"edit\">edit</a>]</span><span class=\"mw-headline\" id=\"%s\">", bdata(tmp));
  bdestroy(tmp);
  
  bprintf(k, "%s</span>", bdata(((_kw_t*)k)->tag_content));
  char tag[4];
  sprintf(tag, "h%d", ((_kw_t*)k)->current_header_level);
  close_tag(k, tag);
  bprintf(k, "\n");
  
  struct node *current = kw_list_append_new(&((_kw_t*)k)->toc_list);
  
  tmp = bstrcpy(((_kw_t*)k)->tag_content);
  strip_html_markup(tmp);
  urlencode(tmp);
  bprintf(k, "  <a name=\"%s\"></a>\n", bdata(tmp));
  bassign(current->name, tmp);
  
  bstring human_name = bstrcpy(((_kw_t*)k)->tag_content);
  strip_tags(human_name);
  brtrimws(human_name);
  bassign(current->content, human_name);
  current->level = ((_kw_t*)k)->current_header_level;
  
  bdestroy(tmp);
  bdestroy(human_name);
  
  btrunc(((_kw_t*)k)->tag_content, 0);
  ((_kw_t*)k)->current_header_level = 0;
}

static int strfirstdiff(const char* s1, const char* s2)
{
	const char* base = s1;
	while( ( *s1 == *s2 ) && ( *s1 != 0 ) )
	{
		s1++;
		s2++;
	}
	if( *s1 != *s2  )
	{
		return (int)(s1-base);
	}
	return -1;
}

KIWI_ACTION(mixed_list_action_1) {
		int i;
		const char* tmp;
		tmp = bdata(((_kw_t*)k)->current_mixed_list_level);
		for(i=blength(((_kw_t*)k)->current_mixed_list_level)-1; i >= 0; i--)
		{
			switch(tmp[i])
			{
			case '#':
				bprintf(k, "</li></ol>");
			break;
			case '*':
				bprintf(k, "</li></ul>");
			break;
			case ':':
				bprintf(k, "</dd></dl>");
			break;
			case ';':
				bprintf(k, "</dt></dl>");
			break;
			}
		}
  bassigncstr(((_kw_t*)k)->current_mixed_list_level, "");
}

KIWI_ACTION(mixed_bullet_action_1) {
	int idx;
	int old_len;
	old_len = blength(((_kw_t*)k)->current_mixed_list_level);
	idx = strfirstdiff(bdata(((_kw_t*)k)->current_mixed_list_level), text);
	if( idx < 0 )
	{
		switch(text[old_len-1])
		{
		case '#':
		case '*':
			bprintf(k, "</li><li>");
			break;
		case ':':
			bprintf(k, "</dd><dd>");
			break;
		case ';':
			bprintf(k, "</dt><dt>");
			break;
		}
	}
	else
	{
		int i;
		const char* tmp = bdata(((_kw_t*)k)->current_mixed_list_level);
		for(i=old_len-1; i >= idx; i--)
		{
			switch(tmp[i])
			{
			case '#':
				bprintf(k, "</li></ol>");
			break;
			case '*':
				bprintf(k, "</li></ul>");
			break;
			case ':':
				bprintf(k, "</dd></dl>");
			break;
			case ';':
				bprintf(k, "</dt></dl>");
			break;
			}
		}
		old_len = strlen(text);
		if( idx == old_len )
		{
			switch(text[old_len-1])
			{
			case '#':
			case '*':
				bprintf(k, "</li><li>");
				break;
			case ':':
				bprintf(k, "</dd><dd>");
				break;
			case ';':
				bprintf(k, "</dt><dt>");
				break;
			}
		}
		i++;
		for(; i < old_len; i++)
		{
			switch(text[i])
			{
			case '#':
				bprintf(k, "<ol><li>");
			break;
			case '*':
				bprintf(k, "<ul><li>");
			break;
			case ':':
				bprintf(k, "<dl><dd>");
			break;
			case ';':
				bprintf(k, "<dl><dt>");
			break;
			}
		}
	}
  bassigncstr(((_kw_t*)k)->current_mixed_list_level, text);
}

KIWI_ACTION(nowiki_action_1) {
  bstring markup = bfromcstr(text);
  strip_tags(markup);
  append_to_tag_content(k, "%s", bdata(markup));
  bdestroy(markup);
}

KIWI_ACTION(comment_action_1) {
  bstring markup = bfromcstr(text);
  strip_tags(markup);
  //append_to_tag_content(k, "%s", bdata(markup));
  bdestroy(markup);
}

KIWI_ACTION(local_link_action_1) {
  strip_html_markup(((_kw_t*)k)->link_path);
  urlencode(((_kw_t*)k)->link_path);
  remove_parentheticals(((_kw_t*)k)->link_text);
  btrimws(((_kw_t*)k)->link_text);
  append_to_tag_content(k, "<a href=\"%s%s\">%s</a>", bdata(((_kw_t*)k)->base_url), bdata(((_kw_t*)k)->link_path), bdata(((_kw_t*)k)->link_text));
  btrunc(((_kw_t*)k)->link_path, 0);
  btrunc(((_kw_t*)k)->link_text, 0);
}

KIWI_ACTION(image_action_1) {
  if(((_kw_t*)k)->image_attributes & IMAGE_FRAME) {
    bprintf(k, "<div class=\"thumb tright\"><div class=\"thumbinner\">");
  }

  if(!(((_kw_t*)k)->image_attributes & IMAGE_NOLINK)) {
    if(((_kw_t*)k)->image_attributes & IMAGE_CUSTOMLINK)
      bprintf(k, "<a href=\"%s\" class=\"image\">", bdata(((_kw_t*)k)->image_link_url));
    else
      if(((_kw_t*)k)->image_attributes & IMAGE_HAS_CAPTION)
        bprintf(k, "<a href=\"/File:%s\" class=\"image\" title=\"%s\">", bdata(((_kw_t*)k)->image_url), bdata(((_kw_t*)k)->image_caption));
      else
        bprintf(k, "<a href=\"/File:%s\" class=\"image\">", bdata(((_kw_t*)k)->image_url));
  }

  if(((_kw_t*)k)->image_attributes & IMAGE_THUMB) {
    bprintf(k, "<img src=\"%s%s\" class=\"thumbimage\"/>", bdata(((_kw_t*)k)->image_base_url), bdata(((_kw_t*)k)->image_url));
  } else if((((_kw_t*)k)->image_attributes & IMAGE_FRAME) || (((_kw_t*)k)->image_attributes & IMAGE_BORDER)) {
    bprintf(k, "<img src=\"%s%s\" class=\"thumbimage\"/>", bdata(((_kw_t*)k)->image_base_url), bdata(((_kw_t*)k)->image_url));
  } else {
    bprintf(k, "<img src=\"%s%s\" %s/>", bdata(((_kw_t*)k)->image_base_url), bdata(((_kw_t*)k)->image_url), bdata(((_kw_t*)k)->image_variables));
  }
  if(!(((_kw_t*)k)->image_attributes & IMAGE_NOLINK)) {
    bprintf(k, "</a>");
  }

  if(((_kw_t*)k)->image_attributes & IMAGE_FRAME) {
    bprintf(k, "<div class=\"thumbcaption\">%s</div></div>", bdata(((_kw_t*)k)->image_caption));
  }
}

KIWI_ACTION(image_link_action_1) {
  if(text_length == 0) {
    ((_kw_t*)k)->image_attributes |= IMAGE_NOLINK;
  } else {
    ((_kw_t*)k)->image_attributes |= IMAGE_CUSTOMLINK;
    bassignformat(((_kw_t*)k)->image_link_url, "%s", text);
  }
}

KIWI_ACTION(image_caption_action_1) {
  ((_kw_t*)k)->image_attributes |= IMAGE_HAS_CAPTION;
  bassignformat(((_kw_t*)k)->image_caption, "%s", text);
}

KIWI_ACTION(table_open_action_1) {
  init_tag_vars(k);
  bcatcstr(((_kw_t*)k)->tag_name, "table");
  ((_kw_t*)k)->tr_found = 0; 
  bprintf(k, "<table");
}

KIWI_ACTION(table_open_action_2) {
  // Simpler to check the last output character than to hack up the LEG rules
  if(bdata(((_kw_t*)k)->output_buffer)[((_kw_t*)k)->output_buffer->slen - 1] != '>') {
    bprintf(k, ">"); 
  }
}

KIWI_ACTION(table_caption_action_1) {
  init_tag_vars(k);
  bcatcstr(((_kw_t*)k)->tag_name, "caption");
  bprintf(k, "<caption"); 
}

KIWI_ACTION(table_caption_action_2) {
  // Simpler to check the last output character than to hack up the LEG rules
  if(bdata(((_kw_t*)k)->output_buffer)[((_kw_t*)k)->output_buffer->slen - 1] != '>') {
    bprintf(k, ">"); 
  }
}

KIWI_ACTION(table_caption_action_3) {
  brtrimws(((_kw_t*)k)->output_buffer); 
  bprintf(k, "</caption>"); 
}

KIWI_ACTION(cell_attribute_list_action_1) {
	kw_list_iterate(((_kw_t*)k)->tag_attributes_list.head->next, validate_tag_attributes, k);
  bprintf(k, "%s>", bdata(((_kw_t*)k)->tag_attributes_validated));
}

KIWI_ACTION(cell_attribute_name_action_1) {
  btrunc(((_kw_t*)k)->tag_attribute, 0);
  bcatcstr(((_kw_t*)k)->tag_attribute, text);
}

KIWI_ACTION(cell_attribute_value_action_1) {
  struct node *node = kw_list_append_new(&((_kw_t*)k)->tag_attributes_list);
  bconcat(node->name, ((_kw_t*)k)->tag_attribute);
  bcatcstr(node->content, text);
  btrimws(node->name);
  btrimws(node->content);
}

KIWI_ACTION(cell_close_action_1) {
  brtrimws(((_kw_t*)k)->output_buffer); 
  bprintf(k, "</td>"); 
}

KIWI_ACTION(sol_cell_open_action_1) {
  init_tag_vars(k); 
  bcatcstr(((_kw_t*)k)->tag_name, "td");
  bprintf(k, "<td"); 
}

KIWI_ACTION(inline_cell_open_action_1) {
  init_tag_vars(k);
  bcatcstr(((_kw_t*)k)->tag_name, "td");
  bprintf(k, "<td"); 
}

KIWI_ACTION(complex_header_action_1) {
  init_tag_vars(k); 
  bcatcstr(((_kw_t*)k)->tag_name, "th"); 
  bprintf(k, "<th"); 
}

KIWI_ACTION(complex_header_action_2) {
  // Simpler to check the last output character than to hack up the LEG rules
  if(bdata(((_kw_t*)k)->output_buffer)[((_kw_t*)k)->output_buffer->slen - 1] != '>') {
    bprintf(k, ">"); 
  }
}

KIWI_ACTION(template_name_action_1) {
  bcatcstr(((_kw_t*)k)->template_list.tail->name, text);
  brtrimws(((_kw_t*)k)->template_list.tail->name);
}

KIWI_ACTION(template_content_action_1) {
  bcatcstr(((_kw_t*)k)->template_list.tail->content, text);
  ((_kw_t*)k)->template_list.tail->level = hash(text);
}

KIWI_ACTION(template_close_action_1) {
  if(((_kw_t*)k)->template_list.tail->level == 0) {
    // Cover cases where the template has no arguments
    ((_kw_t*)k)->template_list.tail->level = hash("");
  }
  if(PRESERVE_TEMPLATES) {
    // Some bug in leg prevents using braces here or even in this comment
    bprintf(k, "__%s_%"PRIu64"__", bdata(((_kw_t*)k)->template_list.tail->name), hash(bdata(((_kw_t*)k)->template_list.tail->content)));
  }
}

KIWI_ACTION(tag_attribute_name_action_1) {
  btrunc(((_kw_t*)k)->tag_attribute, 0);
  bcatcstr(((_kw_t*)k)->tag_attribute, text);
}

KIWI_ACTION(tag_attribute_value_action_1) {
  struct node *node = kw_list_append_new(&((_kw_t*)k)->tag_attributes_list);
  bconcat(node->name, ((_kw_t*)k)->tag_attribute);
  bcatcstr(node->content, text);
  btrimws(node->name);
  btrimws(node->content);
}

KIWI_ACTION(tag_close_action_1) {
	kw_list_iterate(((_kw_t*)k)->tag_attributes_list.head->next, validate_tag_attributes, k);

  btolower(((_kw_t*)k)->tag_name);
  if(valid_html_tag(bdata(((_kw_t*)k)->tag_name), ((_kw_t*)k)->tag_name->slen)) {
    if(tag_self_closing(bdata(((_kw_t*)k)->tag_name)) && text && text[text_length-1] == '/') {
      bprintf(k, "<%s%s>", bdata(((_kw_t*)k)->tag_name), bdata(((_kw_t*)k)->tag_attributes_validated));
    } else {
      if(bdata(((_kw_t*)k)->tag_name)[0] == '/') {
        if(close_needed_tags(k)) return;
      } else {
        kw_push(&((_kw_t*)k)->tag_stack, (void *)bstrcpy(((_kw_t*)k)->tag_name));
        bstring last_tag = kw_peek(&((_kw_t*)k)->tag_stack, 1);
        if(last_tag != NULL)
        {
	  if(biseqcstr(((_kw_t*)k)->tag_name, "li"))
          {
            if(bstrcmp(last_tag, ((_kw_t*)k)->tag_name) == 0)
            {
	      close_tag(k, "li");
	      last_tag = kw_pop(&((_kw_t*)k)->tag_stack);
	      bdestroy(last_tag);
            }
          }
        }
      }
      bprintf(k, "<%s%s>", bdata(((_kw_t*)k)->tag_name), bdata(((_kw_t*)k)->tag_attributes_validated));
    }
  } else {
    strip_tags(((_kw_t*)k)->tag_name);
    bprintf(k, "&lt;%s&gt;", bdata(((_kw_t*)k)->tag_name));
  }
}

KIWI_ACTION(tag_action_1) {
  bcatcstr(((_kw_t*)k)->tag_name, text);
}

void kw_parse(Kw* k) {
  bprintf(k, "<p>");
  while(kw_low_level_parse(k)) {}
  bassigncstr(((_kw_t*)k)->tag_name, "");
  close_needed_tags(k);
  bprintf(k, "</p>");
  handle_toc(k);
}

