/* 
 * File:   randLogic.h
 * Author: machiry
 *
 * Created on March 17, 2013, 5:24 PM
 */

#ifndef RANDLOGIC_H
#define	RANDLOGIC_H

#ifdef	__cplusplus
extern "C" {
#endif

void updateRANDMetaData(CacheBlockPtr currNode);
int compareRANDHeapNode(HeapBlockPtr node1,HeapBlockPtr node2);
void linkRANDCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr);
HeapBlockPtr getNewRANDHeapNode(void);


#ifdef	__cplusplus
}
#endif

#endif	/* RANDLOGIC_H */

