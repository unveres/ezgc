#include <stdlib.h>
#include <stdio.h>
#include <ezgc.h>

typedef struct {
  void *ptr;
  enum {
    ORDINARY,
    WEAK,         /* type flag: weak reference */
    SYMBOLIC,     /* type flag: symbolic reference */
    TOREF = 4,    /* if points to another reference */
    CYCLING = 8,  /* if reference in cycle */
  } flags;
} ref_t;

ref_t *mkref(int flags)
{
  ref_t *r;
  r = malloc(sizeof(ref_t));
  r->ptr = NULL;
  r->flags = flags & 3;
  return r;
}

void *rsref(ref_t *ref)
{
  ref_t *tmp;        /* copy of ref, used for first loop */
  int    is_cycling;

  tmp = ref;
  is_cycling = 0;

  while (tmp && tmp->flags & TOREF) {
    if (tmp->flags & CYCLING) {
      is_cycling = 1;
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
    return NULL; /* should return something else */

  return ref->ptr;
}

ref_t *setrefp(ref_t *dest, void *src)
{
  dest->ptr = src;
  dest->flags &= ~TOREF;
  return dest;
}

ref_t *setrefr(ref_t *dest, ref_t *src)
{
  dest->ptr = src;
  dest->flags |= TOREF;
  return dest;
}

void rmref(ref_t **ref)
{
  if (ref == NULL || *ref == NULL)
    return;

  free(*ref);
  *ref = NULL;
}

int main()
{
  ref_t *foo,
        *bar;

  foo = mkref(ORDINARY);
  bar = mkref(ORDINARY);

  setrefr(foo, bar);
  setrefr(bar, foo);
  printf("%p\n", rsref(foo));
  setrefp(bar, (void*)1);
  printf("%p\n", rsref(foo));

  exit(0);
}
