/* 
 * File:   tournament_barrier.h
 * Author: machiry
 *
 * Created on March 1, 2013, 1:05 PM
 */

#ifndef TOURNAMENT_BARRIER_H
#define	TOURNAMENT_BARRIER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    enum PossibleRoles{
        WINNER,
        LOSER,
        BYE,
        CHAMPION,
        DROPOUT
    };
    
    typedef struct round_node{
        enum PossibleRoles role;
        char* opponent;
        char flag;
    } RoundNodeOMP;

    typedef struct RoundData{
        enum PossibleRoles currentRole;
        int opponent;
    } RoundNodeMPI;

    int initializeTournamentCommonStructures_MPI(RoundNodeMPI* currentNodeRounds, unsigned int current_thread_no, unsigned int no_of_threads);
    
    RoundNodeOMP** initializeTournamentCommonStructures_omp(unsigned int no_of_threads);
    
    
    void tournament_barrier_mpi(RoundNodeMPI* currProcNode, int thread_no, unsigned int no_of_rounds);
    
    void tournament_barrier_omp(RoundNodeOMP** rounds,int thread_no, int logp, char *sense);
    


#ifdef	__cplusplus
}
#endif

#endif	/* TOURNAMENT_BARRIER_H */

