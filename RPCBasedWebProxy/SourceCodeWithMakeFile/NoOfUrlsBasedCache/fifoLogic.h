/* 
 * File:   fifoLogic.h
 * Author: machiry
 *
 * Created on March 17, 2013, 5:24 PM
 */

#ifndef FIFOLOGIC_H
#define	FIFOLOGIC_H

#ifdef	__cplusplus
extern "C" {
#endif

void updateFIFOMetaData(CacheBlockPtr currNode);
int compareFIFOHeapNode(HeapBlockPtr node1,HeapBlockPtr node2);
void linkFIFOCacheMetaData(CacheBlockPtr cacheBlockCurr,HeapBlockPtr heapNodeCurr);
HeapBlockPtr getNewFIFOHeapNode(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FIFOLOGIC_H */

