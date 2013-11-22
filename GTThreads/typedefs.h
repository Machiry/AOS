/* 
 * File:   typedefs.h
 * Author: machiry
 *
 * Created on January 25, 2013, 1:16 PM
 */
#include "common.h"
#ifndef TYPEDEFS_H
#define	TYPEDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef unsigned long gtthread_t;
    typedef unsigned long gtthread_mutex_t;
    typedef unsigned long ulong;
    typedef unsigned long thr_priority;
#define MAX_PRIORITY 0
#define MIN_THR_PRIORITY 9
#define MIN_PRIORITY MIN_THR_PRIORITY+1

    typedef struct POINTERLISTNODE {
        void* curr;
        struct POINTERLISTNODE* next;
    } PTRNODE;

    typedef enum {
        INVALID_MIN,
        FIFO,
        PRIORITY,
        INVALID_MAX
    } SchedulerType;

    typedef struct TCBLISTNODE {
        gtthread_t currThreadID;
        ucontext_t* threadContext;
        void* returnValue;
        volatile int isCompleted;
        volatile int isBlocked;
        thr_priority currThreadPriority;
        int isMainThread;
        void **joinedReturnStatus;
        struct TCBLISTNODE* prev;
        struct TCBLISTNODE* next;
        struct TCBLISTNODE* blockedOnThread;
        PTRNODE* waitingThreads;
    } TCBNODE;

    typedef struct MutexStruct {
        gtthread_mutex_t mutexID;
        int islocked;
        TCBNODE* lockedTCB;
        //This is required to pick one the threads in random to wake up
        //We do it FIFO!! gotta complete the paper. if i get time i will try to pick a random TCB
        int numberOfWaitingThreads;
        PTRNODE* waitingThreads;
        struct MutexStruct* next;
    } MUNODE;

    typedef struct COMPLLISTNODE {
        gtthread_t currThreadID;
        void **completedStatus;
        struct COMPLLISTNODE * next;
    } COMPLNODE;



#ifdef	__cplusplus
}
#endif

#endif	/* TYPEDEFS_H */

