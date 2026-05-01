#include <stdio.h>
#include <omp.h> // The OpenMP API: Enables multi-core processing

#define MAX_JOBS 750
#define MAX_MACHINES 750
#define MAX_OPS 600000
#define MAX_THREADS 128 // Defines the maximum hardware threads the code can scale to

// SECTION: DATA STRUCTURES
// Here we define what an "Operation" looks like.
// It tracks which machine it needs, how long it takes,
// and when it is scheduled to start.
typedef struct
{
    int machineID;
    int duration;
    int startTime;
    int nextOpIndex;
} Operation;

// SHARED MEMORY: These arrays are stored in a place where all threads
// can read them to coordinate the schedule.
Operation memoryPool[MAX_OPS];
int firstOpOfJob[MAX_JOBS];
int machineFreeTime[MAX_MACHINES];
int jobReadyTime[MAX_JOBS];
int opsDone[MAX_JOBS];

// PERFORMANCE MONITORING: Global arrays to track individual thread behavior
double threadStartTimes[MAX_THREADS];
double threadEndTimes[MAX_THREADS];

int main()
{
    // SECTION: FILE LOADING
    // The program opens the instruction file and reads the
    // total number of jobs and machines.
    int numJobs = 0, numMachines = 0;
    FILE *file = fopen("gg150.jss", "r"); // Change to "gg03.jss" or "gg20.jss" for a smaller test case
    if (!file)
    {
        printf("Error: gg150.jss not found!\n"); // Change to "gg03.jss" or "gg20.jss" for a smaller test case
        return 1;
    }
    fscanf(file, "%d %d", &numJobs, &numMachines);

    // SECTION: ORGANIZING THE TASKS
    // We loop through every job and link its operations together
    // in a chain, so the computer knows the required order of work.
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

    // SECTION: INITIALIZATION
    // Before we start scheduling, we set all machines to be free at time 0.
    for (int i = 0; i < numMachines; i++)
        machineFreeTime[i] = 0;

    // SECTION: SCHEDULING LOGIC
    // We start a timer and begin a loop that continues until every
    // single operation is scheduled.
    double start = omp_get_wtime();

    int completedTotal = 0;
    int totalOps = numJobs * numMachines;

    while (completedTotal < totalOps)
    {
        int bestJob = -1;
        int earliestStart = 2147483647;

// SECTION: THE PARALLEL REGION
// #pragma omp parallel tells the compiler to create a team of threads.
// Each thread executes the code inside this block simultaneously.
#pragma omp parallel
        {

            // THREAD IDENTIFICATION: Each 'worker' gets a unique ID (tid)
            int tid = omp_get_thread_num();

            if (completedTotal == 0)
                threadStartTimes[tid] = omp_get_wtime();

            // THREAD-LOCAL STORAGE: These variables are 'private' to each thread
            // so they can keep track of their own 'best find' without interference.
            int localBestJob = -1;
            int localEarliestStart = 2147483647;
            // WORK SHARING: The 'omp for' directive automatically splits the
            // 750 jobs among the available threads (e.g., thread 1 does jobs 1-50).

#pragma omp for
            for (int j = 0; j < numJobs; j++)
            {
                if (opsDone[j] < numMachines)
                {

                    // Each thread performs this calculation independently
                    int currentIdx = firstOpOfJob[j];
                    for (int s = 0; s < opsDone[j]; s++)
                    {
                        currentIdx = memoryPool[currentIdx].nextOpIndex;
                    }
                    int mID = memoryPool[currentIdx].machineID;
                    int pStart = (jobReadyTime[j] > machineFreeTime[mID]) ? jobReadyTime[j] : machineFreeTime[mID];

                    // Local comparison: thread checks its assigned jobs
                    if (pStart < localEarliestStart)
                    {
                        localEarliestStart = pStart;
                        localBestJob = j;
                    }
                }
            }

// SECTION: MUTUAL EXCLUSION (CRITICAL SECTION)
// Only one thread at a time can enter this block.
// This prevents 'Race Conditions' where two threads might try to
// write to 'earliestStart' at the exact same millisecond.
#pragma omp critical
            {
                if (localEarliestStart < earliestStart)
                {

                    // The global 'winning' job is determined here
                    earliestStart = localEarliestStart;
                    bestJob = localBestJob;
                }
            }

            // SYNCHRONIZATION POINT: Threads record their finish time
            // before the parallel region ends.

            threadEndTimes[tid] = omp_get_wtime();
            // END PARALLEL REGION: Threads 'join' back into a single process here.
        }

        // SECTION: UPDATING THE TIMELINE
        // Once the best job is found, we "lock it in" to the schedule,
        // update the machine's busy timer, and mark the job as ready
        // for its next step.
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
    double end = omp_get_wtime();

    // SECTION: PARALLEL DIAGNOSTICS
    printf("\n--- Thread Execution Times ---\n");
    int actualThreads = 0;

// MASTER DIRECTIVE: Only the primary thread asks the system
// how many workers were actually deployed.
#pragma omp parallel
    {
#pragma omp master
        actualThreads = omp_get_num_threads();
    }

    // Displays the efficiency of the parallel distribution
    for (int i = 0; i < actualThreads; i++)
    {
        printf("Thread %d: Start = %.4f, End = %.4f, Duration = %.4f\n",
               i, threadStartTimes[i], threadEndTimes[i], (threadEndTimes[i] - threadStartTimes[i]));
    }

    // SECTION: FINAL SCHEDULE OUTPUT
    // 1. Makespan: The time the last machine finishes.
    // 2. Start Times: When each operation in each job begins.
    // 3. Total Runtime: How long the computer spent thinking.
    printf("------------------------------\n");
    printf("\nMakespan: ");
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

    printf("Total Execution Time: %f seconds\n", (end - start));

    return 0;
}