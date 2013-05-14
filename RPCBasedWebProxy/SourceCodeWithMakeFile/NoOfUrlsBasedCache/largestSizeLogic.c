/* 
 * File:   largestSizeLogic.c
 * Author: machiry
 *
 * Created on March 17, 2013, 5:23 PM
 */

#include "proxy_utilities.h"
#include "heap_utilities.h"
#include "proxy_header.h"
#include "largestSizeLogic.h"


void updateLargestSizeMetaData(CacheBlockPtr currNode){
    //We do nothing, We don't need to update meta data after every access
    if(currNode && currNode->heapNode){
        HeapBlockPtr targetHeapNode = currNode->heapNode;
        if(targetHeapNode->key){
            *((long*)(targetHeapNode->key)) = currNode->size;
        }
    }
}

int compareLargestSizeHeapNode(HeapBlockPtr node1,HeapBlockPtr node2){
    if(node1 && node2){
        return *((long*)(node2->key)) - *((long*)(node1->key));
    }
    return node1 == node2 ? 0 : ((node1 == NULL ? -1 : 1));
}

void linkLargestSizeCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr){
    if(cacheBlockCurr && heapNodeCurr){
        cacheBlockCurr->heapNode = heapNodeCurr;
        heapNodeCurr->targetBlock = cacheBlockCurr;
        *((long*)(heapNodeCurr->key)) = 0;
    }
}

HeapBlockPtr getNewLargestSizeHeapNode(void){
     HeapBlockPtr newNode = getNewHeapNode();
    if(newNode){
        newNode->key = malloc(sizeof(long));
    }
    return newNode;
}


