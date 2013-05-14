/* 
 * File:   common_mp.h
 * Author: machiry
 *
 * Created on February 28, 2013, 4:41 AM
 */

#ifndef COMMON_MP_H
#define	COMMON_MP_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>    
#include <string.h>
#include <math.h>    
#include <sys/time.h>
    
unsigned int ceil_log2( unsigned int n );
unsigned long getTimeDiff(struct timeval start,struct timeval end);
double getTimeDiffInMilli(struct timeval start,struct timeval end);


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_MP_H */

