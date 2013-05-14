/* 
 * File:   proxy_data.h
 * Author: machiry
 *
 * Created on March 16, 2013, 3:05 PM
 */

#ifndef PROXY_DATA_H
#define	PROXY_DATA_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAXSIZE_CACHE 100
    
#define MAXSIZE_HASHTABLE MAXSIZE_CACHE*4

    typedef struct ProxyCacheStruct{
        char *data; //Data to be stored in heap
        unsigned long size;
        char *url;
        unsigned long hashVal;
        void *heapNode;
        struct ProxyCacheStruct *left;
        struct ProxyCacheStruct *right;
    } CacheBlock;
    
    typedef CacheBlock* CacheBlockPtr;
    extern CacheBlockPtr cacheEntries[MAXSIZE_HASHTABLE];
    extern unsigned long currentCacheSize;
    
    typedef struct HeapNode{
      void *key;
      CacheBlockPtr targetBlock;
    } HeapBlock;
    typedef HeapBlock* HeapBlockPtr;
    extern HeapBlockPtr currentHeap[MAXSIZE_CACHE];

    typedef int (*CompareHeapNodes)(HeapBlockPtr,HeapBlockPtr);
    typedef void (*LinkAndInitialize)(CacheBlockPtr,HeapBlockPtr);
    typedef void (*UpdateMetaData)(CacheBlockPtr);
    typedef HeapBlockPtr (*GetNewHeapNode)(void);
    
    extern unsigned long MaxCacheMemSize;
#ifdef	__cplusplus
}
#endif

#endif	/* PROXY_DATA_H */

