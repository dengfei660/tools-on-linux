#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>

#ifdef  __cplusplus
extern "C" {
#endif

static long long getCurrentTimeMillis(void);

static int g_activeLevel= 5;

void Logger_set_level(int setLevel)
{
    if (setLevel <=0) {
        g_activeLevel = 0;
    } else if (setLevel > 6) {
        g_activeLevel = 6;
    } else {
        g_activeLevel = setLevel;
    }
}

void logPrint( int level, const char *fmt, ... )
{
   if ( level <= g_activeLevel )
   {
      va_list argptr;
      fprintf( stderr, "%lld: ", getCurrentTimeMillis());
      va_start( argptr, fmt );
      vfprintf( stderr, fmt, argptr );
      va_end( argptr );
   }
}

static long long getCurrentTimeMillis(void)
{
   struct timeval tv;
   long long utcCurrentTimeMillis;

   gettimeofday(&tv,0);
   utcCurrentTimeMillis= tv.tv_sec*1000LL+(tv.tv_usec/1000LL);

   return utcCurrentTimeMillis;
}
#ifdef  __cplusplus
}
#endif
