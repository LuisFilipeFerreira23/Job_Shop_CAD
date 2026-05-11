// 1. INCLUDES & MACROS
// ==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Uncomment the following block to enable the OMP PARALLEL FOR REDUCE version of the algorithm. 
Make sure to comment out the original parallel region and sequential update in the main loop as well.
/ OMP PARALLEL FOR REDUCE ------------------------------------------------------------------------------>
#include <stdint.h>
// OMP PARALLEL FOR REDUCE ------------------------------------------------------------------------------>
*/

#define MAX_JOBS 750
#define MAX_MACHINES 750
#define MAX_OPS 600000

// 2. DATA STRUCTURES & GLOBAL MEMORY
// ==============================================================================
typedef struct
{
    int machineID;
    int duration;
    int startTime;
    int nextOpIndex;
} Operation;

// Array-based linked list
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];
int machineFreeTime[MAX_MACHINES];
int jobReadyTime[MAX_JOBS];
int opsDone[MAX_JOBS];

// 3. MAIN PROGRAM
// ==============================================================================
int main(int argc, char *argv[])
{
    // --- 3.1 Argument Validation ---
    if (argc != 5)
    {
        printf("Usage: %s <input_file.jss> <output_file.txt> <num_threads> <num_repetitions>\n", argv[0]);
        return 1;
    }

    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    int numThreads = atoi(argv[3]);
    int numRepetitions = atoi(argv[4]);

    if (numThreads <= 0 || numRepetitions <= 0)
    {
        printf("Error: Number of threads and repetitions must be greater than 0.\n");
        return 1;
    }

    omp_set_num_threads(numThreads);

    // --- 3.2 File Parsing ---
    FILE *file = fopen(inputFileName, "r");
    if (!file)
    {
        printf("Error: Input file %s not found!\n", inputFileName);
        return 1;
    }

    int numJobs = 0, numMachines = 0;
    fscanf(file, "%d %d", &numJobs, &numMachines);
    int opCounter = 0;

    for (int i = 0; i < numJobs; i++)
    {
        int previousOpIdx = -1;
        for (int j = 0; j < numMachines; j++)
        {
            memoryPool[opCounter].nextOpIndex = -1;
            fscanf(file, "%d %d", &memoryPool[opCounter].machineID, &memoryPool[opCounter].duration);
            if (j == 0)
                firstOpOfJob[i] = opCounter;
            else
                memoryPool[previousOpIdx].nextOpIndex = opCounter;
            previousOpIdx = opCounter;
            opCounter++;
        }
    }
    fclose(file);

    // Algoritmo comum //
    int totalOps = numJobs * numMachines;
    double totalTime = 0.0;

    // Array to store the exact execution time of each repetition
    double *repTimes = (double *)malloc(numRepetitions * sizeof(double));
    if (!repTimes)
    {
        printf("Error: Memory allocation failed for repTimes.\n");
        return 1;
    }

    // --- 3.3 Performance Measurement Loop ---
    for (int rep = 0; rep < numRepetitions; rep++)
    {
        // 3.3.1 Reset State for Deterministic Runs
        for (int i = 0; i < numJobs; i++)
        {
            opsDone[i] = 0;
            jobReadyTime[i] = 0;
        }
        for (int i = 0; i < numMachines; i++)
        {
            machineFreeTime[i] = 0;
        }
        int completedTotal = 0;

        // 3.3.2 Start Timing
        double start = omp_get_wtime();

        // 3.3.3 Core Scheduling Loop
        while (completedTotal < totalOps)
        {
            int globalBestJob = -1;
            int globalEarliestStart = 2147483647;

            // --- PARALLEL REGION: Search for the next best operation ---
            // ---> FORK: OpenMP wakes up the team of worker threads here <---
#pragma omp parallel for
            for (int j = 0; j < numJobs; j++)
            {
                // All threads are now executing simultaneously, dividing the 'numJobs' iterations.
                if (opsDone[j] < numMachines)
                {
                    // Traverse logical linked list to find current operation
                    int currentIdx = firstOpOfJob[j];
                    for (int s = 0; s < opsDone[j]; s++)
                    {
                        currentIdx = memoryPool[currentIdx].nextOpIndex;
                    }

                    int mID = memoryPool[currentIdx].machineID;
                    int pStart = (jobReadyTime[j] > machineFreeTime[mID]) ? jobReadyTime[j] : machineFreeTime[mID];

                    // --- CRITICAL SECTION: Update global best job and earliest start time ---
#pragma omp critical
                    {
                        if (pStart < globalEarliestStart)
                        {
                            globalEarliestStart = pStart;
                            globalBestJob = j;
                        }
                    }
                }
            }
            // ---> Worker threads park/terminate.<---
            // --- END PARALLEL REGION ---

            // --- 3.3.4 Sequential State Update ---
            if (globalBestJob != -1)
            {
                int currentIdx = firstOpOfJob[globalBestJob];
                for (int s = 0; s < opsDone[globalBestJob]; s++)
                {
                    currentIdx = memoryPool[currentIdx].nextOpIndex;
                }
                memoryPool[currentIdx].startTime = globalEarliestStart;
                int finish = globalEarliestStart + memoryPool[currentIdx].duration;

                // Advance time variables
                machineFreeTime[memoryPool[currentIdx].machineID] = finish;
                jobReadyTime[globalBestJob] = finish;

                opsDone[globalBestJob]++;
                completedTotal++;
            }
        }

        /* Uncomment the following block to enable the OMP PARALLEL FOR REDUCE version of the algorithm. 
        Make sure to comment out the original parallel region and sequential update in the main loop as well.
        // OMP PARALLEL FOR REDUCE ------------------------------------------------------------------------------>
        // 1. Initialize a 64-bit unsigned integer to the maximum possible value
        uint64_t globalBestPacked = UINT64_MAX;

        // 2. Apply the standard reduction clause to the packed variable
#pragma omp parallel for reduction(min : globalBestPacked)
        for (int j = 0; j < numJobs; j++)
        {
            if (opsDone[j] < numMachines)
            {
                int currentIdx = firstOpOfJob[j];
                for (int s = 0; s < opsDone[j]; s++)
                {
                    currentIdx = memoryPool[currentIdx].nextOpIndex;
                }

                int mID = memoryPool[currentIdx].machineID;
                int pStart = (jobReadyTime[j] > machineFreeTime[mID]) ? jobReadyTime[j] : machineFreeTime[mID];

                // 3. Pack the data: Shift time left by 32 bits, then apply a bitwise OR with the Job ID
                uint64_t localPacked = ((uint64_t)pStart << 32) | (uint32_t)j;

                // 4. Lock-free local update
                if (localPacked < globalBestPacked)
                {
                    globalBestPacked = localPacked;
                }
            }
        } // Implicit barrier and safe OpenMP reduction occur here

        // 5. Unpack the results sequentially
        if (globalBestPacked != UINT64_MAX)
        {
            // Extract upper 32 bits (Time) by shifting right
            int globalEarliestStart = (int)(globalBestPacked >> 32);

            // Extract lower 32 bits (Job ID) by masking with 0xFFFFFFFF
            int globalBestJob = (int)(globalBestPacked & 0xFFFFFFFF);

            // --- Sequential State Update ---
            int currentIdx = firstOpOfJob[globalBestJob];
            for (int s = 0; s < opsDone[globalBestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }
            memoryPool[currentIdx].startTime = globalEarliestStart;
            int finish = globalEarliestStart + memoryPool[currentIdx].duration;

            machineFreeTime[memoryPool[currentIdx].machineID] = finish;
            jobReadyTime[globalBestJob] = finish;

            opsDone[globalBestJob]++;
            completedTotal++;
        }
    }
    // OMP PARALLEL FOR REDUCE ------------------------------------------------------------------------------>
*/

        // 3.3.5 End Timing
        double end = omp_get_wtime();
        double currentRepTime = end - start;

        repTimes[rep] = currentRepTime; // Store in the array
        totalTime += currentRepTime;    // Add to total for the average
    }

    // --- 3.4 Calculate Metrics ---
    double averageTime = totalTime / numRepetitions;
    printf("Average execution time (%d repetitions): %f seconds\n", numRepetitions, averageTime);

    // --- 3.5 Write Output to File ---
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
    {
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i];
    }

    FILE *outFile = fopen(outputFileName, "w");
    if (!outFile)
    {
        printf("Error: Could not create output file %s\n", outputFileName);
        return 1;
    }

    fprintf(outFile, "%d\n", makespan);

    for (int i = 0; i < numJobs; i++)
    {
        int currentIdx = firstOpOfJob[i];
        while (currentIdx != -1)
        {
            fprintf(outFile, "%d ", memoryPool[currentIdx].startTime);
            currentIdx = memoryPool[currentIdx].nextOpIndex;
        }
        fprintf(outFile, "\n");
    }

    // --- Append the Performance Report ---
    fprintf(outFile, "\n--- Performance Analysis ---\n");
    fprintf(outFile, "Threads used: %d\n", numThreads);
    for (int r = 0; r < numRepetitions; r++)
    {
        fprintf(outFile, "Repetition %d: %f seconds\n", r + 1, repTimes[r]);
    }
    fprintf(outFile, "Average Execution Time: %f seconds\n", averageTime);

    fclose(outFile);
    free(repTimes); // Always free dynamically allocated memory

    return 0;
}