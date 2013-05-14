/* 
 * File:   mcs_barrier.c
 * Author: machiry
 *
 * Created on March 1, 2013, 12:55 PM
 */

#include "common_mp.h"
#include "common_mpi.h"
#include "mcs_barrier.h"

/*
 * 
 */
OMPTreeNode* initializeMCSCommonStructures_omp(unsigned int no_of_threads) {
    int i;
    int j = 0;
    OMPTreeNode* nodes = NULL;
    if (no_of_threads > 0) {
        nodes = (OMPTreeNode*) malloc(sizeof (OMPTreeNode) * no_of_threads);
        if (nodes) {
            memset(nodes, 0, sizeof (OMPTreeNode) * no_of_threads);
            for (i = 0; i < no_of_threads; i++) {
                for (j = 0; j < 4; j++) {
                    //This was the Bug in MCS paper
                    //in paper it is 4*i + j , where as it should be
                    // 4 * i + j + 1
                    nodes[i].haveChild[j] = (4 * i + j + 1) < no_of_threads;
                    nodes[i].childNotReady[j] = nodes[i].haveChild[j];
                }
                nodes[i].parentSense = 0;
                if (!i) {
                    nodes[i].parentPointer = &(nodes[i].dummy);
                } else {
#ifdef DEBUG1
                    printf("For Node:%d parent:%d,%d\n", i, (i - 1) / 4, (i - 1) % 4);
#endif
                    nodes[i].parentPointer = &(nodes[(i - 1) / 4].childNotReady[(i - 1) % 4]);
                }
                nodes[i].childPointers[0] = (2 * i + 1) >= no_of_threads ? &(nodes[i].dummy) : &(nodes[2 * i + 1].parentSense);
                nodes[i].childPointers[1] = (2 * i + 2) >= no_of_threads ? &(nodes[i].dummy) : &(nodes[2 * i + 2].parentSense);
                nodes[i].dummy = 1;
#ifdef DEBUG1
                printf("For Node:%d children:%d,%d\n", i, (2 * i + 1) >= no_of_threads ? i : (2 * i + 1), (2 * i + 2) >= no_of_threads ? i : (2 * i + 2));
#endif
            }
        }

    }
    return nodes;
}


int initializeMCSCommonStructures_MPI(MPITreeNode* currN, unsigned int curr_thread_no, unsigned int no_of_threads) {
    int retVal = 1;
    int j = 0;
    if (no_of_threads > 0 && currN) {
        for (j = 0; j < 4; j++) {
            //This was the Bug in MCS paper
            //in paper it is 4*curr_thread_no + j , where as it should be
            // 4 * curr_thread_no + j + 1
            currN->haveChild[j] = (4 * curr_thread_no + j + 1) < no_of_threads;
            if (currN->haveChild[j]) {
                currN->childNotReady[j] = (4 * curr_thread_no + j + 1);
            }
        }
        currN->childPointers[0] = (2 * curr_thread_no + 1) >= no_of_threads ? curr_thread_no : 2 * curr_thread_no + 1;
        currN->childPointers[1] = (2 * curr_thread_no + 2) >= no_of_threads ? curr_thread_no : 2 * curr_thread_no + 2;
#ifdef DEBUG1
        printf("For Node:%d children:%d,%d\n", curr_thread_no, (2 * curr_thread_no + 1) >= no_of_threads ? curr_thread_no : (2 * curr_thread_no + 1), (2 * curr_thread_no + 2) >= no_of_threads ? curr_thread_no : (2 * curr_thread_no + 2));
#endif

        retVal = 0;
    }
    return retVal;
}

void mcs_barrier_omp(OMPTreeNode* currNode, int curr_thread_no, char* sense) {
    int allChildrenReady = 0;
    int i = 0;
    do {
        allChildrenReady = 1;
        for (i = 0; i < 4; i++) {
            if (currNode->childNotReady[i]) {
                allChildrenReady = 0;
                break;
            }
        }

    } while (!allChildrenReady);

    for (i = 0; i < 4; i++) {
        currNode->childNotReady[i] = currNode->haveChild[i];
    }
#ifdef DEBUG1
    printf("Entering second loop:%d\n", curr_thread_no);
#endif
    *(currNode->parentPointer) = 0;
    if (curr_thread_no) {
        while (currNode->parentSense != *sense);
    }
    *(currNode->childPointers[0]) = *sense;
    *(currNode->childPointers[1]) = *sense;
    *sense = !(*sense);
}

void mcs_barrier_mpi(MPITreeNode* currNode, int curr_thread_no,int roundNo) {
    int i = 0;
    MPI_Status mpi_result;
    int tag = 1;
    int msg = -1;
    for (i = 0; i < 4; i++) {
        if (currNode->haveChild[i]) {
            msg = -1;
            MPI_Recv(&msg, 1, MPI_INT, currNode->childNotReady[i], tag, MPI_COMM_WORLD, &mpi_result);
            if(msg != roundNo){
                fprintf(stderr,"Improper Msg Received by thread:%d, Expected: %d, Got:%d\n",curr_thread_no,roundNo,msg);
            }
        }
    }

#ifdef DEBUG1
    printf("Entering second loop:%d\n", curr_thread_no);
#endif

    if (curr_thread_no) {
        msg = roundNo;
        MPI_Send(&msg, 1, MPI_INT, (curr_thread_no - 1) / 4, tag, MPI_COMM_WORLD);
        msg = -1;
        MPI_Recv(&msg, 1, MPI_INT, (curr_thread_no - 1) / 2, tag, MPI_COMM_WORLD, &mpi_result);
        if(msg != roundNo){
                fprintf(stderr,"Improper Msg Received by thread:%d, Expected: %d, Got:%d\n",curr_thread_no,roundNo,msg);
        }
    }
    if (currNode->childPointers[0] != curr_thread_no) {
        msg = roundNo;
        MPI_Send(&msg, 1, MPI_INT, currNode->childPointers[0], tag, MPI_COMM_WORLD);
    }
    if (currNode->childPointers[1] != curr_thread_no) {
        msg = roundNo;
        MPI_Send(&msg, 1, MPI_INT, currNode->childPointers[1], tag, MPI_COMM_WORLD);
    }
}

