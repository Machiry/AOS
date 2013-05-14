/* 
 * File:   dissemination_barrier.c
 * Author: machiry
 *
 * Created on March 1, 2013, 12:48 PM
 */

#include "common_mp.h"
#include "common_mpi.h"
#include "dissemination_barrier.h"


/*
 * 
 */
ProcNode* initializeDisseminationCommonStructures_OMP(unsigned int no_of_threads) {
    ProcNode* allNodes = NULL;
    int allOk = 0;
    int i = 0;
    if (no_of_threads > 0) {
        unsigned int logp = ceil_log2(no_of_threads);
        allNodes = (ProcNode*) malloc(sizeof (ProcNode) * no_of_threads);
        if (allNodes) {
            for (i = 0; i < no_of_threads; i++) {
                allNodes[i].myflags[0] = (char*) malloc(sizeof (char) *logp);
                allNodes[i].myflags[1] = (char*) malloc(sizeof (char) *logp);
                if (!allNodes[i].myflags[0] || !allNodes[i].myflags[1]) {
                    allOk = 1;
                    break;
                }
                memset(allNodes[i].myflags[0], 0, logp);
                memset(allNodes[i].myflags[1], 0, logp);
                allNodes[i].partnerflags[0] = (char**) malloc(sizeof (char*) * logp);
                allNodes[i].partnerflags[1] = (char**) malloc(sizeof (char*) * logp);
                if (!allNodes[i].partnerflags[0] || !allNodes[i].partnerflags[1]) {
                    allOk = 1;
                    break;
                }
            }
            if (!allOk) {
                int r, k;
                for (i = 0; i < no_of_threads; i++) {
                    for (k = 0; k < logp; k++) {
                        for (r = 0; r < 2; r++) {
                            *(allNodes[i].partnerflags[r] + k) = allNodes[(i + (1 << k)) % no_of_threads].myflags[r] + k;
                        }
                    }
                }
            }
        }
    }
    return allNodes;
}

void dissemination_barrier_omp(ProcNode* localflags, char* sense, int logp, int* parity) {
    int i = 0;
    for (i = 0; i < logp; i++) {
        **(localflags->partnerflags[*parity] + i) = *sense;
        while (*(localflags->myflags[*parity] + i) != *sense);
    }
    if (*parity == 1) {
        *sense = !(*sense);
    }
    *parity = 1 - (*parity);
}

