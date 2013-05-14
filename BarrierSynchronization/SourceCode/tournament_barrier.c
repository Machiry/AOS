/* 
 * File:   tournament_barrier.c
 * Author: machiry
 *
 * Created on March 1, 2013, 12:50 PM
 */

#include "common_mp.h"
#include "tournament_barrier.h"
#include "common_mpi.h"


RoundNodeOMP** initializeTournamentCommonStructures_omp(unsigned int no_of_threads) {
    int isStateSet;
    int i, k;
    RoundNodeOMP** rounds = NULL;
    if (no_of_threads > 0) {
        unsigned int logp = ceil_log2(no_of_threads);
        rounds = (RoundNodeOMP**) malloc(sizeof (RoundNodeOMP*) * no_of_threads);
        if (rounds) {
            memset(rounds, 0, sizeof (RoundNodeOMP*) * no_of_threads);
            for (i = 0; i < no_of_threads; i++) {
                rounds[i] = (RoundNodeOMP*) malloc(sizeof (RoundNodeOMP) * (logp + 1));
                if (rounds[i]) {
                    memset(rounds[i], 0, sizeof (RoundNodeOMP)*(logp + 1));
                }
            }
            for (i = 0; i < no_of_threads; i++) {
                for (k = 0; k <= logp; k++) {
                    RoundNodeOMP* currentRound = *(rounds + i) + k;
#ifdef DEBUG1
                    printf("i=%d,k=%d\n", i, k);
#endif
                    currentRound->flag = 0;
                    isStateSet = 0;
                    if (k > 0) {
                        if (!(i % (1 << k))) {
                            if (((i + (1 << (k - 1))) < no_of_threads) && ((1 << k) < no_of_threads)) {
                                currentRound->role = WINNER;
                                isStateSet = 1;
#ifdef DEBUG1
                                printf("i=%d,k=%d: WINNER\n", i, k);
#endif
                            } else if ((i + (1 << (k - 1))) >= no_of_threads) {
                                isStateSet = 1;
                                currentRound->role = BYE;
#ifdef DEBUG1
                                printf("i=%d,k=%d: BYE\n", i, k);
#endif
                            }
                        }
                        if (!isStateSet) {
                            if ((i % (1 << k)) == (1 << (k - 1))) {

                                currentRound->role = LOSER;
#ifdef DEBUG1
                                printf("i=%d,k=%d: LOSER\n", i, k);
#endif
                            } else if (i == 0 && ((1 << k) >= no_of_threads)) {
                                currentRound->role = CHAMPION;
#ifdef DEBUG1
                                printf("i=%d,k=%d: CHAMPION\n", i, k);
#endif
                            }
                        }
                    } else if (k == 0) {
                        currentRound->role = DROPOUT;
#ifdef DEBUG1
                        printf("i=%d,k=%d: DROPOUT\n", i, k);
#endif
                    }
                    RoundNodeOMP* toPoint = NULL;
                    int temp;
                    toPoint = NULL;
                    switch (currentRound->role) {
                        case LOSER:

                            temp = i - (1 << (k - 1));
                            toPoint = *(rounds + temp) + k;
                            break;
                        case WINNER:
                        case CHAMPION:

                            temp = i + (1 << (k - 1));
                            toPoint = *(rounds + temp) + k;
                            break;
                    }
                    if (toPoint) {
                        currentRound->opponent = &(toPoint->flag);
                    }
#ifdef DEBUG1
                    printf("i=%d,k=%d,%x\n", i, k, currentRound);
#endif
                }
            }
        }
    }
    return rounds;
}

void tournament_barrier_omp(RoundNodeOMP** rounds,int thread_no, int logp, char *sense) {
    int roundNo = 1;
    int needToExit = 0;
    if (logp > 0 && rounds) {
        while (1) {
            RoundNodeOMP* currNode = *(rounds + thread_no) + roundNo;
#ifdef DEBUG1
            printf("i=%d,k=%d,%x\n", thread_no, roundNo, currNode);
            printf("Barrier: loopi1 thread_no:%d , roundNo:%d role:%d\n", thread_no, roundNo, currNode->role);
#endif
            needToExit = 0;
            switch (currNode->role) {
                case LOSER:
                    *(currNode->opponent) = *sense;
                    while (currNode->flag != *sense);
                    needToExit = 1;
                    break;
                case WINNER:
                    while (currNode->flag != *sense);
                    break;
                case CHAMPION:
                    while (currNode->flag != *sense);
                    *(currNode->opponent) = *sense;
                    needToExit = 1;
                    break;
                case BYE: break;
                case DROPOUT: break;
            }
            if (needToExit) {
                break;
            }
            roundNo++;
        }
#ifdef DEBUG1
        printf("Thread : %d exited\n", thread_no);
#endif
        while (1) {
            roundNo--;
            RoundNodeOMP* currNode = *(rounds + thread_no) + roundNo;
            needToExit = 0;
            switch (currNode->role) {
                case LOSER: break;
                case WINNER:
                    *(currNode->opponent) = *sense;
                    break;
                case BYE: break;
                case CHAMPION: break;
                case DROPOUT:
                    needToExit = 1;
                    break;
            }
            if (needToExit) {
                break;
            }
        }

        *sense = !(*sense);
    }
}

int initializeTournamentCommonStructures_MPI(RoundNodeMPI* currentNodeRounds, unsigned int current_thread_no, unsigned int no_of_threads) {
    int retVal = 0;
    int isStateSet;
    int k;
    if (no_of_threads > 0) {
        unsigned int logp = ceil_log2(no_of_threads);
        if (currentNodeRounds) {
            for (k = 0; k <= logp; k++) {
                RoundNodeMPI* currentRound = currentNodeRounds + k;
#ifdef DEBUG1
                printf("i=%d,k=%d\n", current_thread_no, k);
#endif
                isStateSet = 0;
                if (k > 0) {
                    if (!(current_thread_no % (1 << k))) {
                        if (((current_thread_no + (1 << (k - 1))) < no_of_threads) && ((1 << k) < no_of_threads)) {
                            currentRound->currentRole = WINNER;
                            isStateSet = 1;
#ifdef DEBUG1
                            printf("i=%d,k=%d: WINNER\n", current_thread_no, k);
#endif
                        } else if ((current_thread_no + (1 << (k - 1))) >= no_of_threads) {
                            isStateSet = 1;
                            currentRound->currentRole = BYE;
#ifdef DEBUG1
                            printf("i=%d,k=%d: BYE\n", current_thread_no, k);
#endif
                        }
                    }
                    if (!isStateSet) {
                        if ((current_thread_no % (1 << k)) == (1 << (k - 1))) {

                            currentRound->currentRole = LOSER;
#ifdef DEBUG1
                            printf("i=%d,k=%d: LOSER\n", current_thread_no, k);
#endif
                        } else if (current_thread_no == 0 && ((1 << k) >= no_of_threads)) {
                            currentRound->currentRole = CHAMPION;
#ifdef DEBUG1
                            printf("i=%d,k=%d: CHAMPION\n", current_thread_no, k);
#endif
                        }
                    }
                } else if (k == 0) {
                    currentRound->currentRole = DROPOUT;
#ifdef DEBUG1
                    printf("i=%d,k=%d: DROPOUT\n", current_thread_no, k);
#endif
                }
                int temp = -1;
                switch (currentRound->currentRole) {
                    case LOSER:

                        temp = current_thread_no - (1 << (k - 1));
                        break;
                    case WINNER:
                    case CHAMPION:

                        temp = current_thread_no + (1 << (k - 1));
                        break;
                }
                if (temp >= 0) {
                    currentRound->opponent = temp;
                }
#ifdef DEBUG1
                printf("i=%d,k=%d,%x\n", current_thread_no, k, currentRound);
#endif
            }

        } else {
            retVal = 1;
        }
    }
    return retVal;
}

void tournament_barrier_mpi(RoundNodeMPI* currProcNode, int thread_no, unsigned int no_of_rounds) {
    int roundNo = 1;
    int needToExit = 0;
    MPI_Status mpi_result;
    int tag = 1;
    int msg;
    if (no_of_rounds > 0) {
        while (1) {
            RoundNodeMPI* currNode = currProcNode + roundNo;
#ifdef DEBUG1
            printf("i=%d,k=%d,%x\n", thread_no, roundNo, currNode);
            printf("Barrier: loopi1 thread_no:%d , roundNo:%d role:%d\n", thread_no, roundNo, currNode->currentRole);
#endif
            needToExit = 0;
            switch (currNode->currentRole) {
                case LOSER:
                    msg = roundNo;
                    MPI_Send(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD);
                    MPI_Recv(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD, &mpi_result);
                     if(msg != roundNo){
                                fprintf(stderr,"Tournament: Improper Msg Received by thread:%d, Expected: %d, Got:%d\n",thread_no,roundNo,msg);
                        }
                    needToExit = 1;
                    break;
                case WINNER:
                    MPI_Recv(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD, &mpi_result);
                    if(msg != roundNo){
                                fprintf(stderr,"Tournament: Improper Msg Received by thread:%d, Expected: %d, Got:%d\n",thread_no,roundNo,msg);
                        }
                    break;
                case CHAMPION:
                    MPI_Recv(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD, &mpi_result);
                    if(msg != roundNo){
                                fprintf(stderr,"Tournament: Improper Msg Received by thread:%d, Expected: %d, Got:%d\n",thread_no,roundNo,msg);
                        }
                    msg = roundNo;
                    MPI_Send(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD);
                    needToExit = 1;
                    break;
                case BYE: break;
                case DROPOUT: break;
            }
            if (needToExit) {
                break;
            }
            roundNo++;
        }
#ifdef DEBUG1
        printf("Thread : %d exited\n", thread_no);
#endif
        while (1) {
            roundNo--;
            RoundNodeMPI* currNode = currProcNode + roundNo;
            needToExit = 0;
            switch (currNode->currentRole) {
                case LOSER: break;
                case WINNER:
                    msg = roundNo;
                    MPI_Send(&msg, 1, MPI_INT, currNode->opponent, tag, MPI_COMM_WORLD);
                    break;
                case BYE: break;
                case CHAMPION: break;
                case DROPOUT:
                    needToExit = 1;
                    break;
            }
            if (needToExit) {
                break;
            }
        }
    }
}

