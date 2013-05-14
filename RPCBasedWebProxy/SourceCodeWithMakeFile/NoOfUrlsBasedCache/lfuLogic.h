/* 
 * File:   lfuLogic.h
 * Author: machiry
 *
 * Created on March 16, 2013, 3:06 PM
 */

#include "proxy_data.h"

#ifndef LFULOGIC_H
#define	LFULOGIC_H

#ifdef	__cplusplus
extern "C" {
#endif



void updateLFUMetaData(CacheBlockPtr currNode);
int compareLFUHeapNode(HeapBlockPtr node1,HeapBlockPtr node2);
void linkLFUCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr);
HeapBlockPtr getNewLFUHeapNode(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LFULOGIC_H */

