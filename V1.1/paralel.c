#include <stdio.h>
#include <omp.h>

#define MAX_JOBS 50
#define MAX_MACHINES 50
#define MAX_OPS 2500

typedef struct
{
    int machineID;
    int duration;
    int startTime;
    int nextOpIndex;
} Operation;

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

    // 1. DATA INITIALIZATION
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
                firstOpOfJob[i] = opCounter;
            else
                memoryPool[previousOpIdx].nextOpIndex = opCounter;

            previousOpIdx = opCounter;
            opCounter++;
        }
    }
    fclose(file);

    for (int i = 0; i < MAX_MACHINES; i++)
        machineFreeTime[i] = 0;

    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    // 2. PARALLEL SIMULATION
    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647;

/* PARALLEL SEARCH: Multiple threads look for a candidate */
#pragma omp parallel
        {
            int localBestJob = -1;
            int localEarliestStart = 2147483647;

#pragma omp for
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
                    int pStart = (jobReadyTime[j] > machineFreeTime[mID])
                                     ? jobReadyTime[j]
                                     : machineFreeTime[mID];

                    if (pStart < localEarliestStart)
                    {
                        localEarliestStart = pStart;
                        localBestJob = j;
                    }
                }
            }

/* CRITICAL: Update global best. The first thread to reach
   this block with the best time "wins" the machine. */
#pragma omp critical
            {
                if (localEarliestStart < earliestStart)
                {
                    earliestStart = localEarliestStart;
                    bestJob = localBestJob;
                }
            }
        }

        // 3. UPDATE STATE (Sequential update ensures integrity)
        if (bestJob != -1)
        {
            int currentIdx = firstOpOfJob[bestJob];
            for (int s = 0; s < opsDone[bestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }

            Operation *op = &memoryPool[currentIdx];
            int mID = op->machineID;

            op->startTime = earliestStart;
            int finish = earliestStart + op->duration;

            machineFreeTime[mID] = finish;
            jobReadyTime[bestJob] = finish;

            opsDone[bestJob]++;
            completedTotal++;
        }
    }

    // 4. OUTPUT
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