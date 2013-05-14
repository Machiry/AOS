/* 
 * File:   dissemination_barrier.h
 * Author: machiry
 *
 * Created on March 1, 2013, 1:04 PM
 */

#ifndef DISSEMINATION_BARRIER_H
#define	DISSEMINATION_BARRIER_H

#ifdef	__cplusplus
extern "C" {
#endif

     typedef struct flags{
        char* myflags[2];
        char** partnerflags[2];
    } ProcNode;
    
    ProcNode* initializeDisseminationCommonStructures_OMP(unsigned int no_of_threads);
    void dissemination_barrier_omp(ProcNode* localflags, char* sense, int logp, int* parity);



#ifdef	__cplusplus
}
#endif

#endif	/* DISSEMINATION_BARRIER_H */

