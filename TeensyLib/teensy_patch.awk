BEGIN { n = 0 }
/^void \* _sbrk\(int incr\)$/ { n = 1; print "__attribute__((weak)) void * _sbrk(int incr)" }
      { if (n == 0) print; n = 0;  }
