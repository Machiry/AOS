/* 
 * File:   common.h
 * Author: machiry
 *
 * Created on January 24, 2013, 11:03 PM
 */

#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "typedefs.h"
#include <ucontext.h>
#include <string.h>
#include <stdlib.h>
#include "GTError.h"
extern struct itimerval originalTime, zeroTime;
extern struct sigaction schedulerAction;
extern int signalType;
extern int timerType;
extern int tempCount;

extern sigset_t ourSignals;
#define MAINSTACKSIZE 64*1024
#define COLLECTORSTACK 64*1024
#define null NULL
extern volatile TCBNODE* readyQueue[MIN_PRIORITY];
extern volatile TCBNODE* currentTCB;
extern volatile TCBNODE* blockedThreads;
extern volatile COMPLNODE* completedQueue;
extern SchedulerType currSchedulerType;
    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

