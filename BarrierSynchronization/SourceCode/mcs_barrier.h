/* 
 * File:   mcs_barrier.h
 * Author: machiry
 *
 * Created on March 1, 2013, 12:57 PM
 */

#ifndef MCS_BARRIER_H
#define	MCS_BARRIER_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct mcs_mpi_node {
        int childPointers[2];
        char haveChild[4];
        char childNotReady[4];
    } MPITreeNode;
    
     typedef struct mcs_omp_node {
        char parentSense;
        char* parentPointer;
        char *childPointers[2];
        char haveChild[4];
        char childNotReady[4];
        char dummy;
    } OMPTreeNode;
    
    OMPTreeNode* initializeMCSCommonStructures_omp(unsigned int no_of_threads);
    int initializeMCSCommonStructures_MPI(MPITreeNode* currN, unsigned int curr_thread_no, unsigned int no_of_threads);
    
    void mcs_barrier_omp(OMPTreeNode* currNode, int curr_thread_no, char* sense);
    void mcs_barrier_mpi(MPITreeNode* currNode, int curr_thread_no,int roundNo);
    


#ifdef	__cplusplus
}
#endif

#endif	/* MCS_BARRIER_H */

