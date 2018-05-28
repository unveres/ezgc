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
*/

#include <stdlib.h>
#include <string.h>
#include <ezgc.h>

typedef struct {
	void *ptr;
	size_t refs;
	char flags;
	void (*atfree)(void);
} gcblock;

static void *__realloc_gc(void *ptr, size_t size)
{
	if (size > 0)
		return realloc(ptr, size);

	free(ptr);
	return NULL;
}

static void *(*__realloc)(void *, size_t) = __realloc_gc;

void **gcmalloc(size_t size)
{
	gcblock *block;
	void	*ptr;

	ptr = __realloc(NULL, size);

	if (ptr == NULL)
		return NULL;

	block = __realloc(NULL, sizeof(gcblock));
	*block = (gcblock) {
		.ptr = ptr,
		.refs = /*0*/ 1,
		.flags = 0,
		.atfree = NULL
	};

	return (void**)block;
}

void **gcrealloc(void **_block, size_t size)
{
	gcblock	*block;
	block = (gcblock*)_block;

	if (block == NULL) {
		block = (gcblock*) gcmalloc(size);
		return (void**)block;
	}

	block->ptr = __realloc(block->ptr, size);

	if (block->ptr == NULL) {
		__realloc(block, 0);
		return NULL;
	}

	return (void**)block;
}

void **gccalloc(size_t num, size_t _size)
{
	size_t	size;
	void	**ptr;

	size = num * _size;
	ptr = gcmalloc(size);

	if (ptr == NULL || *ptr == NULL)
		return NULL;

	memset(*ptr, '\0', size);
	return ptr;
}

void gcfree(void **ptr)
{
	gcblock  *block;
	void 	(*atfree)(void);

	block = (gcblock *)ptr;

	if (block == NULL)
		return;

	atfree = block->atfree;

	if (block->ptr != NULL)
		__realloc(block->ptr, 0);
	
	__realloc(block, 0);
	atfree();
}

void gcsetrealloc(void *(*func)(void *, size_t))
{
	if (func == NULL)
		__realloc = __realloc_gc;
	else
		__realloc = func;
}

void gcatfree(void **ptr, void (*func)(void))
{
	((gcblock *)ptr)->atfree = func;
}

void gclink(void ***ref, void **ptr)
{
	gcblock	**old_block,
			*new_block;

	old_block = (gcblock **)ref;
	new_block = (gcblock *)ptr;

	if (new_block != NULL)
		++new_block->refs;

	if (*old_block != NULL) {
		/*if (*old_block != new_block || new_block->refs != 1)*/
			--(*old_block)->refs;

		if ((*old_block)->refs == 0)
			gcfree((void**)*old_block);
	}

	*ref = ptr;
}

size_t foo(void **ptr)
{
	return ((gcblock *)ptr)->refs;
}
