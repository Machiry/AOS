/* 
 * File:   libraryfunctions.c
 * Author: machiry
 *
 * Created on January 26, 2013, 9:00 PM
 */

#include "common.h"
#include "gtthread.h"
#include "GTError.h"

void scheduler(int sig);
struct itimerval originalTime, zeroTime;
struct sigaction schedulerAction;
int signalType = SIGVTALRM;
int timerType = ITIMER_VIRTUAL;
extern volatile int isInitialized = 0;
sigset_t ourSignals;

volatile TCBNODE* readyQueue[MIN_PRIORITY];
volatile COMPLNODE* completedThreads = null;
volatile TCBNODE* currentTCB = null;
volatile TCBNODE* blockedThreads = null;
gtthread_t currentThreadNo = 1;
SchedulerType currSchedulerType = INVALID_MAX;
#define DEFAULT_PRIORITY 0

static void initializeGlobals(ulong mSec, SchedulerType stype) {
    ulong seconds = mSec / (1000000);
    ulong mseconds = mSec % (1000000);
    memset(readyQueue, 0, sizeof (readyQueue));
    completedThreads = null;
    currentTCB = null;
    originalTime.it_interval.tv_sec = seconds;
    originalTime.it_interval.tv_usec = mseconds;
    originalTime.it_value.tv_sec = seconds;
    originalTime.it_value.tv_usec = mseconds;

    zeroTime.it_interval.tv_sec = 0;
    zeroTime.it_interval.tv_usec = 0;
    zeroTime.it_value.tv_sec = 0;
    zeroTime.it_value.tv_usec = 0;
    currSchedulerType = stype;
    sigemptyset(&ourSignals);
    sigaddset(&ourSignals, signalType);
    //printf("\nSignals:%u\n",ourSignals.__val[0]);
    memset(&schedulerAction, 0, sizeof (schedulerAction));
    sigemptyset(&schedulerAction.sa_mask);
    schedulerAction.sa_flags = 0;
    schedulerAction.sa_handler = &scheduler;
    sigaction(signalType, &schedulerAction, NULL);
}

static void myfree1(void *tobefreed) {
#ifdef DEBUG
    printf("Trying to free in myfree1:%x\n", tobefreed);
#endif
    free(tobefreed);
}

static void* mymalloc1(ulong size) {
    void* retVal = malloc(size);
#ifdef DEBUG
    printf("Allocating:%u and returning pointer:%x\n", size, retVal);
#endif
    return retVal;
}

static COMPLNODE* getNewCompletedNode(gtthread_t id, void *retVal) {
    COMPLNODE* retNode = (COMPLNODE*) mymalloc1(sizeof (COMPLNODE));
    if (retNode) {
        retNode->completedStatus = (void**) mymalloc1(sizeof (void**));
        retNode->currThreadID = id;
        retNode->next = null;
        if (retNode->completedStatus) {
            *(retNode->completedStatus) = retVal;
        } else {
            printf("ERROR:Unable to allocate new completed node return status: Insuccificent space\n");
            //printf("Trying to free in getNewCompletedNode\n");
            myfree1(retNode);
            retNode = null;
        }

    } else {
        printf("ERROR:Unable to allocate new completed node: Insuccificent space\n");
    }
    return retNode;
}

void insertIntoCompletedQueue(gtthread_t id, void *retVal) {
    COMPLNODE* newNode = getNewCompletedNode(id, retVal);
    if (newNode) {
        newNode->next = completedThreads;
        completedThreads = newNode;
    }
}

static void freePointerList(PTRNODE* list) {
    if (list) {
        PTRNODE* next = list->next;
        while (next) {
            PTRNODE* temp = next->next;
#ifdef DEBUG
            printf("Free in Pointer list1\n");
#endif
            myfree1(next);
            next = temp;
        }
#ifdef DEBUG
        printf("Free in Pointer list2\n");
#endif
        myfree1(list);
    }
}

static void doCleanUp(TCBNODE* targetTCBNode) {
    if (targetTCBNode) {
        //This change will ensure that if main thread exits in any case
        //The process exits
        /*if (targetTCBNode->isMainThread) {
            printf("Main Exiting..Terminating All!!!\n");
            exit(0);
        }*/
        //1. UNBLOCK all the threads waiting for this thread.
        PTRNODE* waitThreads = targetTCBNode->waitingThreads;
        if (waitThreads) {
            PTRNODE* currNode = waitThreads;
            while (waitThreads) {
                waitThreads = waitThreads->next;
                if (((TCBNODE*) currNode->curr)) {
                    //Update the return value to the parameter passed for joined status.
                    if (((TCBNODE*) currNode->curr)->joinedReturnStatus) {
                        *(((TCBNODE*) currNode->curr)->joinedReturnStatus) = targetTCBNode->returnValue;
                    }
#ifdef DEBUG
                    printf("Blocked threads return value set:%x\n", *(((TCBNODE*) currNode->curr)->joinedReturnStatus));
#endif
                    ((TCBNODE*) currNode->curr)->isBlocked = 0;
                }
#ifdef DEBUG
                printf("Free in doCleanup\n");
#endif
                //myfree1(currNode);
                currNode = waitThreads;
            }
        } else {
            //If there are no threads waiting then: add the result in to completed queue
            insertIntoCompletedQueue(targetTCBNode->currThreadID, targetTCBNode->returnValue);
        }
        targetTCBNode->isCompleted = 1;
    }
}

static void threadWrapper(void *(*start_routine)(void *), void *arg) {
    if (start_routine) {
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        void* funcRetVal = start_routine(arg);
        sigset_t oldMask;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        if (currentTCB) {
            currentTCB->returnValue = funcRetVal;
#ifdef DEBUG
            printf("Thread: %x return value set: %x\n", currentTCB->currThreadID, currentTCB->returnValue);
#endif
            //doCleanUp(currentTCB);
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        int ret = raise(signalType);
#ifdef DEBUG
        printf("Raise Returned:%d\n", ret);
#endif
    }
}

static void unblocker() {
    sigset_t oldMask;
    if (currentTCB) {
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        doCleanUp(currentTCB);
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
#ifdef DEBUG
        printf("Setting the sigmask and raising the signal\n");
#endif
        //Leave the CPU. let the scheduler schedule another thread
        raise(signalType);
    }

}

static void freeCompletedNode(COMPLNODE* ptr) {
    if (ptr) {
#ifdef DEBUG
        printf("Free in freeCompletedNode1\n");
#endif
        myfree1(ptr->completedStatus);
#ifdef DEBUG
        printf("Free in freeCompletedNode2\n");
#endif
        myfree1(ptr);
    }
}

static void freeCompletedListNode(COMPLNODE* ptr) {
    if (completedThreads && ptr) {
        COMPLNODE* currNode = completedThreads;
        COMPLNODE* prevNode = null;
        while (currNode && currNode != ptr) {
            prevNode = currNode;
            currNode = currNode->next;
        }
        COMPLNODE* tobeDeleted = currNode;
        if (tobeDeleted) {
            if (prevNode) {
                prevNode->next = tobeDeleted->next;
            }
            if (tobeDeleted == completedThreads) {
                completedThreads = tobeDeleted->next;
            }
            freeCompletedNode(tobeDeleted);
        }
    }
}

static void freeContext(ucontext_t* context) {
    if (context) {
        freeContext(context->uc_link);
        if (context->uc_stack.ss_sp) {
#ifdef DEBUG
            printf("Trying to free: %x\n", context->uc_stack.ss_sp);
            printf("Trying to free in freeContext1\n");
#endif
            void* tobeFreed = context->uc_stack.ss_sp;
            myfree1(tobeFreed);
        }
#ifdef DEBUG
        printf("Trying to free in freeContext2\n");
#endif
        myfree1(context);
    }
}

static void freeTCBNODE(TCBNODE* node) {
    if (node) {
        if (node->isMainThread) {
#ifdef DEBUG
            printf("Trying to free in freeTCBNODE1\n");
#endif
            myfree1(node->threadContext);
        } else {
            freeContext(node->threadContext);
        }
        freePointerList(node->waitingThreads);
#ifdef DEBUG
        printf("Trying to free in freeTCBNODE2\n");
#endif
        myfree1(node);
    }
}

TCBNODE* freeReadyListNode(TCBNODE* currNode) {
    TCBNODE* previ = null;
    TCBNODE* nextn = null;
    gtthread_t currThreadID;
    thr_priority currThreadPriority;
    if (currNode) {
        currThreadID = currNode->currThreadID;
        currThreadPriority = currNode->currThreadPriority;
        //Not sure when this will happen, but Just in case
        if (currentTCB == currNode) {
            currentTCB = NULL;
        }

        if (currNode->next == currNode) {
            previ = null;
            nextn = null;
        } else {
            previ = currNode->prev;
            nextn = currNode->next;
        }

        freeTCBNODE(currNode);
        if (previ) {
            previ->next = nextn;
        }
        if (nextn) {
            nextn->prev = previ;
        }
        if (!previ && !nextn) {
            readyQueue[currThreadPriority] = NULL;
        }
        if (readyQueue[currThreadPriority] == currNode) {
            readyQueue[currThreadPriority] = nextn;
        }
    }
    return nextn;
}

static gtthread_t getNewThreadID() {
    ++currentThreadNo;
    return currentThreadNo;
}

static TCBNODE* getNewTCBNode() {
    TCBNODE* retVal = (TCBNODE*) mymalloc1(sizeof (TCBNODE));
    if (retVal) {
        retVal->currThreadID = getNewThreadID();
        retVal->isBlocked = 0;
        retVal->isCompleted = 0;
        retVal->joinedReturnStatus = null;
        retVal->returnValue = null;
        retVal->threadContext = null;
        retVal->isMainThread = 0;
        retVal->currThreadPriority = DEFAULT_PRIORITY;
        retVal->waitingThreads = null;
        retVal->next = retVal->prev = null;
        retVal->blockedOnThread = null;
    } else {
        printf("ERROR:Unable to allocate new TCB: Insufficient memory\n");
    }
    return retVal;
}

static void insertIntoReadyQueue(TCBNODE *node) {
    if (node) {
        TCBNODE* currReadyQueue = readyQueue[node->currThreadPriority];
        if (currReadyQueue) {
            TCBNODE* lastNode = currReadyQueue->prev;
            node->prev = lastNode;
            node->next = lastNode->next;
            lastNode->next = node;
            currReadyQueue->prev = node;
        } else {
            readyQueue[node->currThreadPriority] = node;
            //Make it circular!! Fancy ahh!!??
            node->next = node;
            node->prev = node;
        }
        if (!currentTCB) {
            currentTCB = node;
        }
    } else {
        printf("WARNING:NULL node asked to insert in ready queue\n");
    }
}

static ucontext_t* getUnBlockerContext() {
    ucontext_t* retVal = null;
    retVal = (ucontext_t*) mymalloc1(sizeof (ucontext_t));
    if (!retVal) {
        printf("ERROR:Unable to create new  unblocker context: insufficient space\n");
    } else {
        getcontext(retVal);
        retVal->uc_stack.ss_size = COLLECTORSTACK;
        retVal->uc_stack.ss_sp = (void*) mymalloc1(COLLECTORSTACK);
        retVal->uc_stack.ss_flags = 0;
        if (!(retVal->uc_stack.ss_sp)) {
            myfree1(retVal);
            retVal = null;
            printf("ERROR:Unable to allocate stack for unblocker context : insufficient space\n");
        } else {
            retVal->uc_link = null;
            makecontext(retVal, unblocker, 0);
            //sigemptyset(&retVal->uc_sigmask);
        }
    }
    return retVal;
}

static ucontext_t* getNewContext(void *(*start_routine)(void *), void *arg) {
    ucontext_t* retVal = null;
    if (start_routine != null) {
        retVal = (ucontext_t*) mymalloc1(sizeof (ucontext_t));
        if (!retVal) {
            printf("ERROR:Unable to create new context: insufficient space\n");
        } else {
            getcontext(retVal);
            retVal->uc_stack.ss_size = MAINSTACKSIZE;
            retVal->uc_stack.ss_sp = mymalloc1(MAINSTACKSIZE);
            retVal->uc_stack.ss_flags = 0;
            if (!(retVal->uc_stack.ss_sp)) {
                myfree1(retVal);
                retVal = null;
                printf("ERROR:Unable to allocate stack: insufficient space\n");
            } else {
                //printf("Allocated: %x\n", retVal->uc_stack.ss_sp);
                //printf("Trying to free in malloc\n");
                //myfree1(retVal->uc_stack.ss_sp);
                //printf("Freeing done\n");
                //TODO: change this to collector
                //TODO: check if the same value can be used accross multiple threads.
                retVal->uc_link = getUnBlockerContext();
                //sigemptyset(&(retVal->uc_sigmask));
                makecontext(retVal, threadWrapper, 2, start_routine, arg);
                sigemptyset(&(retVal->uc_sigmask));
            }
        }
    }
    return retVal;
}

static TCBNODE* getTargetTCBNode(gtthread_t id) {
    TCBNODE* targetTCB = null;
    int i = 0;
    for (i = 0; i < MIN_PRIORITY; i++) {
        if (readyQueue[i]) {
            if (readyQueue[i]->currThreadID == id) {
                targetTCB = readyQueue[i];
                break;
            } else {
                TCBNODE* tempPtr = readyQueue[i]->next;
                while (tempPtr && tempPtr != readyQueue[i]) {
                    if (tempPtr->currThreadID == id) {
                        targetTCB = tempPtr;
                        break;
                    }
                    tempPtr = tempPtr->next;
                }
                if (tempPtr && tempPtr->currThreadID == id) {
                    targetTCB = tempPtr;
                    break;
                }
            }
        }
    }
    return targetTCB;
}

static COMPLNODE* getTargetComplNode(gtthread_t id) {
    if (completedThreads) {
        COMPLNODE* currThread = completedThreads;
        while (currThread) {
            if (currThread->currThreadID == id) {
                return currThread;
            }
            currThread = currThread->next;
        }
    }
    return null;
}

static PTRNODE* insertIntoWaitingList(TCBNODE* toInsert, PTRNODE* nextNode) {
    PTRNODE* retVal = null;
    PTRNODE* targetWaitNode = (PTRNODE*) mymalloc1(sizeof (PTRNODE));
    if (targetWaitNode) {
        targetWaitNode->curr = toInsert;
        targetWaitNode->next = nextNode;
        retVal = targetWaitNode;
    } else {
        printf("ERROR:Unable to allocate new waiting list node: Insufficient memory\n");
    }
    return retVal;
}

int gtthread_init(ulong timeQuantum, SchedulerType stype, thr_priority mainThrPriority) {
#ifdef DEBUG
    printf("Value of isInitilized = %d\n", isInitialized);
#endif
    if (!isInitialized) {
#ifdef DEBUG
        printf("Trying to initialize\n");
#endif
        if (timeQuantum > 0 && stype > INVALID_MIN && stype < INVALID_MAX) {
            initializeGlobals(timeQuantum, stype);
            ucontext_t* mainContext = (ucontext_t*) mymalloc1(sizeof (ucontext_t));
            TCBNODE* mainNode = getNewTCBNode();
            if (mainContext && mainNode) {
                mainNode->isMainThread = 1;
                mainNode->threadContext = mainContext;
                if (stype == PRIORITY) {
                    mainNode->currThreadPriority = mainThrPriority % MIN_PRIORITY;
                }
                //This is fine because : if main thread exits then
                //We should basically terminate
                mainNode->threadContext->uc_link = null;
                mainNode->threadContext->uc_stack.ss_flags = 0;
                insertIntoReadyQueue(mainNode);
                getcontext(mainContext);
                //sigemptyset(&mainContext->uc_sigmask);
                // printf("\nMain Signals:%u\n",mainContext->uc_sigmask.__val[0]);

#ifdef DEBUG
                printf("Setting timer\n");
#endif
                setitimer(timerType, &originalTime, null);
                isInitialized = 1;
                return OK;
            } else {
                return INSUFFICIENT_MEMORY;
            }
        }
        return INVALID_INPUT;
    }
    return INIT_DONE;
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg, thr_priority currentPrio) {
    sigset_t oldMask;
    int retVal = UNEXPECTED_ERROR;
    if (isInitialized) {
        currentPrio = currentPrio % MIN_PRIORITY;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
#ifdef DEBUG
        printf("Trying to create thread\n");
#endif
        TCBNODE* newNode = getNewTCBNode();
        if (newNode) {
            newNode->threadContext = getNewContext(start_routine, arg);
            //printf("\nCreated Context Signals:%u\n",newNode->threadContext->uc_sigmask.__val[0]);
#ifdef DEBUG
            printf("\nCreated Thread\n");
#endif
            if (!(newNode->threadContext)) {
                freeTCBNODE(newNode);
                retVal = INSUFFICIENT_MEMORY;
            } else {
                if (thread) {
                    *thread = newNode->currThreadID;
                }
                if (currSchedulerType == PRIORITY) {
                    newNode->currThreadPriority = currentPrio;
                }
#ifdef DEBUG
                printf("\nInserted into ready queue\n");
#endif
                insertIntoReadyQueue(newNode);
                if (currSchedulerType == PRIORITY) {
                    sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
                    if (!currentTCB || newNode->currThreadPriority < currentTCB->currThreadPriority) {
                        raise(signalType);
                    }
                }
                retVal = OK;
            }
        } else {
            retVal = INSUFFICIENT_MEMORY;
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}

int gtthread_join(gtthread_t thread, void **status) {
    sigset_t oldMask;
    int retVal = UNEXPECTED_ERROR;
    void* targetReturnStatus = null;
    if (isInitialized) {
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        TCBNODE* targetTCBN = getTargetTCBNode(thread);
#ifdef DEBUG
        printf("Getting target TCB\n");
#endif
        if (targetTCBN) {
#ifdef DEBUG
            printf("found target TCB\n");
#endif
            //The target thread is still running.
            if (!(targetTCBN->waitingThreads)) {
                //Only if no other thread has joined already.
                targetTCBN->waitingThreads = insertIntoWaitingList(currentTCB, targetTCBN->waitingThreads);
                if (targetTCBN->waitingThreads) {
                    //printf("Inserted into target TCB waiting threads\n");
                    currentTCB->joinedReturnStatus = status;
                    currentTCB->isBlocked = 1;
                    sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
#ifdef DEBUG
                    printf("Busy waiting: %d,%u\n", currentTCB->isBlocked, currentTCB->currThreadID);
#endif
                    while (currentTCB->isBlocked) {
                        //Don't wait let other threads run.
                        raise(signalType);
                        //printf("Busy waiting: %d,%u\n",currentTCB->isBlocked,currentTCB->currThreadID);
                    }
#ifdef DEBUG
                    printf("Exiting Busy waiting:%d,%u\n", currentTCB->isBlocked, currentTCB->currThreadID);
#endif
                    retVal = OK;
                } else {
                    retVal = INSUFFICIENT_MEMORY;
                }
            }
        } else {
            COMPLNODE* targetCompletedNode = getTargetComplNode(thread);
            if (targetCompletedNode) {
                targetReturnStatus = *(targetCompletedNode->completedStatus);
                freeCompletedListNode(targetCompletedNode);
                retVal = OK;
            }

            if (status) {
                *status = targetReturnStatus;
            }
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}

void gtthread_exit(void *retval) {
    sigset_t oldMask;
    if (isInitialized) {
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        if (currentTCB) {
            currentTCB->returnValue = retval;
            doCleanUp(currentTCB);
            sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
#ifdef DEBUG
            printf("Exiting thread:%lu\n", currentTCB->currThreadID);
#endif
            //Leave the CPU. let the scheduler schedule another thread
            raise(signalType);
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }

}

void gtthread_yield(void) {
    //sigset_t oldMask;
    //sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
    if (isInitialized) {
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
        raise(signalType);
    }
    //sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);

}

thr_priority gtthread_getpriority(gtthread_t t1){
    thr_priority retVal = -1;
    if (isInitialized && currSchedulerType == PRIORITY) {
        sigset_t oldMask;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        //1.find the corresponding TCBNODE
        TCBNODE* targetTCBN = getTargetTCBNode(t1);
        if (targetTCBN) {
            retVal = targetTCBN->currThreadPriority;
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}

int gtthread_equal(gtthread_t t1, gtthread_t t2) {
    int retVal = 0;
    if (isInitialized) {
        sigset_t oldMask;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        retVal = t1 == t2;
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;

}

int gtthread_cancel(gtthread_t thread) {
    int retVal = UNEXPECTED_ERROR;
    if (isInitialized) {
        sigset_t oldMask;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        //1.find the corresponding TCBNODE
        TCBNODE* targetTCBN = getTargetTCBNode(thread);
        if (targetTCBN) {
            //2. Try to finish it 
            targetTCBN->returnValue = null;
            doCleanUp(targetTCBN);
            retVal = OK;
            sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
            //Leave the CPU. let the scheduler schedule another thread
            raise(signalType);
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;

}

gtthread_t gtthread_self(void) {
    gtthread_t retVal = 0;
    if (isInitialized) {
        sigset_t oldMask;
        sigprocmask(SIG_BLOCK, &ourSignals, &oldMask);
        if (currentTCB) {
            retVal = currentTCB->currThreadID;
        }
        sigprocmask(SIG_UNBLOCK, &ourSignals, NULL);
    }
    return retVal;
}
