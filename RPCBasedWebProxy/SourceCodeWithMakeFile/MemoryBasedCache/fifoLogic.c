
#include <sys/time.h>

#include "proxy_utilities.h"
#include "heap_utilities.h"
#include "proxy_header.h"
#include "fifoLogic.h"

static unsigned long nextFIFONumber = 1;

void updateFIFOMetaData(CacheBlockPtr currNode){
    //We do nothing, We don't need to update meta data after every access
}

int compareFIFOHeapNode(HeapBlockPtr node1,HeapBlockPtr node2){
    if(node1 && node2){
        return *((long*)(node1->key)) - *((long*)(node2->key));
    }
    return node1 == node2 ? 0 : ((node1 == NULL ? -1 : 1));
}

void linkFIFOCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr){
    if(cacheBlockCurr && heapNodeCurr){
        cacheBlockCurr->heapNode = heapNodeCurr;
        heapNodeCurr->targetBlock = cacheBlockCurr;
        *((long*)(heapNodeCurr->key)) = nextFIFONumber++;
        //printf("Current Number:%d\n",nextFIFONumber);
    }
}

HeapBlockPtr getNewFIFOHeapNode(void){
     HeapBlockPtr newNode = getNewHeapNode();
    if(newNode){
        newNode->key = malloc(sizeof(long));
    }
    return newNode;
}
