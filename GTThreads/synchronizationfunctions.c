/* 
 * File:   synchronizationfunctions.c
 * Author: machiry
 *
 * Created on January 27, 2013, 2:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"
#include "common.h"

MUNODE* mutexList = null;
gtthread_mutex_t initialMutexID = 1;
int signalType;
extern volatile int isInitialized;

static gtthread_mutex_t getNewMutexID() {
    initialMutexID++;
    return initialMutexID;
}

static void myfree2(void *tobefreed) {
#ifdef DEBUG
    printf("Trying to free in myfree2:%x\n",tobefreed);
#endif
    free(tobefreed);
}

static void* mymalloc2(ulong size) {
    void* retVal = malloc(size);
#ifdef DEBUG
    printf("Allocating:%u and returning 2 pointer:%x\n",size,retVal);
#endif
    return retVal;
}

static PTRNODE* insertAtLast(TCBNODE* toInsert, PTRNODE* targetList) {
    PTRNODE* retVal = null;
    PTRNODE* targetWaitNode = (PTRNODE*) mymalloc2(sizeof (PTRNODE));
    if (targetWaitNode) {
        targetWaitNode->curr = toInsert;
        targetWaitNode->next = null;
        retVal = targetList ? targetList:targetWaitNode;
        PTRNODE* currNode = targetList;
        while(currNode && currNode->next){
            currNode = currNode->next;
        }
        if(currNode){
            currNode->next = targetWaitNode;
        }
    } else {
        printf("ERROR:Unable to allocate new waiting list node: Insufficient memory\n");
    }
    return retVal;
}



static MUNODE* getNewMutexNode() {
    MUNODE* retVal = (MUNODE*) mymalloc2(sizeof (MUNODE));
    if (retVal) {
        retVal->mutexID = getNewMutexID();
        retVal->lockedTCB = null;
        retVal->islocked = 0;
        retVal->numberOfWaitingThreads = 0;
        retVal->next = null;
        retVal->waitingThreads = null;
    } else {
        printf("ERROR:Unable to allocate getNetMutexNode: Insufficient space\n");
    }
    return retVal;
}

static MUNODE* getTheTargetMutexNode(gtthread_mutex_t id) {
    MUNODE* retVal = null;
    if (mutexList) {
        MUNODE* currNode = mutexList;
        while (currNode && currNode->mutexID != id) {
            currNode = currNode->next;
        }
        if (currNode) {
            retVal = currNode;
        }
    }
}

static void insertIntoMutexQueue(MUNODE* node) {
    if (node) {
        node->next = mutexList;
        mutexList = node;
    }
}

int gtthread_mutex_init(gtthread_mutex_t *mutex) {
    int retVal = UNEXPECTED_ERROR;
    if (isInitialized && mutex) {
        sigprocmask(SIG_BLOCK, &ourSignals, null);
        MUNODE* newNode = getNewMutexNode();
        *mutex = newNode->mutexID;
        insertIntoMutexQueue(newNode);
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        retVal = OK;
    }
    return retVal;
}

int gtthread_mutex_lock(gtthread_mutex_t *mutex) {
    int retVal = UNEXPECTED_ERROR;
    if (isInitialized && mutex) {
        sigprocmask(SIG_BLOCK, &ourSignals, null);
        MUNODE* targetMutex = getTheTargetMutexNode(*mutex);
        if (targetMutex) {
            if (targetMutex->islocked) {
                targetMutex->waitingThreads = insertAtLast(currentTCB, targetMutex->waitingThreads);
                targetMutex->numberOfWaitingThreads = targetMutex->numberOfWaitingThreads + 1;
                currentTCB->isBlocked = 1;
                sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
                //Busy wait till someone releases us.
                while (currentTCB->isBlocked) {
                    raise(signalType);
                }
                retVal = OK;
            } else {
                targetMutex->islocked = 1;
                targetMutex->lockedTCB = currentTCB;
                retVal = OK;
            }

        } else{
            retVal = INVALID_MUTEX;
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}

int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
    int retVal = UNEXPECTED_ERROR;
    if (mutex) {
        sigprocmask(SIG_BLOCK, &ourSignals, null);
        MUNODE* targetMutex = getTheTargetMutexNode(*mutex);
        if (targetMutex) {
            if (targetMutex->lockedTCB == currentTCB) {
                if (targetMutex->islocked) {
                    if(targetMutex->waitingThreads){
                        PTRNODE* toBeUnLocked = targetMutex->waitingThreads;
                        targetMutex->waitingThreads = targetMutex->waitingThreads->next;
                        targetMutex->numberOfWaitingThreads = targetMutex->numberOfWaitingThreads - 1;
                        if(targetMutex->numberOfWaitingThreads < 0) {
                            //something crazy might have happned!!!? I don't know
                            targetMutex->numberOfWaitingThreads = 0;
                        }
                        targetMutex->lockedTCB = toBeUnLocked->curr;
                        ((TCBNODE*)toBeUnLocked->curr)->isBlocked = 0;
                        if(targetMutex->numberOfWaitingThreads == 0){
                            //Again something crazy happned!!
                            targetMutex->waitingThreads = null;
                        }
                        myfree2(toBeUnLocked);
                    } else{
                        targetMutex->islocked = 0;
                        targetMutex->lockedTCB = null;
                    }
                    retVal = OK;
                } else {
                    retVal = MUTEX_NOT_LOCKED;
                }
            } else {
                retVal = NOT_OWNER_ERROR;
            }

        } else {
            retVal = INVALID_MUTEX;
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}


