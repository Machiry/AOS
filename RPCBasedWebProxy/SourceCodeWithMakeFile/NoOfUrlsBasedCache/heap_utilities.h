/* 
 * File:   heap_utilities.h
 * Author: machiry
 *
 * Created on March 16, 2013, 4:50 PM
 */

#ifndef HEAP_UTILITIES_H
#define	HEAP_UTILITIES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "proxy_data.h"
    void heapify(CompareHeapNodes targetStrategy,HeapBlockPtr *actualHeap,int size);
    void insertIntoHeap(HeapBlockPtr toInsert,HeapBlockPtr *actualHeap,unsigned long insertAt,CompareHeapNodes targetStrategy);
    HeapBlockPtr getNewHeapNode();
    CacheBlockPtr getCorrespondingCacheBlock(HeapBlockPtr currHeapNode);
#ifdef	__cplusplus
}
#endif

#endif	/* HEAP_UTILITIES_H */

