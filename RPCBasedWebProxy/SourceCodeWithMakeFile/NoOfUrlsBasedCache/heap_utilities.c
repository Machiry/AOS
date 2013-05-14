/* 
 * File:   heap_utilities.c
 * Author: machiry
 *
 * Created on March 16, 2013, 3:01 PM
 */

#include "heap_utilities.h"
#include "proxy_header.h"


/*
 * This wills shift the node recursively down till
 */

static void siftDown(HeapBlockPtr *actualHeap, int start, int end,CompareHeapNodes targetStrategy) {
    if (actualHeap && targetStrategy) {
        while (start * 2 + 1 <= end) {
            int leftChild = start*2+1;
            int rightChild = leftChild+1;
            int smallestChild = leftChild;
            if(rightChild <= end){
                int leftSmaller = targetStrategy(actualHeap[leftChild],actualHeap[rightChild]);
                if(leftSmaller <= 0){
                    smallestChild = leftChild;
                } else{
                    smallestChild = rightChild;
                }
            }
            
            int toSwap = targetStrategy(actualHeap[start],actualHeap[smallestChild]);
            if(toSwap > 0){
                HeapBlockPtr temp = actualHeap[start];
                actualHeap[start] = actualHeap[smallestChild];
                actualHeap[smallestChild] = temp;
                start = smallestChild;
            } else{
                break;
            }
            
        }
    }

}

/*
 * This is the normalization function which ensures that heap/priority queue is correctly
 * ordered
 * 
 */
void heapify(CompareHeapNodes targetStrategy, HeapBlockPtr *actualHeap, int size) {
    //printf("Heapify start\n");
    if (targetStrategy && actualHeap) {
        int start = (size-2)/2;
        while(start >= 0){
            siftDown(actualHeap,start,size-1,targetStrategy);
            start--;
        }
    }
    //printf("Heapify end\n");
}


/*
 * This will insert the given HeapBlock or PriorityQueue block into the heap
 * 
 */
void insertIntoHeap(HeapBlockPtr toInsert, HeapBlockPtr *actualHeap, unsigned long insertAt, CompareHeapNodes targetStrategy) {
    if (toInsert && actualHeap && targetStrategy) {
        actualHeap[insertAt] = toInsert;
        heapify(targetStrategy, actualHeap, insertAt + 1);
    }
}

HeapBlockPtr getNewHeapNode(){
    HeapBlockPtr toRet = (HeapBlockPtr)malloc(sizeof(HeapBlock));
    if(toRet){
        toRet->key = NULL;
        toRet->targetBlock = NULL;
    } else{
        printf("Error: Insufficent memory in HeapBlock\n");
    }
    return toRet;
}

CacheBlockPtr getCorrespondingCacheBlock(HeapBlockPtr currHeapNode){
    if(currHeapNode){
        return currHeapNode->targetBlock;
    }
    return NULL;
}

