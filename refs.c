#include <stdlib.h>
#include <stdio.h>
#include <ezgc.h>

typedef struct {
  void *ptr;
  void *dep;          /* list of dependent references */

  enum {
    ORDINARY,
    WEAK,             /* type flag: weak reference */
    SYMBOLIC,         /* type flag: symbolic reference */
    TOREF = 4,        /* if points to another reference */
    CYCLING = 8,      /* if reference in cycle */
  } flags;
} ref_t;

struct refatom {
  ref_t *ref;
  struct refatom *next;
};

struct reflist {
  struct refatom *first;
  struct refatom *last;
};

#define DEFINENIL(NAME)                      \
static void __##NAME##_causeerror()          \
{                                            \
  *(char *)0 = 0;                            \
}                                            \
                                             \
void *NAME = (void **)__##NAME##_causeerror; \

DEFINENIL(emptyref);
DEFINENIL(noref);
DEFINENIL(cycleref);

ref_t *mkref(int flags)
{
  ref_t *r;
  r = malloc(sizeof(ref_t));
  r->ptr = NULL;
  r->dep = NULL;
  r->flags = flags & 3;
  return r;
}

void **rsref(ref_t *ref)
{
  ref_t *tmp;        /* copy of ref, used for first loop */
  int    is_cycling,
         is_empty;

  tmp = ref;
  is_cycling = 0;
  is_empty = 0;

  if (ref == NULL || ref == emptyref || ref == cycleref)
    return noref;

  if (!(ref->flags & SYMBOLIC))
    return (void **)ref->ptr;

  while (tmp && tmp->flags & TOREF) {
    if (tmp->flags & CYCLING) {
      is_cycling = 1;
      break;
    }

    if (tmp->ptr == NULL) {
      is_empty = 1;
      break;
    }

    tmp->flags |= CYCLING;
    tmp = tmp->ptr;
  }

  while (ref && ref->flags & CYCLING) {
    ref->flags &= ~CYCLING;
    ref = ref->ptr;
  }

  if (is_cycling)    
    return cycleref;

  if (is_empty)
    return emptyref;

  return (void**)ref->ptr;
}

ref_t *setrefp(ref_t **dest, void **src)
{
  if ((*dest)->flags & WEAK)
    (*dest)->ptr = (void *)src;
  else
    gclink((void***)&((*dest)->ptr), src);

  if (src == NULL) {
    free(*dest);
    *dest = NULL;
    return *dest;
  }

  (*dest)->flags &= ~TOREF;
  return *dest;
}

ref_t *setrefr(ref_t **dest, ref_t *src)
{
  if ((*dest)->flags & SYMBOLIC) {
    (*dest)->ptr = src;
    (*dest)->flags |= TOREF;
  } else
    (*dest)->ptr = (void *)rsref(src);

  return *dest;
}

/*void rmref(ref_t **ref)
{
  if (ref == NULL || *ref == NULL)
    return;

  free(*ref);
  *ref = NULL;
}*/

int main()
{
  ref_t *foo,
        *bar,
        *lol,
        *XD;

  printf("noref:    %p\n", noref);
  printf("emptyref: %p\n", emptyref);
  printf("cycleref: %p\n", cycleref);

  foo = mkref(SYMBOLIC);
  bar = mkref(SYMBOLIC);
  lol = mkref(SYMBOLIC);
  XD = mkref(SYMBOLIC);

  setrefr(&foo, bar);
  setrefr(&bar, lol);
  setrefr(&lol, XD);
  setrefr(&XD, foo);

  printf("%p\n", rsref(foo));
  setrefr(&XD, NULL);
  printf("%p\n%p\n", rsref(foo), rsref(NULL));
  setrefp(&XD, NULL);
  printf("%p\n", rsref(foo));
  
  exit(0);
}
