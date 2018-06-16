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

                                                            <ezgc.h>
Ezechiel's Garbage Collector for C, v1
*/

#ifndef _EZGC_H
#define _EZGC_H

#include <stddef.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#  define inline
#endif

/* no inline for ANSI C */

inline void gcsetrealloc(void *(*)(void *, size_t));
       void **gchug(void *);
inline void **gcmalloc(size_t);
inline void **gccalloc(size_t, size_t);
       void **gcrealloc(void **, size_t);
       void gcfree(void **);
inline void gcatfree(void **, void (*)(void));
       void gclink(void ***, void **);

#undef inline
#endif
