/* 
 * File:   lruLogic.h
 * Author: machiry
 *
 * Created on March 17, 2013, 5:09 PM
 */
#include "proxy_data.h"
#ifndef LRULOGIC_H
#define	LRULOGIC_H

#ifdef	__cplusplus
extern "C" {
#endif

void updateLRUMetaData(CacheBlockPtr currNode);
int compareLRUHeapNode(HeapBlockPtr node1,HeapBlockPtr node2);
void linkLRUCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr);
HeapBlockPtr getNewLRUHeapNode(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LRULOGIC_H */

