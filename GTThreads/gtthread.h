/* 
 * File:   libraryfunctions.h
 * Author: machiry
 *
 * Created on January 25, 2013, 12:16 AM
 */

#ifndef LIBRARYFUNCTIONS_H
#define	LIBRARYFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "typedefs.h"
//Threading mechanisms.
//#define DEBUG

    
int gtthread_init(ulong timeQuantum,SchedulerType stype,thr_priority mainThrPriority);
int gtthread_create(gtthread_t *thread,void *(*start_routine)(void *),void *arg,thr_priority currentPrio);
int  gtthread_join(gtthread_t thread, void **status);
void gtthread_exit(void *retval);
void gtthread_yield(void);
int  gtthread_equal(gtthread_t t1, gtthread_t t2);
int  gtthread_cancel(gtthread_t thread);
gtthread_t gtthread_self(void);
thr_priority gtthread_getpriority(gtthread_t t1);

//Synchronization Constructs.
int  gtthread_mutex_init(gtthread_mutex_t *mutex);
int  gtthread_mutex_lock(gtthread_mutex_t *mutex);
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex);


#ifdef	__cplusplus
}
#endif

#endif	/* LIBRARYFUNCTIONS_H */

