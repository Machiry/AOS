/* 
 * File:   lruLogic.c
 * Author: machiry
 *
 * Created on March 17, 2013, 5:08 PM
 */

#include <sys/time.h>

#include "proxy_utilities.h"
#include "heap_utilities.h"
#include "proxy_header.h"
#include "lruLogic.h"

void updateLRUMetaData(CacheBlockPtr currNode){
     if(currNode && currNode->heapNode){
        HeapBlockPtr targetHeapNode = currNode->heapNode;
        if(targetHeapNode->key){
            //printf("Current LFU count:%d\n",*((int*)targetHeapNode->key));
            gettimeofday(targetHeapNode->key,NULL);
        }
    }
}

int compareLRUHeapNode(HeapBlockPtr node1,HeapBlockPtr node2){
    if(node1 && node2){
        time_t sec1;
        time_t sec2;
        suseconds_t mic1;
        suseconds_t mic2;
        struct timeval *time1 = node1->key;
        struct timeval *time2 = node2->key;
        
        sec1 = time1->tv_sec;
        mic1 = time1->tv_usec;
        
        sec2 = time2->tv_sec;
        mic2 = time2->tv_usec;
        
        return sec1 == sec2 ? mic1 - mic2 : sec1-sec2;
    }
    return node1 == node2 ? 0 : ((node1 == NULL ? -1 : 1));
}

void linkLRUCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr){
    if(cacheBlockCurr && heapNodeCurr){
        cacheBlockCurr->heapNode = heapNodeCurr;
        heapNodeCurr->targetBlock = cacheBlockCurr;
        gettimeofday(heapNodeCurr->key,NULL);
    }
}

HeapBlockPtr getNewLRUHeapNode(void){
     HeapBlockPtr newNode = getNewHeapNode();
    if(newNode){
        newNode->key = malloc(sizeof(struct timeval));
    }
    return newNode;
}

