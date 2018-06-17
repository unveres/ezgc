/* 
Copyright (C) 2018 Ezechiel <admin@yehezkel.cba.pl>

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

                                                               ezgc v1.01
*/

#include <stdlib.h>
#include <string.h>
#include <ezgc.h>

/* ignore inline ANSI C */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#  define inline
#endif

typedef struct {
  void *ptr;
  size_t refs;
  void (*atfree)(void);
} gcblock;

static void *__realloc_default(void *ptr, size_t size)
{
  if (size > 0)
    return realloc(ptr, size);

  free(ptr);
  return NULL; /* this line is why we must implement this function */
}

static void *(*__realloc)(void *, size_t) = __realloc_default;

inline void **gchug(void *ptr)
{
  gcblock *block;

  if (ptr == NULL)
    return NULL;
  
  block = __realloc(NULL, sizeof(gcblock));
  block->ptr = ptr;
  block->refs = 1;
  block->atfree = NULL;
  
  return (void**)block;
}

inline void **gcmalloc(size_t size)
{
  return gchug(__realloc(NULL, size)); /* i like this malloc */
}

void **gcrealloc(void **_block, size_t size)
{
  gcblock *block;
  block = (gcblock*)_block;

  if (block == NULL)
    return gcmalloc(size);

  block->ptr = __realloc(block->ptr, size);

  if (block->ptr == NULL) {
    __realloc(block, 0);
    return NULL;
  }

  return (void**)block;
}

void **gccalloc(size_t num, size_t item_size)
{
  size_t   size;
  void   **ptr;

  size = num * item_size;
  ptr = gcmalloc(size);

  if (ptr != NULL && *ptr != NULL)
    memset(*ptr, '\0', size);

  return ptr;
}

inline void gcfree(void **ptr)
{
  gcblock  *block;
  void    (*atfree)(void);

  block = (gcblock *)ptr;

  if (block == NULL)
    return;

  atfree = block->atfree;

  if (block->ptr != NULL)
    __realloc(block->ptr, 0);
  
  __realloc(block, 0);

  if (atfree != NULL)
    atfree();
}

void gcsetrealloc(void *(*func)(void *, size_t))
{
    __realloc = func != NULL ? func : __realloc_default;
}

void gcatfree(void **ptr, void (*func)(void))
{
  ((gcblock *)ptr)->atfree = func;
}

void gclink(void ***ref, void **ptr)
{
  gcblock **old_block,
           *new_block;

  old_block = (gcblock **)ref;
  new_block = (gcblock *)ptr;

  if (new_block != NULL)
    ++new_block->refs;

  if (*old_block != NULL) {
    --(*old_block)->refs;

    if ((*old_block)->refs == 0)
      gcfree((void**)*old_block);
  }

  *ref = ptr;
}

#undef inline
