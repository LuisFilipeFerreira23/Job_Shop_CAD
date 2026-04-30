#include <stdio.h>
#include <omp.h> // Required for OpenMP functions and directives

#define MAX_JOBS 50
#define MAX_MACHINES 50
#define MAX_OPS 2500

typedef struct
{
    int machineID;
    int duration;
    int startTime;
    int nextOpIndex; // Index-based "pointer" to the next operation in the pool
} Operation;

/* GLOBAL DATA STRUCTURES
   Pre-allocated arrays to avoid dynamic memory allocation (malloc) */
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];
int machineFreeTime[MAX_MACHINES];
int jobReadyTime[MAX_JOBS];
int opsDone[MAX_JOBS];

int main()
{
    FILE *file = fopen("gg03.jss", "r");
    if (!file)
        return 1;

    int numJobs, numMachines;
    fscanf(file, "%d %d", &numJobs, &numMachines);

    /* 1. DATA INITIALIZATION
       Reads the JSS file and links operations together using array indices */
    int opCounter = 0;
    for (int i = 0; i < numJobs; i++)
    {
        int previousOpIdx = -1;
        opsDone[i] = 0;
        jobReadyTime[i] = 0;
        for (int j = 0; j < numMachines; j++)
        {
            memoryPool[opCounter].startTime = 0;
            memoryPool[opCounter].nextOpIndex = -1;
            fscanf(file, "%d %d", &memoryPool[opCounter].machineID, &memoryPool[opCounter].duration);

            if (j == 0)
                firstOpOfJob[i] = opCounter; // First op of Job 'i'
            else
                memoryPool[previousOpIdx].nextOpIndex = opCounter; // Link current op to the previous one

            previousOpIdx = opCounter;
            opCounter++;
        }
    }
    fclose(file);

    for (int i = 0; i < MAX_MACHINES; i++)
        machineFreeTime[i] = 0;

    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    /* 2. MAIN SIMULATION LOOP
       Runs until every single operation across all jobs is scheduled */
    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647;

/* START PARALLEL REGION
   The team of threads is created here. Variables defined inside are private to each thread. */
#pragma omp parallel
        {
            int localBestJob = -1;
            int localEarliestStart = 2147483647;

/* PARALLEL FOR LOOP
   Distributes the 'numJobs' iterations across available CPU cores. */
#pragma omp for
            for (int j = 0; j < numJobs; j++)
            {
                if (opsDone[j] < numMachines)
                {
                    // Find the current operation for Job 'j' by traversing indices
                    int currentIdx = firstOpOfJob[j];
                    for (int s = 0; s < opsDone[j]; s++)
                    {
                        currentIdx = memoryPool[currentIdx].nextOpIndex;
                    }

                    int mID = memoryPool[currentIdx].machineID;

                    // Logic: Op starts only when BOTH the job is ready AND the machine is free
                    int pStart = (jobReadyTime[j] > machineFreeTime[mID])
                                     ? jobReadyTime[j]
                                     : machineFreeTime[mID];

                    // Threads update their own local best candidates first
                    if (pStart < localEarliestStart)
                    {
                        localEarliestStart = pStart;
                        localBestJob = j;
                    }
                }
            }

/* CRITICAL SECTION
   Only one thread at a time can enter this block.
   This prevents a "Race Condition" when updating the global shared bestJob. */
#pragma omp critical
            {
                if (localEarliestStart < earliestStart)
                {
                    earliestStart = localEarliestStart;
                    bestJob = localBestJob;
                }
            }
        } // END PARALLEL REGION

        /* 3. UPDATE STATE
           Executed sequentially to ensure we don't skip increments or corrupt data. */
        if (bestJob != -1)
        {
            // Find the chosen operation again to update its finish time
            int currentIdx = firstOpOfJob[bestJob];
            for (int s = 0; s < opsDone[bestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }

            Operation *op = &memoryPool[currentIdx];
            op->startTime = earliestStart;
            int finish = earliestStart + op->duration;

            // Commit updates to shared resources
            machineFreeTime[op->machineID] = finish;
            jobReadyTime[bestJob] = finish;

            opsDone[bestJob]++;
            completedTotal++;
        }
    }

    /* 4. CALCULATE MAKESPAN AND OUTPUT
       The makespan is the maximum finish time across all machines. */
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i];

    printf("%d\n", makespan);
    for (int i = 0; i < numJobs; i++)
    {
        int currentIdx = firstOpOfJob[i];
        while (currentIdx != -1)
        {
            printf("%d ", memoryPool[currentIdx].startTime);
            currentIdx = memoryPool[currentIdx].nextOpIndex;
        }
        printf("\n");
    }

    return 0;
}