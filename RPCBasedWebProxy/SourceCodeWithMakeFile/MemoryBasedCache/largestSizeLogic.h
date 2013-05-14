/* 
 * File:   largestSizeLogic.h
 * Author: machiry
 *
 * Created on March 17, 2013, 5:24 PM
 */

#ifndef LARGESTSIZELOGIC_H
#define	LARGESTSIZELOGIC_H

#ifdef	__cplusplus
extern "C" {
#endif

void updateLargestSizeMetaData(CacheBlockPtr currNode);
int compareLargestSizeHeapNode(HeapBlockPtr node1,HeapBlockPtr node2);
void linkLargestSizeCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr);
HeapBlockPtr getNewLargestSizeHeapNode(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LARGESTSIZELOGIC_H */

