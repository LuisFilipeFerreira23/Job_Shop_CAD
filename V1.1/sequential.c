// 1. INCLUDES & MACROS
// ==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    if (argc != 3)
    {
        printf("Usage: %s <input_file.jss> <output_file.txt>\n", argv[0]);
        return 1;
    }

    char *inputFileName = argv[1];
    char *outputFileName = argv[2];

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

    // 3.3 Declare timespec structures
    struct timespec start, end;

    // 3.3.1 Capture the start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    int totalOps = numJobs * numMachines;
    int completedTotal = 0;
    
    // 3.4 Core Scheduling Loop
    while (completedTotal < totalOps)
    {
        int globalBestJob = -1;
        int globalEarliestStart = 2147483647;

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

                if (pStart < globalEarliestStart)
                {
                    globalEarliestStart = pStart;
                    globalBestJob = j;
                }
            }
        }

        // --- 3.5 Sequential State Update ---
        if (globalBestJob != -1)
        {
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

    // 3.3.3 Capture the end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // 3.3.4 Calculate total elapsed time in seconds
    double totalTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    // --- 3.6 Write Output to File ---
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
    fprintf(outFile, "Execution time: %f seconds\n", totalTime);

    fclose(outFile);
    return 0;
}