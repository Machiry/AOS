/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "proxy.h"
#include "proxy_data.h"
#include "lfuLogic.h"
#include "fifoLogic.h"
#include "randLogic.h"
#include "largestSizeLogic.h"
#include "lruLogic.h"
#include "proxy_header.h"
#include "cache_utilities.h"
#include "heap_utilities.h"
#include <curl/curl.h>

#define LRU 1
#define LFU 2
#define FIFO 3
#define RAND 4
#define LARGESTSIZE 5
//#define NOCACHE

struct wd_in {
    size_t size;
    size_t len;
    char **data;
};

CacheBlockPtr cacheEntries[MAXSIZE_HASHTABLE];
HeapBlockPtr currentHeap[MAXSIZE_CACHE];
int isInitialized = 0;
unsigned long currentCacheSize;
CompareHeapNodes currentStrategyComparison;
LinkAndInitialize currentStrategyHeapInitialization;
UpdateMetaData currentStrategyUpdateMetaData;
GetNewHeapNode currentStrategyGetHeapNode;
int replacementAlgo = 0;
FILE *currOutPutFile;

static size_t write_data(void *buffer, size_t size,
        size_t nmemb, void *userp) {
    struct wd_in *wdi = userp;

    //while (wdi->len + (size * nmemb) >= wdi->size) {
    /* check for realloc failing in real code. */
    wdi->size = wdi->len + (size * nmemb) + 1;
    *(wdi->data) = realloc(*(wdi->data), wdi->size);
    // wdi->size *= 2;
    //}

    memcpy(*(wdi->data) + wdi->len, buffer, size * nmemb);
    wdi->len += size*nmemb;
    (*(wdi->data))[wdi->len] = 0;
    return size * nmemb;
}

void initializeGlobalData(char *initString) {
    memset(cacheEntries, 0, MAXSIZE_HASHTABLE * sizeof (CacheBlockPtr));
    memset(currentHeap, 0, MAXSIZE_CACHE * sizeof (HeapBlockPtr));
    currentCacheSize = 0;
    int currentReplacementAlgo = 0;
    char outputFileName[1024];
    char *userString = "";
    printf("Got:%s\n", initString);
    if (initString) {
        char *savePtr;
        char *temp;
        temp = strtok_r(initString, ",", &savePtr);
        printf("First Token:%s\n", temp);
        if (strstr(temp, "init")) {
            temp = strtok_r(NULL, ",", &savePtr);
            printf("Second Token:%s\n", temp);
            if (temp) {
                userString = temp;
            }
            temp = strtok_r(NULL, ",", &savePtr);
            printf("Third Token:%s\n", temp);
            currentReplacementAlgo = atoi(temp);

        }
    }

    switch (currentReplacementAlgo) {
        case LRU:
            sprintf(outputFileName, "LRU_CacheSize%d_%s.txt", MAXSIZE_CACHE, userString);
            currentStrategyComparison = &compareLRUHeapNode;
            currentStrategyHeapInitialization = &linkLRUCacheMetaData;
            currentStrategyUpdateMetaData = &updateLRUMetaData;
            currentStrategyGetHeapNode = &getNewLRUHeapNode;
            break;
        case LFU:
            sprintf(outputFileName, "LFU_CacheSize%d_%s.txt", MAXSIZE_CACHE, userString);
            currentStrategyComparison = &compareLFUHeapNode;
            currentStrategyHeapInitialization = &linkLFUCacheMetaData;
            currentStrategyUpdateMetaData = &updateLFUMetaData;
            currentStrategyGetHeapNode = &getNewLFUHeapNode;
            break;
        case RAND:
            sprintf(outputFileName, "RAND_CacheSize%d_%s.txt", MAXSIZE_CACHE, userString);
            currentStrategyComparison = &compareRANDHeapNode;
            currentStrategyHeapInitialization = &linkRANDCacheMetaData;
            currentStrategyUpdateMetaData = &updateRANDMetaData;
            currentStrategyGetHeapNode = &getNewRANDHeapNode;
            break;
        case FIFO:
            sprintf(outputFileName, "FIFO_CacheSize%d_%s.txt", MAXSIZE_CACHE, userString);
            currentStrategyComparison = &compareFIFOHeapNode;
            currentStrategyHeapInitialization = &linkFIFOCacheMetaData;
            currentStrategyUpdateMetaData = &updateFIFOMetaData;
            currentStrategyGetHeapNode = &getNewFIFOHeapNode;
            break;
        case LARGESTSIZE:
            sprintf(outputFileName, "LARGESTSIZE_CacheSize%d_%s.txt", MAXSIZE_CACHE, userString);
            currentStrategyComparison = &compareLargestSizeHeapNode;
            currentStrategyHeapInitialization = &linkLargestSizeCacheMetaData;
            currentStrategyUpdateMetaData = &updateLargestSizeMetaData;
            currentStrategyGetHeapNode = &getNewLargestSizeHeapNode;
            break;
        default:
            fprintf(stderr, "Expecting initialization string: init,userString,1-5\n");
            exit(-1);
            break;
    }
    currOutPutFile = fopen(outputFileName, "w");
    fprintf(currOutPutFile, "UserString:%s\n", userString);
    replacementAlgo = currentReplacementAlgo;
    isInitialized = 1;
}

bool_t
proxy_fetch_1_svc(char **argp, webpageResponse *result, struct svc_req *rqstp) {
    static bool_t retval = 1;

#ifndef NOCACHE
    CacheBlockPtr targetCacheBlock = NULL;
    if (!isInitialized) {
        initializeGlobalData(*argp);
        srand(time(NULL));
        return retval;
    }
    if (!strcmp(*argp, "exit")) {
        printf("Exit Command Received. Exiting\n");
        fprintf(currOutPutFile, "Exit Command Received. Exiting\n");
        fflush(stdout);
        fflush(currOutPutFile);
        fclose(currOutPutFile);
        exit(0);
    }
    /*
     * insert server code here
     */
    //printf("Got :%s\n", *argp);
    //printf("size of cache:%d\n", currentCacheSize);
    targetCacheBlock = getCacheBlock(*argp, cacheEntries);
    //printf("Got Cache Block:%x\n", targetCacheBlock);
    if (targetCacheBlock) {
        fprintf(currOutPutFile, "Cache Hit for:%s\n", *argp);
        if (replacementAlgo != LARGESTSIZE) {
            currentStrategyUpdateMetaData(targetCacheBlock);
        }

    } else {
        CacheBlockPtr newNode = NULL;
        if (currentCacheSize < MAXSIZE_CACHE) {
            fprintf(currOutPutFile, "New Cache Block for:%s\n", *argp);
            //New Cache Entry
            newNode = getNewCacheBlock(*argp);
            //printf("Inserting new Cache Block:%x\n", newNode);
            if (newNode) {
                HeapBlockPtr currentHeapBlock = currentStrategyGetHeapNode();
                currentStrategyHeapInitialization(newNode, currentHeapBlock);
                insertIntoHeap(currentHeapBlock, currentHeap, currentCacheSize, currentStrategyComparison);
                insertIntoHashMap(newNode, cacheEntries);
                currentCacheSize++;
            }
        } else {
            //Replace a cache entry
            HeapBlockPtr victimHeapNode = currentHeap[0];
            CacheBlockPtr victimCacheBlock = getCorrespondingCacheBlock(victimHeapNode);
            fprintf(currOutPutFile, "Evicted Cache Block of: %s,for:%s\n", victimCacheBlock->url, *argp);
            //printf("To Remove:%x\n", victimCacheBlock);
            //printf("LFU Count:%d\n", *((int*) victimHeapNode->key));
            unmapCacheBlock(victimCacheBlock, cacheEntries);
            freeCacheBlock(victimCacheBlock);
            newNode = getNewCacheBlock(*argp);
            if (newNode) {
                currentStrategyHeapInitialization(newNode, victimHeapNode);
                insertIntoHashMap(newNode, cacheEntries);
            }
        }
        targetCacheBlock = newNode;

        CURL *curl;
        CURLcode res;
        struct wd_in wdi;

        memset(&wdi, 0, sizeof (wdi));
        wdi.data = &(targetCacheBlock->data);
        /* Get a curl handle.  Each thread will need a unique handle. */
        curl = curl_easy_init();

        if (NULL != curl) {
            wdi.size = 1024;
            /* Check for malloc failure in real code. */
            *(wdi.data) = malloc(wdi.size);

            /* Set the URL for the operation. */
            curl_easy_setopt(curl, CURLOPT_URL, *argp);

            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, -1);

            /* "write_data" function to call with returned data. */
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
            curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);

            /* userp parameter passed to write_data. */
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wdi);

            /* Actually perform the query. */
            //printf("Trying to do web query\n");
            res = curl_easy_perform(curl);
            //printf("Web Query finished\n");
            targetCacheBlock->size = wdi.len;
            //printf("Size:%d\n", targetCacheBlock->size);
            //printf("Pointer:%x,%x\n", *(wdi.data), targetCacheBlock->data);
            if (replacementAlgo == LARGESTSIZE) {
                currentStrategyUpdateMetaData(targetCacheBlock);
            }

            /* Check the return value and do whatever. */

            /* Clean up after ourselves. */
            curl_easy_cleanup(curl);
        } else {
            fprintf(stderr, "Error: could not get CURL handle.\n");
            //exit(EXIT_FAILURE);
        }

    }
    heapify(currentStrategyComparison, currentHeap, currentCacheSize);
    result->webPageContents = targetCacheBlock->data;
#endif


#ifdef NOCACHE
    CURL *curl;
    CURLcode res;
    struct wd_in wdi;
    char *temp;
    memset(&wdi, 0, sizeof (wdi));
    wdi.data = &(temp);
    /* Get a curl handle.  Each thread will need a unique handle. */
    curl = curl_easy_init();

    if (NULL != curl) {
        wdi.size = 1024;
        /* Check for malloc failure in real code. */
        *(wdi.data) = malloc(wdi.size);

        /* Set the URL for the operation. */
        curl_easy_setopt(curl, CURLOPT_URL, *argp);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, -1);

        /* "write_data" function to call with returned data. */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);

        /* userp parameter passed to write_data. */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wdi);

        /* Actually perform the query. */
        //printf("Trying to do web query\n");
        res = curl_easy_perform(curl);
        // printf("Web Query finished\n");
        // printf("Size:%d\n", targetCacheBlock->size);
        //printf("Pointer:%x,%x\n", *(wdi.data), targetCacheBlock->data);
        /* Check the return value and do whatever. *
        /* Clean up after ourselves. */
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Error: could not get CURL handle.\n");
        //exit(EXIT_FAILURE);
    }
    result->webPageContents =  *(wdi.data);
#endif

    
    //printf("\nWebPage Contents:%s\n",result->webPageContents);
    return retval;
}

int
proxy_fetch_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result) {
#ifdef NOCACHE
    xdr_free (xdr_result, result);
#endif
    /*
     * Insert additional freeing code here, if needed
     */

    return 1;
}