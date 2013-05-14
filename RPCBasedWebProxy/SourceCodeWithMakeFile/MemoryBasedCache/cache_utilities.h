/* 
 * File:   cache_utilities.h
 * Author: machiry
 *
 * Created on March 16, 2013, 3:02 PM
 */

#ifndef CACHE_UTILITIES_H
#define	CACHE_UTILITIES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "proxy_utilities.h"
#include "proxy_data.h"

    void insertIntoHashMap(CacheBlockPtr toInsert,CacheBlockPtr* hashMap);
    CacheBlockPtr getCacheBlock(char* url,CacheBlockPtr* hashMap);
    CacheBlockPtr getNewCacheBlock(char *url);
    void freeCacheBlock(CacheBlockPtr targetBlock);
    void unmapCacheBlock(CacheBlockPtr targetBlock,CacheBlockPtr* hashMap);
#ifdef	__cplusplus
}
#endif

#endif	/* CACHE_UTILITIES_H */

