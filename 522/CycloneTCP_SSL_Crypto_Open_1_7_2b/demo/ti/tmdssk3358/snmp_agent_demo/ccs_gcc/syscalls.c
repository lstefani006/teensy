#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

extern caddr_t __HeapBase;
extern caddr_t __HeapLimit;

int _write_r(struct _reent *r, int file, char *ptr, int len)
{
   int i;

   if(file == 1)
   {
      for(i = 0; i < len; i++)
         fputc(ptr[i], stdout);
   }
   else if(file == 2)
   {
      for(i = 0; i < len; i++)
         fputc(ptr[i], stderr);
   }

   return len;
}

int _read_r(struct _reent *r, int file, char *ptr, int len)
{
   errno = EINVAL;
   return -1;
}

int _lseek_r(struct _reent *r, int file, int ptr, int dir)
{
   return 0;
}

int _close_r(struct _reent *r, int file)
{
   return 0;
}

int _fstat_r(struct _reent *r, int file, struct stat *st)
{
   memset(st, 0, sizeof(*st));
   st->st_mode = S_IFCHR;
   return 0;
}

int _isatty_r(struct _reent *r, int fd)
{
   return 1;
}

caddr_t _sbrk_r(struct _reent *r, int incr)
{
   static caddr_t heap_end = NULL;
   caddr_t prev_heap_end;

   if(heap_end == NULL)
      heap_end = (caddr_t) &__HeapBase;

   prev_heap_end = heap_end;

   if((heap_end + incr) > (caddr_t) &__HeapLimit)
   {
      errno = ENOMEM;
      return (caddr_t) -1;
   }

   heap_end += incr;

   return prev_heap_end;
}
