#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

void kw_stack_init(stack *s) {
  s->stack = malloc(STACK_DEFAULT_SIZE * sizeof(void*));
  s->pos = 0;
  s->mlen = STACK_DEFAULT_SIZE;
}

void kw_stack_free(stack *s) {
  if(!s) return;
  if(!s->stack) return;

  free(s->stack);
}

void *kw_stack_grow(stack *s) {
  void *junk;   // So we don't whack s->stack on failure
  s->mlen *= 2; // Double it
  junk = realloc(s->stack, sizeof(void *) * s->mlen);
  return junk;
}

int kw_push(stack *s, void *item) {
  if(!s) {
    fprintf(stderr, "Bad stack passed to kw_pop()\n");
	return -1;
  }
  if(!item) return -1;
  if(s->pos >= s->mlen && !kw_stack_grow(s)) {
    fprintf(stderr, "Realloc failed\n"); 
    return -1;
  }

  s->stack[s->pos] = item;
  return ++s->pos;
}

void *kw_pop(stack *s) {
  if(!s) {
    fprintf(stderr, "Bad stack passed to kw_pop()\n");
	return NULL;
  }
  if(s->pos > s->mlen) return NULL;
  if(s->pos - 1 < 0) return NULL;

  return s->stack[--s->pos];
}

void *kw_peek(stack *s, int back) {
  if(!s) {
    fprintf(stderr, "Bad stack passed to kw_peek()\n");
	return NULL;
  }
  int pos = s->pos - back - 1;
  if(pos < 0) {
	return NULL;
  }

  return s->stack[pos];
}
