#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#undef errno
extern int errno;

extern caddr_t Heap_Bank3_Start;
extern daddr_t Heap_Bank3_Size;

extern void _init(void);
extern caddr_t _sbrk(int incr);
extern int link(char *old, char *new);
extern int _close(int file);
extern int _fstat(int file, struct stat *st);
extern int _isatty(int file);
extern int _lseek(int file, int ptr, int dir);
extern void _exit(int status);
extern void _kill(int pid, int sig);
extern int _getpid(void);

void _init(void)
{
}

int _write(int file, char *ptr, int len)
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

int _read(int file, char *ptr, int len)
{
   return 0;
}

extern caddr_t _sbrk(int incr)
{
   static caddr_t heap_ptr = NULL;
   caddr_t heap_base;

   if(heap_ptr == NULL)
      heap_ptr = (caddr_t) &Heap_Bank3_Start;

   heap_base = heap_ptr;

   if(incr > (int) &Heap_Bank3_Size)
   {
      errno = ENOMEM;
      return (caddr_t) -1;
   }

   heap_ptr += incr;

   return heap_base;
}

extern int link(char *old, char *new)
{
   return -1;
}

extern int _close(int file)
{
   return -1;
}

extern int _fstat(int file, struct stat *st)
{
   st->st_mode = S_IFCHR;
   return 0;
}

extern int _isatty(int file)
{
   return 1;
}

extern int _lseek(int file, int ptr, int dir)
{
   return 0;
}

extern void _exit(int status)
{
   while(1);
}

extern void _kill(int pid, int sig)
{
   return;
}

extern int _getpid(void)
{
   return -1;
}

int AllowPLLInitByStartup(void)
{
   return 1;
}
