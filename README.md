# ezgc

Ezechiel's Garbage Collector for C

Minimalistic library providing garbage collection utilities written in C(99). It's very simple, featuring allocation functions analogical to those which exist in libc (gcmalloc, gcrealloc, gccalloc, gcfree). Example of use:

```
#include <stdio.h>
#include <string.h>
#include <ezgc.h>

void foo()
{
  puts("bar");
}

int main()
{
  void **str,
       **str2;

  str = gcmalloc(sizeof(char) * 64); // reference count: 1
  str2 = NULL;

  gcatfree(str, foo);
  strcpy(*str, "Hello World!");
  gclink(&str2, str); // reference count: 2

  printf("%s\n", (char*)*str);
  printf("%s\n\n", (char*)*str2);

  gclink(&str, NULL); // reference count: 1

  printf("%s\n", (char*)*str2);

  gclink(&str2, NULL); // deallocation and calling foo
  
  return 0;
}

```

Remember to link "libezgc.a" after compiling your project. ;)

How to install
--------------

Just download the binary [release](https://github.com/unveres/ezgc/releases/latest) tarball and unpack it to the root directory.
That's it.
