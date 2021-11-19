#include <limits.h>
#include <sys/time.h>
#include <stdlib.h>
#include "Times.h"

int64_t Times::getSystemTimeNenosec()
{
    // Clock support varies widely across hosts. Mac OS doesn't support
    // posix clocks, older glibcs don't support CLOCK_BOOTTIME and Windows
    // is windows.
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    gettimeofday(&t, NULL);
    return (t.tv_sec)*1000000000LL + (t.tv_usec)*1000LL;
}