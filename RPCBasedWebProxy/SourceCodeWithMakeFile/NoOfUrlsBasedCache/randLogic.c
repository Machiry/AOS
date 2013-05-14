/* 
 * File:   randLogic.c
 * Author: machiry
 *
 * Created on March 17, 2013, 5:23 PM
 */

#include "proxy_utilities.h"
#include "heap_utilities.h"
#include "proxy_header.h"
#include "randLogic.h"


static int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

void updateRANDMetaData(CacheBlockPtr currNode){
     /*if(currNode && currNode->heapNode){
        HeapBlockPtr targetHeapNode = currNode->heapNode;
        if(targetHeapNode->key){
            *((int*)targetHeapNode->key) = rand_lim(MAXCACHESIZE);
        }
    }*/
}

int compareRANDHeapNode(HeapBlockPtr node1,HeapBlockPtr node2){
    if(node1 && node2){
        return *((int*)(node1->key)) - *((int*)(node2->key));
    }
    return node1 == node2 ? 0 : ((node1 == NULL ? -1 : 1));
}

void linkRANDCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr){
    if(cacheBlockCurr && heapNodeCurr){
        cacheBlockCurr->heapNode = heapNodeCurr;
        heapNodeCurr->targetBlock = cacheBlockCurr;
        *((int*)(heapNodeCurr->key)) = rand_lim(1000);
    }
}

HeapBlockPtr getNewRANDHeapNode(void){
     HeapBlockPtr newNode = getNewHeapNode();
    if(newNode){
        newNode->key = malloc(sizeof(int));
    }
    return newNode;
}

