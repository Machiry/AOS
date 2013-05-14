/* 
 * File:   testHarness.c
 * Author: machiry
 *
 * Created on March 1, 2013, 1:52 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "common_mp.h"
#include "common_mpi.h"
#include "dissemination_barrier.h"
#include "tournament_barrier.h"
#include "mcs_barrier.h"

#define DefNoBarrierRounds 1000000
#define DefNoCombiBarrierRounds 1000

/*
 * 
 */

int testTournament_omp(int noOfRounds, int no_of_threads) {
    char sense = 1;
    unsigned int thread_no;
    unsigned int logp = 0;
    RoundNodeOMP** rounds = NULL;
    if (noOfRounds > 0 && no_of_threads > 0) {

        if (rounds = initializeTournamentCommonStructures_omp(no_of_threads)) {

            logp = ceil_log2(no_of_threads);
            omp_set_num_threads(no_of_threads);

#pragma omp parallel private(thread_no,sense) shared(no_of_threads,noOfRounds,logp)
            {
                thread_no = omp_get_thread_num();
                sense = 1;
                int i = noOfRounds;
                unsigned long totalTime = 0;
                struct timeval start, end;

                gettimeofday(&start, NULL);
                while (i > 0) {
                    /*#pragma omp critical
                                    {
                                        printf("Entering tournament barrier for :%d by %d\n", i, thread_no);
                                    }*/
                    tournament_barrier_omp(rounds, thread_no, logp, &sense);

                    /*#pragma omp critical
                                    {
                                        printf("Exiting tournament barrier for :%d by %d\n", i, thread_no);
                                    }*/
                    i--;
                }
                gettimeofday(&end, NULL);
                totalTime += getTimeDiff(start, end);
                fprintf(stdout, "\nTotal time for: %d rounds of Tournament OMP Barrier is %ld on %d with %d Threads\n", noOfRounds, totalTime, thread_no,no_of_threads);

            }

            return (EXIT_SUCCESS);
        }
    } else {
        return (EXIT_FAILURE);
    }
}

int testDissemination_omp(int noOfRounds, int no_of_threads) {
    char sense = 1;
    unsigned int parity = 0;
    unsigned int thread_no;
    unsigned int logp = 0;
    ProcNode* procNodes = NULL;

    if (procNodes = initializeDisseminationCommonStructures_OMP(no_of_threads)) {

        logp = ceil_log2(no_of_threads);
        omp_set_num_threads(no_of_threads);

#pragma omp parallel private(thread_no,sense,parity) shared(no_of_threads,noOfRounds,logp)
        {
            thread_no = omp_get_thread_num();
            ProcNode* currNode = procNodes + thread_no;
            unsigned long totalTime = 0;
            struct timeval start, end;
            sense = 1;
            parity = 0;
            int i = noOfRounds;
            gettimeofday(&start, NULL);
            while (i > 0) {
               /* #pragma omp critical
                                {
                                    printf("Entering dissemination barrier for :%d by %d\n", i, thread_no);
                                }*/
                dissemination_barrier_omp(currNode, &sense, logp, &parity);
                /*#pragma omp critical
                                {
                                    printf("Exiting dissemination barrier for :%d by %d\n", i, thread_no);
                                }*/
                i--;
            }
            gettimeofday(&end, NULL);
            totalTime += getTimeDiff(start, end);
            fprintf(stdout, "\nTotal time for: %d rounds of Dissemination OMP Barrier is %ld on %d with %d Threads\n", noOfRounds, totalTime, thread_no,no_of_threads);

        }

        return (EXIT_SUCCESS);
    } else {
        return (EXIT_FAILURE);
    }
}

int testMCS_omp(int noOfRounds, int no_of_threads) {
    char sense = 1;
    unsigned int thread_no;
    OMPTreeNode* nodes = NULL;

    if (nodes = initializeMCSCommonStructures_omp(no_of_threads)) {
        omp_set_num_threads(no_of_threads);

#pragma omp parallel private(thread_no,sense) shared(no_of_threads,noOfRounds)
        {
            thread_no = omp_get_thread_num();
            sense = 1;
            int i = noOfRounds;
            unsigned long totalTime = 0;
            struct timeval start, end;
            OMPTreeNode* currNode = nodes + thread_no;
            gettimeofday(&start, NULL);
            while (i > 0) {
                /*#pragma omp critical
                                {
                                    printf("Entering mcs barrier for :%d by %d\n", i, thread_no);
                                }*/
                mcs_barrier_omp(currNode, thread_no, &sense);
                /*#pragma omp critical
                                {
                                    printf("Exiting mcs barrier for :%d by %d\n", i, thread_no);
                                }*/
                i--;
            }
            gettimeofday(&end, NULL);
            totalTime += getTimeDiff(start, end);
            fprintf(stdout, "\nTotal time for: %d rounds of MCS OMP Barrier is %ld on %d with %d Threads\n", noOfRounds, totalTime, thread_no,no_of_threads);
        }
        return (EXIT_SUCCESS);
    } else {
        return (EXIT_FAILURE);
    }
}

int testMCS_mpi(int argc, char** argv) {
    unsigned int thread_no;
    MPITreeNode currNode;
    unsigned int noOfRounds = DefNoBarrierRounds;
    unsigned int no_of_threads;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread_no);

    struct timeval start, end;
    unsigned long totalTime = 0;
    if (!initializeMCSCommonStructures_MPI(&currNode, thread_no, no_of_threads)) {
        int i = noOfRounds;

        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering mcs_mpi barrier for :%d by %d\n", i, thread_no);
            mcs_barrier_mpi(&currNode, thread_no, i);
            //printf("Exiting mcs_mpi barrier for :%d by %d\n", i, thread_no);
            i--;
        }
        gettimeofday(&end, NULL);

        totalTime += getTimeDiff(start, end);
        fprintf(stdout, "\nTotal time for: %d rounds of MCS_MPI Barrier is %ld on %d with %d procs\n", noOfRounds, totalTime, thread_no,no_of_threads);
        MPI_Finalize();
        return (EXIT_SUCCESS);
    } else {
        MPI_Finalize();
        return (EXIT_FAILURE);
    }
}

int testTournament_mpi(int argc, char** argv) {
    unsigned int thread_no;
    unsigned int noOfRounds = DefNoBarrierRounds;
    unsigned int logp = 0;
    RoundNodeMPI* currentProcRounds;
    unsigned int no_of_threads = 100;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread_no);
    logp = ceil_log2(no_of_threads);

    currentProcRounds = (RoundNodeMPI*) malloc(sizeof (RoundNodeMPI)*(logp + 1));
    if (!initializeTournamentCommonStructures_MPI(currentProcRounds, thread_no, no_of_threads)) {
        int i = noOfRounds;
        struct timeval start, end;
        unsigned long totalTime = 0;

        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering tournament barrier mpi for :%d by %d\n", i, thread_no);
            tournament_barrier_mpi(currentProcRounds, thread_no, logp);
            //printf("Exiting tournament barrier mpi for :%d by %d\n", i, thread_no);
            i--;
        }
        gettimeofday(&end, NULL);
        totalTime += getTimeDiff(start, end);
        fprintf(stdout, "\nTotal time for: %d rounds of Tournament MPI Barrier is %ld on %d with %d procs\n", noOfRounds, totalTime, thread_no,no_of_threads);
        MPI_Finalize();
        return (EXIT_SUCCESS);
    } else {
        MPI_Finalize();
        return (EXIT_FAILURE);
    }
}

int testCombinedBarrier_MCS_Dissemination(int argc, char** argv) {
    unsigned int proc_no;
    MPITreeNode currNode;
    unsigned int noOfMPIRounds = DefNoBarrierRounds;
    unsigned int noOfOMPRounds = DefNoBarrierRounds;
    unsigned int no_of_threads = 2;
    unsigned int final_no_threads = 8;
    unsigned int no_of_procs;
    unsigned long timings[7];
    int i=0;
    
    MPI_Init(&argc, &argv);
    for(i=0;i<7;i++){
        timings[i] = 0;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_no);

    if (!initializeMCSCommonStructures_MPI(&currNode, proc_no, no_of_procs)) {
        int i = noOfMPIRounds;
        struct timeval start, end;
        double totalTime = 0;
        unsigned long totalTimeInM = 0;
        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering mcs_combined barrier for :%d by %d\n", i, proc_no);
            for (no_of_threads = 2; no_of_threads <= final_no_threads; no_of_threads++) {
                char sense = 1;
                unsigned int parity = 0;
                unsigned int thread_no;
                unsigned int logp = 0;
                ProcNode* allNodes = initializeDisseminationCommonStructures_OMP(no_of_threads);
                if (allNodes) {
                    logp = ceil_log2(no_of_threads);
                    struct timeval start1, end1;
                    unsigned long totalTime1 = 0;
                    gettimeofday(&start1, NULL);
                    omp_set_num_threads(no_of_threads);
#pragma omp parallel private(thread_no,sense,parity) shared(no_of_threads,noOfOMPRounds,logp,proc_no)
                    {
                        thread_no = omp_get_thread_num();
                        sense = 1;
                        parity = 0;
                        ProcNode* currNode = allNodes + thread_no;
                        
                       
                        int j = noOfOMPRounds;

                        //gettimeofday(&start1, NULL);
                        while (j > 0) {
                            /*#pragma omp critical
                                                    {
                                                        printf("Entering dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
                            dissemination_barrier_omp(currNode, &sense, logp, &parity);
                            /*#pragma omp critical
                                                    {
                                                        printf("Exiting dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
                            j--;

                        }
                        //gettimeofday(&end1, NULL);
                        //totalTime1 += getTimeDiff(start1, end1);
                        //fprintf(stderr,"\nTotal time for: %d inner rounds of MCS_MPI_Combined Barrier is %ld on %d by thread %d\n",noOfOMPRounds,totalTime1,proc_no,thread_no);
                    }
                    gettimeofday(&end1, NULL);
                    totalTime1 += getTimeDiff(start1, end1);
                    if(totalTime1 > timings[no_of_threads-2]){
                        timings[no_of_threads-2] = totalTime1;
                    }
                }
            }
            mcs_barrier_mpi(&currNode, proc_no, i);

            //printf("Exiting mcs_combined barrier for :%d by %d\n", i, proc_no);
            i--;
        }
        gettimeofday(&end, NULL);
        totalTime += getTimeDiffInMilli(start, end);
        totalTimeInM += getTimeDiff(start,end);
        fprintf(stdout, "\nTotal time for: %d outer rounds of MCS_MPI_Combined Barrier is %lf milliseconds and %ld Microseconds on %d\n", noOfMPIRounds, totalTime, totalTimeInM,proc_no);
        for(i = 0;i<7;i++){
            fprintf(stdout,"\nTime for %d OMP threads for %ld rounds is %ld\n",i+2,noOfOMPRounds,timings[i]);
        }
        MPI_Finalize();
        return (EXIT_SUCCESS);
    } else {
        MPI_Finalize();
        return (EXIT_FAILURE);
    }
}

int testCombinedBarrier_MCS_Dissemination_final(int argc, char** argv) {
    unsigned int proc_no;
    MPITreeNode currNode;
    unsigned int noOfMPIRounds = DefNoBarrierRounds;
    unsigned int no_of_threads = 2;
    unsigned int no_of_procs;
    int i=0;
    
    MPI_Init(&argc, &argv);
    no_of_threads = atoi(argv[3]);

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_no);

    if (!initializeMCSCommonStructures_MPI(&currNode, proc_no, no_of_procs)) {
        int i = noOfMPIRounds;
        struct timeval start, end;
        double totalTime = 0;
        unsigned long totalTimeInM = 0;
        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering mcs_combined barrier for :%d by %d\n", i, proc_no);
            //for (no_of_threads = 2; no_of_threads <= final_no_threads; no_of_threads++) {
                char sense = 1;
                unsigned int parity = 0;
                unsigned int thread_no;
                unsigned int logp = 0;
                ProcNode* allNodes = initializeDisseminationCommonStructures_OMP(no_of_threads);
                if (allNodes) {
                    logp = ceil_log2(no_of_threads);
                    omp_set_num_threads(no_of_threads);
#pragma omp parallel private(thread_no,sense,parity) shared(no_of_threads,logp,proc_no)
                    {
                        thread_no = omp_get_thread_num();
                        sense = 1;
                        parity = 0;
                        ProcNode* currNode = allNodes + thread_no;
                        
                       
                        //int j = noOfOMPRounds;

                        //gettimeofday(&start1, NULL);
                        //while (j > 0) {
                            /*#pragma omp critical
                                                    {
                                                        printf("Entering dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
                            dissemination_barrier_omp(currNode, &sense, logp, &parity);
                            /*#pragma omp critical
                                                    {
                                                        printf("Exiting dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
                            //j--;

                        //}
                        //gettimeofday(&end1, NULL);
                        //totalTime1 += getTimeDiff(start1, end1);
                        //fprintf(stderr,"\nTotal time for: %d inner rounds of MCS_MPI_Combined Barrier is %ld on %d by thread %d\n",noOfOMPRounds,totalTime1,proc_no,thread_no);
                    }
                }
            //}
            mcs_barrier_mpi(&currNode, proc_no, i);

            //printf("Exiting mcs_combined barrier for :%d by %d\n", i, proc_no);
            i--;
        }
        gettimeofday(&end, NULL);
        totalTime += getTimeDiffInMilli(start, end);
        totalTimeInM += getTimeDiff(start,end);
        fprintf(stdout, "\nTotal time for: %d outer rounds of MCS_MPI_Combined Barrier is %lf milliseconds or %ld Microseconds on %d with %d Procs and %d Threads\n", noOfMPIRounds, totalTime, totalTimeInM,proc_no,no_of_procs,no_of_threads);
        MPI_Finalize();
        return (EXIT_SUCCESS);
    } else {
        MPI_Finalize();
        return (EXIT_FAILURE);
    }
}

int testCombinedBarrier_default_barrier_final(int argc, char** argv) {
    unsigned int proc_no;
    unsigned int noOfMPIRounds = DefNoCombiBarrierRounds;
    unsigned int no_of_threads = 2;
    unsigned int no_of_procs;
    
    MPI_Init(&argc, &argv);
    no_of_threads = atoi(argv[3]);

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_no);
        int i = noOfMPIRounds;
        struct timeval start, end;
        double totalTime = 0;
        unsigned long totalTimeInM = 0;
        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering mcs_combined barrier for :%d by %d\n", i, proc_no);
            //for (no_of_threads = 2; no_of_threads <= final_no_threads; no_of_threads++) {
                //char sense = 1;
                //unsigned int parity = 0;
                //unsigned int thread_no;
                //unsigned int logp = 0;
                //if (allNodes) {
                    //logp = ceil_log2(no_of_threads);
                    omp_set_num_threads(no_of_threads);
#pragma omp parallel
                    {
                        //thread_no = omp_get_thread_num();
                        //sense = 1;
                        //parity = 0;
                        //ProcNode* currNode = allNodes + thread_no;
                        
                       
                        //int j = noOfOMPRounds;

                        //gettimeofday(&start1, NULL);
                        //while (j > 0) {
                            /*#pragma omp critical
                                                    {
                                                        printf("Entering dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
#pragma omp barrier 
                            /*#pragma omp critical
                                                    {
                                                        printf("Exiting dissemination combined barrier for :%d by %d on %d\n", j, thread_no, proc_no);
                                                    }*/
                            //j--;

                        //}
                        //gettimeofday(&end1, NULL);
                        //totalTime1 += getTimeDiff(start1, end1);
                        //fprintf(stderr,"\nTotal time for: %d inner rounds of MCS_MPI_Combined Barrier is %ld on %d by thread %d\n",noOfOMPRounds,totalTime1,proc_no,thread_no);
                    }
                //}
            //}
            MPI_Barrier(MPI_COMM_WORLD);

            //printf("Exiting mcs_combined barrier for :%d by %d\n", i, proc_no);
            i--;
        }
        gettimeofday(&end, NULL);
        totalTime += getTimeDiffInMilli(start, end);
        totalTimeInM += getTimeDiff(start,end);
        fprintf(stdout, "\nTotal time for: %d outer rounds of Default Combined Barrier is %lf milliseconds or %ld Microseconds on %d with %d Procs and %d Threads\n", noOfMPIRounds, totalTime, totalTimeInM,proc_no,no_of_procs,no_of_threads);
        MPI_Finalize();
        return (EXIT_SUCCESS);
}

int basecase_omp(int noOfRounds, int no_of_threads) {
    char sense = 1;
    unsigned int thread_no;
        omp_set_num_threads(no_of_threads);

#pragma omp parallel private(thread_no,sense) shared(no_of_threads,noOfRounds)
        {
            thread_no = omp_get_thread_num();
            sense = 1;
            int i = noOfRounds;
            unsigned long totalTime = 0;
            struct timeval start, end;
           
            gettimeofday(&start, NULL);
            while (i > 0) {
                /*#pragma omp critical
                                {
                                    printf("Entering mcs barrier for :%d by %d\n", i, thread_no);
                                }*/
#pragma omp barrier
                /*#pragma omp critical
                                {
                                    printf("Exiting mcs barrier for :%d by %d\n", i, thread_no);
                                }*/
                i--;
            }
            gettimeofday(&end, NULL);
            totalTime += getTimeDiff(start, end);
            fprintf(stdout, "\nTotal time for: %d rounds of default OMP Barrier is %ld on %d with %d Threads\n", noOfRounds, totalTime, thread_no,no_of_threads);
        }
        return (EXIT_SUCCESS);
}

int testdefault_mpi(int argc, char** argv) {
    unsigned int thread_no;
    unsigned int noOfRounds = DefNoCombiBarrierRounds;
    unsigned int logp = 0;
    //RoundNodeMPI* currentProcRounds;
    unsigned int no_of_threads = 100;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_of_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread_no);
    logp = ceil_log2(no_of_threads);

    //currentProcRounds = (RoundNodeMPI*) malloc(sizeof (RoundNodeMPI)*(logp + 1));
        int i = noOfRounds;
        struct timeval start, end;
        unsigned long totalTime = 0;

        gettimeofday(&start, NULL);
        while (i > 0) {
            //printf("Entering tournament barrier mpi for :%d by %d\n", i, thread_no);
            MPI_Barrier(MPI_COMM_WORLD);
            //printf("Exiting tournament barrier mpi for :%d by %d\n", i, thread_no);
            i--;
        }
        gettimeofday(&end, NULL);
        totalTime += getTimeDiff(start, end);
        fprintf(stdout, "\nTotal time for: %d rounds of Default MPI Barrier is %ld on %d with %d Procs\n", noOfRounds, totalTime, thread_no,no_of_threads);
        //MPI_Finalize();
        return (EXIT_SUCCESS);
}



void printUsage(char *programName){
    fprintf(stderr,"\nUsage:%s <Type> <BarrierType> <noOfOpenMPThreads>\n",programName);
    fprintf(stderr,"<Type>:1 for OpenMP, 2 for OpenMPI and 3 for Combined\n");
    fprintf(stderr,"<BarrierType>: 1 for Dissemination , 2 for Tournament, 3 for MCS and 4 for Default\n");
    fprintf(stderr,"Optional : <noOfOpenMPThreads>: Number of OpenMP threads, this option makes sense only for open MP/Combined barriers\n");    
    
    fprintf(stderr,"\n\nSupported Barriers:\n\tOpenMP: All\n\tOpenMPI: Tournament and MCS\n\tCombined: MCS Outer MPI and Dissemination Inner MP\n\n");
    exit(-1);
    
}

int main(int argc, char** argv) {
    //testTournament_omp(10,100);

    //printf("\n\n Start Dissemination\n");
    //testDissemination_omp(10,100);
    int type = -1;
    int barrier = -1;
    int noOfOpenMPThreads = -1;
    if (argc > 2) {
        type = atoi(argv[1]);
        if (type >= 1 && type <= 3) {
            barrier = atoi(argv[2]);
            if (barrier >= 1 && barrier <= 4) {
                if ((type == 1 || type == 3)) {
                    if (argc > 3) {
                        noOfOpenMPThreads = atoi(argv[3]);
                    } else {
                        printUsage(argv[0]);
                    }
                }
            } else {
                printUsage(argv[0]);
            }
        } else {
            printUsage(argv[0]);
        }
    } else {
        printUsage(argv[0]);
    }
    
    if(type <= 0 || barrier <= 0){
        printUsage(argv[0]);
    }
    
    if(type == 2 && barrier == 1 ){
        printUsage(argv[0]);
    }
    
    if((type == 1|| type == 3) && noOfOpenMPThreads <= 0){
        printUsage(argv[0]);
    }
    
    switch(type){
        case 1: 
            switch(barrier){
                case 1:
                        testDissemination_omp(DefNoBarrierRounds,noOfOpenMPThreads); break;
                case 2:
                        testTournament_omp(DefNoBarrierRounds,noOfOpenMPThreads); break;
                case 3: 
                        testMCS_omp(DefNoBarrierRounds,noOfOpenMPThreads); break;
                case 4:
                        basecase_omp(DefNoBarrierRounds,noOfOpenMPThreads); break;
            }
            break;
        case 2: 
            switch(barrier){
                case 2:
                    testTournament_mpi(argc,argv); break;
                case 3:
                    testMCS_mpi(argc,argv); break;
                case 4:
                    testdefault_mpi(argc,argv); break;
            }
            break;
        case 3: 
            switch(barrier){
                case 4: testCombinedBarrier_default_barrier_final(argc,argv); break;
                default: testCombinedBarrier_MCS_Dissemination_final(argc,argv); break;
            }
            break;
    } 
    

    /*printf("\nOpen MP Testing\n");
    int totalNoOfRounds = 1000000;
    int i = 0;
    for (i = 2; i <= 8; i++) {
        printf("\nDissemination Barrier, no of threads:%d\n", i);
        testDissemination_omp(totalNoOfRounds, i);
        printf("\nTournament Barrier, no of threads:%d\n", i);
        testTournament_omp(totalNoOfRounds, i);
        printf("\nMCS Barrier, no of threads:%d\n", i);
        testMCS_omp(totalNoOfRounds, i);
    }
    printf("\nOpen MP Testing Ends\n");*/

    //printf("\n\n Start MCS\n");
    //testMCS_omp(10,100);

    // printf("\n\nStart Tounament MPI\n");
     //testTournament_mpi(argc,argv);

    // printf("\n\n Start MCS MPI\n");
    //testMCS_mpi(argc,argv);
    //printf("\n\nStart Combined\n");
    //testCombinedBarrier_MCS_Dissemination(argc,argv);

    return (EXIT_SUCCESS);
}

