/* 
 * File:   cache_utilities.c
 * Author: machiry
 *
 * Created on March 16, 2013, 3:01 PM
 */

#include "cache_utilities.h"
#include "proxy_header.h"



static unsigned long getKey(unsigned long hash){
    return hash % MAXSIZE_HASHTABLE;
}

/*
 * This will insert into the tree maintained as hash entry in the global hash table
 */
static CacheBlockPtr insertIntoTree(CacheBlockPtr root, CacheBlockPtr toInsert) {
    //printf("Inserting into Tree:start\n");
    if (root && toInsert) {
        CacheBlockPtr parent = NULL;
        CacheBlockPtr curr = root;
        while (curr) {
            parent = curr;
            if (curr->hashVal < toInsert->hashVal) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }
        }
        if (parent->hashVal < toInsert->hashVal) {
            parent->right = toInsert;
        } else {
            parent->left = toInsert;
        }
    }
    if (toInsert) {
        toInsert->left = NULL;
        toInsert->right = NULL;
    }
    //printf("Inserting into tree:end\n");
    return root ? root : toInsert;
}


/*
 * This will search the url in the tree located as hash table entry
 */
static CacheBlockPtr searchTree(char *url, unsigned long hashV, CacheBlockPtr root) {
    if (root && url) {
        if (root->hashVal < hashV) {
            return searchTree(url, hashV, root->right);
            //We compare both hash and URL to be safe
        } else if (root->hashVal == hashV && !strcmp(url, root->url)) {
            return root;
        } else {
            return searchTree(url, hashV, root->left);
        }
    }
    return NULL;
}


/*
 * This will insert a new cache block into the hashmap
 * 
 */
void insertIntoHashMap(CacheBlockPtr toInsert, CacheBlockPtr* hashMap) {
    if (toInsert && hashMap) {
        unsigned long hashKey = getKey(toInsert->hashVal);
        hashMap[hashKey] = insertIntoTree(hashMap[hashKey],toInsert);
    }
}

/*
 * Search the hashMap for a url and return the CacheBlockPtr if we find one
 */
CacheBlockPtr getCacheBlock(char* url, CacheBlockPtr* hashMap) {
    CacheBlockPtr toRet = NULL;
    if (url && hashMap) {
        unsigned long currHash = djbHash(url);
        unsigned long hashKey = getKey(currHash);
        //printf("Max Size:%d\n",MAXCACHESIZE);
        //printf("Got Key:%lu\n",hashKey);
        //printf("Contents:%x\n",hashMap[hashKey]);
        toRet = searchTree(url, currHash, hashMap[hashKey]);
    }
    return toRet;
}


/*
 * This will create a new cache block
 */
CacheBlockPtr getNewCacheBlock(char *url){
    CacheBlockPtr toRet = (CacheBlockPtr)malloc(sizeof(CacheBlock));
    if(toRet){
        if(url){
            toRet->url = (char*)malloc(strlen(url)+1);
            strcpy(toRet->url,url);
            toRet->hashVal = djbHash(url);
        } else{
            toRet->url = NULL;
            toRet->hashVal = 0;
        }
        toRet->data = NULL;
        toRet->left = toRet->right = NULL;
        toRet->heapNode = NULL;
        
        toRet->size = 0;
    } else{
        printf("Error: Insufficent memory\n");
    }
    return toRet;
}


void freeCacheBlock(CacheBlockPtr targetBlock){
    if(targetBlock){
        if(targetBlock->data){
                free(targetBlock->data);
        }
        if(targetBlock->url){
                free(targetBlock->url);
        }
        free(targetBlock);
    }
}

static CacheBlockPtr unlinkTreeNode(CacheBlockPtr root,CacheBlockPtr victim){
    if(root && victim){
        CacheBlockPtr parent = NULL;
        CacheBlockPtr curr = root;
        while(curr && (curr != victim)){
            parent = curr;
            if(victim->hashVal > curr->hashVal){
                curr = curr->right;
            } else{
                curr = curr->left;
            }
        }
        
        if(curr == victim){
            if (curr->left && curr->right) {
                CacheBlockPtr temp = curr->left;
                while (temp->right) {
                    temp = temp->right;
                }
                temp->right = curr->right;
                curr = curr->left;
            } else {
                curr = curr->left ? curr->left : curr->right;
            }
            if (parent) {
                if(parent->hashVal < victim->hashVal){
                    parent->right = curr;
                } else{
                    parent->left = curr;
                }
            } else {
                root = curr;
            }
        }
    }
    return root;
}

void unmapCacheBlock(CacheBlockPtr targetBlock,CacheBlockPtr* hashMap){
    if(hashMap && targetBlock){
        unsigned long currHash = targetBlock->hashVal;
        unsigned long hashKey = getKey(currHash);
        hashMap[hashKey] = unlinkTreeNode(hashMap[hashKey],targetBlock);
    }
}
