#include <stdio.h>

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

// Global static arrays (Pre-allocated memory, no pointers)
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];
int machineFreeTime[MAX_MACHINES];
int jobReadyTime[MAX_JOBS];
int opsDone[MAX_JOBS];

int main()
{
    FILE *file = fopen("gg03.jss", "r");
    if (!file)
    {
        printf("Error: Could not open file.\n");
        return 1;
    }

    int numJobs, numMachines;
    if (fscanf(file, "%d %d", &numJobs, &numMachines) != 2)
    {
        fclose(file);
        return 1;
    }

    // 1. DATA INITIALIZATION (Sequential Reading)
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
            {
                firstOpOfJob[i] = opCounter;
            }
            else
            {
                memoryPool[previousOpIdx].nextOpIndex = opCounter;
            }

            previousOpIdx = opCounter;
            opCounter++;
        }
    }
    fclose(file);

    for (int i = 0; i < MAX_MACHINES; i++)
        machineFreeTime[i] = 0;

    // 2. SCHEDULING SIMULATION (Sequential Logic)
    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647; // Max int

        // Search through all jobs for the next operation that can start soonest
        for (int j = 0; j < numJobs; j++)
        {
            if (opsDone[j] < numMachines)
            {
                // Navigate through the linked indices in the pool to the current operation
                int currentIdx = firstOpOfJob[j];
                for (int s = 0; s < opsDone[j]; s++)
                {
                    currentIdx = memoryPool[currentIdx].nextOpIndex;
                }

                int mID = memoryPool[currentIdx].machineID;

                // Calculate when this operation could start
                int potentialStart = (jobReadyTime[j] > machineFreeTime[mID])
                                         ? jobReadyTime[j]
                                         : machineFreeTime[mID];

                // If this job can start earlier than our current best candidate, select it
                if (potentialStart < earliestStart)
                {
                    earliestStart = potentialStart;
                    bestJob = j;
                }
            }
        }

        // 3. COMMIT THE CHOSEN OPERATION
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

            // Update machine and job availability for the next round of the loop
            machineFreeTime[mID] = finish;
            jobReadyTime[bestJob] = finish;

            opsDone[bestJob]++;
            completedTotal++;
        }
    }

    // 4. FINAL CALCULATIONS AND OUTPUT
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
    {
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i];
    }

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