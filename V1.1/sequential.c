#include <stdio.h>

#define MAX_JOBS 50
#define MAX_MACHINES 50
#define MAX_OPS 2500

/* DATA STRUCTURE
   Represents a single task (Operation) within a job. */
typedef struct
{
    int machineID;   // Which machine is needed
    int duration;    // How long the task takes
    int startTime;   // Calculated start time (result)
    int nextOpIndex; // Index-based "pointer" to the next task in this job
} Operation;

/* GLOBAL MEMORY POOL
   Pre-allocated arrays to store all data without using pointers or malloc. */
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];        // Stores the index of the first operation for each job
int machineFreeTime[MAX_MACHINES]; // Tracks when each machine is next available
int jobReadyTime[MAX_JOBS];        // Tracks when each job is ready for its next operation
int opsDone[MAX_JOBS];             // Counter for how many operations are finished per job

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

    /* 1. DATA INITIALIZATION
       Reads the file and builds the "linked list" of operations using array indices. */
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
                firstOpOfJob[i] = opCounter; // Mark the start of the job
            else
                memoryPool[previousOpIdx].nextOpIndex = opCounter; // Link previous op to this one

            previousOpIdx = opCounter;
            opCounter++;
        }
    }
    fclose(file);

    for (int i = 0; i < MAX_MACHINES; i++)
        machineFreeTime[i] = 0;

    /* 2. SCHEDULING SIMULATION
       Core Logic: At each step, we look at all available jobs and pick the one
       that can start its next operation the earliest. */
    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647; // Initialize with maximum integer value

        /* SEARCH PHASE
           We check every job to see which one "wins" the next time slot. */
        for (int j = 0; j < numJobs; j++)
        {
            if (opsDone[j] < numMachines)
            {
                // Traverse the index-links to find the current active operation for Job 'j'
                int currentIdx = firstOpOfJob[j];
                for (int s = 0; s < opsDone[j]; s++)
                {
                    currentIdx = memoryPool[currentIdx].nextOpIndex;
                }

                int mID = memoryPool[currentIdx].machineID;

                // An operation can only start when the JOB is ready AND the MACHINE is free
                int potentialStart = (jobReadyTime[j] > machineFreeTime[mID])
                                         ? jobReadyTime[j]
                                         : machineFreeTime[mID];

                // Greedy Selection: Pick the job that can start soonest
                if (potentialStart < earliestStart)
                {
                    earliestStart = potentialStart;
                    bestJob = j;
                }
            }
        }

        /* 3. COMMIT PHASE
           Once the "bestJob" is found, we officially schedule it. */
        if (bestJob != -1)
        {
            // Find the operation again to update its details
            int currentIdx = firstOpOfJob[bestJob];
            for (int s = 0; s < opsDone[bestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }

            Operation *op = &memoryPool[currentIdx];
            int mID = op->machineID;

            op->startTime = earliestStart;
            int finish = earliestStart + op->duration;

            /* UPDATE RESOURCES
               Mark the machine and the job as 'busy' until the finish time. */
            machineFreeTime[mID] = finish;
            jobReadyTime[bestJob] = finish;

            opsDone[bestJob]++;
            completedTotal++;
        }
    }

    /* 4. FINAL CALCULATIONS AND OUTPUT */
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
    {
        // The makespan is the time when the last machine finishes all its work
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i];
    }

    printf("%d\n", makespan);
    for (int i = 0; i < numJobs; i++)
    {
        // Print start times for each job's operations in sequence
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