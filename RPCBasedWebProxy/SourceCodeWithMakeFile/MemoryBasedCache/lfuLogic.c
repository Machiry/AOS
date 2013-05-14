/* 
 * File:   LfuLogic.c
 * Author: machiry
 *
 * Created on March 16, 2013, 2:55 PM
 */

#include "proxy_utilities.h"
#include "heap_utilities.h"
#include "proxy_header.h"
#include "lfuLogic.h"

/*
 * 
 */
void updateLFUMetaData(CacheBlockPtr currNode){
    if(currNode && currNode->heapNode){
        HeapBlockPtr targetHeapNode = currNode->heapNode;
        if(targetHeapNode->key){
            //printf("Current LFU count:%d\n",*((int*)targetHeapNode->key));
            *((int*)(targetHeapNode->key)) = *((int*)(targetHeapNode->key)) + 1;
        }
    }
}

int compareLFUHeapNode(HeapBlockPtr node1,HeapBlockPtr node2){
    if(node1 && node2){
        return  *((int*)(node1->key)) -  *((int*)(node2->key));
    }
    return node1 == node2 ? 0 : ((node1 == NULL ? -1 : 1));
}

void linkLFUCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr){
    if(cacheBlockCurr && heapNodeCurr){
        cacheBlockCurr->heapNode = heapNodeCurr;
        heapNodeCurr->targetBlock = cacheBlockCurr;
        *((int*)(heapNodeCurr->key)) = 1;
    }
}

HeapBlockPtr getNewLFUHeapNode(void){
    HeapBlockPtr newNode = getNewHeapNode();
    if(newNode){
        newNode->key = malloc(sizeof(int));
        if(newNode->key){
            *((int*)(newNode->key)) = 0;
        }
    }
    return newNode;
}


