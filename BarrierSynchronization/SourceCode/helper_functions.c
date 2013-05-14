#include "common_mp.h"

unsigned int ceil_log2( unsigned  int n ){
    return ceil( log( (double)n ) / log( 2 ));  
}

unsigned long getTimeDiff(struct timeval start,struct timeval end){
    unsigned long mtime, secs, usecs;
    secs  = end.tv_sec  - start.tv_sec;
    usecs = end.tv_usec - start.tv_usec;
    mtime = ((secs) * 1000 * 1000 + usecs);
    return mtime;
}

double getTimeDiffInMilli(struct timeval start,struct timeval end){
    unsigned long secs, usecs;
    secs  = end.tv_sec  - start.tv_sec;
    usecs = end.tv_usec - start.tv_usec;
    return (secs * 1000.0 + ((double)usecs/(1000.0)));
}
