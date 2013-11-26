/* 
 * File:   scheduler.c
 * Author: machiry
 *
 * Created on January 26, 2013, 8:58 PM
 */
#include "common.h"

//#define DEBUG
void insertIntoCompletedQueue(gtthread_t id, void *retVal);
TCBNODE* freeReadyListNode(TCBNODE* currNode);
extern sigset_t ourSignals;

static TCBNODE* getNextTCBToSchedule() {
    int i = 0;
    TCBNODE* toRetTCB = NULL;
    if (currSchedulerType == PRIORITY) {
        for (i = 0; i < ((currentTCB && !currentTCB->isBlocked && !currentTCB->isCompleted)?(currentTCB->currThreadPriority+1):MIN_PRIORITY) && !toRetTCB; i++) {
            if (readyQueue[i]) {
                TCBNODE* prevTCB = readyQueue[i];
                TCBNODE* currTCB = readyQueue[i]->next;
                //Case when ready queue has only one TCB
                if(prevTCB == currTCB && currTCB != currentTCB){
                    toRetTCB = currTCB;
                    break;
                }
                prevTCB = NULL;
                while (currTCB && currTCB != prevTCB) {
                    if(!prevTCB){
                        prevTCB = currTCB;
                    }
                    TCBNODE* nextTCB = currTCB->next;
                    if (!currTCB->isBlocked) {
                        if (currTCB->isCompleted) {
                            freeReadyListNode(currTCB);
                        } else if (currTCB != currentTCB) {
                            toRetTCB = currTCB;
                            break;
                        }
                    }
                    currTCB = nextTCB;
                }
            }
        }
    }
    if (currSchedulerType == FIFO) {
        if (currentTCB) {
            TCBNODE* currTCB = currentTCB->next;
            while (currTCB && currTCB != currentTCB) {
                TCBNODE* nextTCB = currTCB->next;
                if (!currTCB->isBlocked) {
                    if (currTCB->isCompleted) {
                        freeReadyListNode(currTCB);
                    } else {
                        toRetTCB = currTCB;
                        break;
                    }
                }
                currTCB = nextTCB;
            }

        }
    }
    return toRetTCB;
}

void scheduler(int sig) {
    //This is to avoid timer interrupts while scheduler is running.
    //This is also to avoid scheduler time being taken from thread quantum.
#ifdef DEBUG
    printf("Scheduler Called\n");
#endif
    //setitimer(timerType, &zeroTime, null);
    //sigprocmask(SIG_BLOCK, &ourSignals,NULL);
    TCBNODE* nextToSchedule = getNextTCBToSchedule();
    if(!currentTCB){
        if(nextToSchedule){
            setitimer(timerType, &originalTime, null);
            sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
            currentTCB = nextToSchedule;
            setcontext(nextToSchedule->threadContext);
        }
    }
    else if (currentTCB->isCompleted) {
        //Here we need to set context.
        freeReadyListNode(currentTCB);
        setitimer(timerType, &originalTime, null);
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        if (nextToSchedule) {
            currentTCB = nextToSchedule;
            setcontext(nextToSchedule->threadContext);
        } else {
#ifdef DEBUG
            printf("\nAll threads finished execution");
#endif
            exit(0);
        }
    } else {
        setitimer(timerType, &originalTime, null);
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        //Here we need to do swap context
        if (nextToSchedule) {
            TCBNODE* savedTCB = currentTCB;
#ifdef DEBUG
            printf("Trying to swap context from:%d,with:%d\n", currentTCB->currThreadID, nextToSchedule->currThreadID);
#endif
            currentTCB = nextToSchedule;
            swapcontext(savedTCB->threadContext, nextToSchedule->threadContext);
        } else if (currentTCB->isBlocked) {
            //fprintf(stderr, "Deadlock detected. Deadlocked thread:%d\n", currentTCB->currThreadID);
        }
    }
    //This is required!!!. few times swap might return or if the current thread is the
    //only one which is capable of running.
    setitimer(timerType, &originalTime, null);
    sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    //setitimer(timerType, &originalTime, null);
}

