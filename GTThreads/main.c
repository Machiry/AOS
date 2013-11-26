

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

#include <gtthread.h>

gtthread_mutex_t chopsticks[5];
gtthread_t philisophers[5];
gtthread_mutex_t randMutex;
int getRand(int limit) {
    int retVal;
    //gtthread_mutex_lock(&randMutex);
    //printf("Random Enter\n");
    retVal =  (rand() % limit) + 1;
    //printf("Random Exit:%d\n",retVal);
    //gtthread_mutex_unlock(&randMutex);
    return retVal;
}

void* philosopherThread(void* philosopherNumber) {
    int philosopherNo = *((int*)philosopherNumber);
    unsigned int iteration = 1;
    while (1) {
        //printf("Philosopher #%d , %u is Thinking\n",philosopherNo+1,iteration);
        long dummy = 10000 * getRand(100000);
        while(dummy > 0){
            dummy--;
        }
        //printf("Philosopher #%d , %u is Hungry\n",philosopherNo+1,iteration);
        int firstForkNumber = philosopherNo != 4 ? philosopherNo : 0;
        int secondForkNumber = philosopherNo != 4 ? philosopherNo + 1 : 4;
        //printf("Philosopher #%d , %u Trying to get fork #%d\n",philosopherNo+1,iteration,firstForkNumber+1);
        gtthread_mutex_lock(&chopsticks[firstForkNumber]);
        //printf("Philosopher #%d , %u got fork #%d\n",philosopherNo+1,iteration,firstForkNumber+1);
        //printf("Philosopher #%d , %u Trying to get fork #%d\n",philosopherNo+1,iteration,secondForkNumber+1);
        gtthread_mutex_lock(&chopsticks[secondForkNumber]);
        //printf("Philosopher #%d , %u got fork #%d\n",philosopherNo+1,iteration,secondForkNumber+1);
        printf("Philosopher #%d , %u is Eating\n",philosopherNo+1,iteration);
        dummy = 10000 * getRand(100000);
        while(dummy > 0){
            dummy--;
        }
        gtthread_mutex_unlock(&chopsticks[firstForkNumber]);
        //printf("Philosopher #%d , %u released fork #%d\n",philosopherNo+1,iteration,firstForkNumber+1);
        gtthread_mutex_unlock(&chopsticks[secondForkNumber]);
        //printf("Philosopher #%d , %u released fork #%d\n",philosopherNo+1,iteration,secondForkNumber+1); 
        iteration++;
    }
}

void thread3(void *dummy1){
    long dummy = ~(-1);
    gtthread_t th1;
    int i=0;
    printf("Thread3,running with priority:%d\n",gtthread_getpriority(gtthread_self()));
    for(i=0;i<10;i++){
        printf("Thread3:%d, still running\n",i);
    dummy = ~(-1);
    while(dummy>0){
        dummy--;
    }
    }
    printf("Thread3 exiting\n");
}
void thread2(void *dummy1){
    long dummy = ~(-1);
    gtthread_t th1;
    long dummy2;
    int i=0;
    printf("Thread2,running with priority:%d\n",gtthread_getpriority(gtthread_self()));
    for(i=0;i<10;i++){
        printf("Thread2, still running\n");
    dummy = ~(-1);
    while(dummy>0){
        dummy--;
    }
    if(i==1){
        printf("Trying to create thread3 with priority:%d\n",2);
        gtthread_create(&th1,thread3,&i,2);
        dummy = ~(-1);
        while(dummy>0){
            dummy1=dummy;
            while(dummy2 >0){
                dummy2--;
            }
        dummy--;
        }
    }
    }
    printf("Thread2 waiting for thread3\n");
    gtthread_join(th1,NULL);
    printf("Thread2 Exiting\n");
}
void* thread1(void *dummy1){
    long dummy = ~(-1);
    long dummy2;
    gtthread_t th1;
    int i=0;
    printf("Thread1,running with priority:%d\n",gtthread_getpriority(gtthread_self()));
    for(i=0;i<10;i++){
        printf("Thread1, still running\n");
    dummy = ~(-1);
    while(dummy>0){
        dummy--;
    }
    if(i==1){
        printf("Trying to create thread2 with priority:%d\n",4);
        gtthread_create(&th1,thread2,&i,4);
        dummy = ~(-1);
        while(dummy>0){
            dummy1=dummy;
            while(dummy2 >0){
                dummy2--;
            }
        dummy--;
        }
    }
    }
    printf("Thread1 waiting for thread2\n");
    gtthread_join(th1,NULL);
    printf("Thread1 Exiting\n");
}

/*void main() {
    int i=0;
    static int arr[5] = {0,1,2,3,4};
    gtthread_init(1000,FIFO,MAX_PRIORITY);
    for(i=0;i<5;i++){
        gtthread_mutex_init(&chopsticks[i]);
    }
    gtthread_mutex_init(&randMutex);
    for(i=0;i<5;i++){
        gtthread_create(&philisophers[i],philosopherThread,&arr[i],MAX_PRIORITY);
    }
    
    while(1);
    printf("Main Exiting\n");
}*/
void main(){
    gtthread_init(100,PRIORITY,MIN_THR_PRIORITY);
    long dummy = ~(-1);
    long dummy1;
    gtthread_t th1;
    int i=0;
    printf("Main thread,running with priority:%d\n",gtthread_getpriority(gtthread_self()));
    for(i=0;i<3;i++){
        printf("Main thread, still running\n");
    dummy = ~(-1);
    while(dummy>0){
        dummy--;
    }
    if(i==1){
        printf("Trying to create thread1 with priority:%d\n",4);
        gtthread_create(&th1,thread1,&i,4);
        dummy = ~(-1);
        while(dummy>0){
            dummy1=dummy;
            while(dummy1 >0){
                dummy1--;
            }
        dummy--;
        }
    }
    }
    gtthread_join(th1,NULL);
    printf("Main Exiting\n");
    
}