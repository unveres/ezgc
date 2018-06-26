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
  return 0;
}
