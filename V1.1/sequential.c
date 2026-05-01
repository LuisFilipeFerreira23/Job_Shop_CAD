#include <stdio.h>
#include <time.h>

#define MAX_JOBS 750
#define MAX_MACHINES 750
#define MAX_OPS 600000

// SECTION: DATA STRUCTURES
// Defines an "Operation" as a single unit of work.
// It keeps track of the machine needed, how long it takes,
// and the final scheduled start time.
typedef struct
{
    int machineID;
    int duration;
    int startTime;
    int nextOpIndex;
} Operation;

// SECTION: GLOBAL STATE
// These arrays store the status of the entire system:
// 1. memoryPool: The master list of all 600,000 operations.
// 2. machineFreeTime: Tracks when each machine will finish its current task.
// 3. jobReadyTime: Tracks when a job can move to its next step.
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];
int machineFreeTime[MAX_MACHINES];
int jobReadyTime[MAX_JOBS];
int opsDone[MAX_JOBS];

int main()
{
    // SECTION: DATA ACQUISITION
    // Open the specification file and read how many Jobs and Machines exist.
    int numJobs = 0, numMachines = 0;
    FILE *file = fopen("gg150.jss", "r"); // Change to "gg03.jss" or "gg20.jss" for a smaller test case
    if (!file)
    {
        printf("Error: gg150.jss not found!\n"); // Change to "gg03.jss" or "gg20.jss" for a smaller test case
        return 1;
    }
    fscanf(file, "%d %d", &numJobs, &numMachines);

    // SECTION: TASK LINKING
    // For every job, we read its sequence of operations and link them
    // together like a chain (Operation 1 -> Operation 2 -> etc).
    int opCounter = 0;
    for (int i = 0; i < numJobs; i++)
    {
        int previousOpIdx = -1;
        opsDone[i] = 0;
        jobReadyTime[i] = 0;
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

    for (int i = 0; i < numMachines; i++)
        machineFreeTime[i] = 0;

    // SECTION: TIME TRACKING
    // Using high-precision clock to measure how long the single core works.
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    // SECTION: LINEAR SEARCH
    // We keep looping until every operation in the factory is finished.
    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647;

        // The single thread loops through every job to see which one
        // can start earliest based on machine and job availability.
        for (int j = 0; j < numJobs; j++)
        {
            if (opsDone[j] < numMachines)
            {
                // Find the specific operation this job needs to do next.
                int currentIdx = firstOpOfJob[j];
                for (int s = 0; s < opsDone[j]; s++)
                {
                    currentIdx = memoryPool[currentIdx].nextOpIndex;
                }

                // Determine the start time:
                // Either when the machine is free OR when the job is ready.
                int mID = memoryPool[currentIdx].machineID;
                int pStart = (jobReadyTime[j] > machineFreeTime[mID]) ? jobReadyTime[j] : machineFreeTime[mID];

                if (pStart < earliestStart)
                {
                    earliestStart = pStart;
                    bestJob = j;
                }
            }
        }

        // SECTION: ASSIGNMENT
        // Update the master schedule with the winning candidate.
        if (bestJob != -1)
        {
            int currentIdx = firstOpOfJob[bestJob];
            for (int s = 0; s < opsDone[bestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }
            memoryPool[currentIdx].startTime = earliestStart;
            int finish = earliestStart + memoryPool[currentIdx].duration;
            machineFreeTime[memoryPool[currentIdx].machineID] = finish;
            jobReadyTime[bestJob] = finish;
            opsDone[bestJob]++;
            completedTotal++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    // SECTION: RESULTS DISPLAY
    printf("Execution finished.\n\n");

    // Calculate the "Makespan" (the point in time when the very last task finishes).
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
    {
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i];
    }
    printf("Makespan: %d\n", makespan);

    // Print the start time of every operation for every job.
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

    // Report total time spent calculating the schedule on a single core.
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\nTotal Execution Time (Sequential): %f seconds\n", elapsed);

    return 0;
}