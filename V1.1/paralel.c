#include <stdio.h> // Standard input/output library
#include <omp.h>   // OpenMP library for parallel programming

#define MAX_JOBS 750     // Maximum number of jobs
#define MAX_MACHINES 750 // Maximum number of machines
#define MAX_OPS 600000   // Maximum number of operations
#define MAX_THREADS 128  // Maximum number of threads, adjust based on CPU

typedef struct
{
    int machineID;   // ID of the machine for this operation
    int duration;    // Processing time for this operation
    int startTime;   // Start time of this operation
    int nextOpIndex; // Index of the next operation in the job
} Operation;

Operation memoryPool[MAX_OPS];     // Pool of all operations
int firstOpOfJob[MAX_JOBS];        // Index of first operation for each job
int machineFreeTime[MAX_MACHINES]; // Free time for each machine
int jobReadyTime[MAX_JOBS];        // Ready time for each job
int opsDone[MAX_JOBS];             // Number of operations completed for each job

double threadStartTimes[MAX_THREADS]; // Start times for each thread
double threadEndTimes[MAX_THREADS];   // End times for each thread

int main()
{
    int numJobs = 0, numMachines = 0;     // Number of jobs and machines
    FILE *file = fopen("gg150.jss", "r"); // Open the input file
    if (!file)
    {
        printf("Error: gg150.jss not found!\n");
        return 1;
    }
    fscanf(file, "%d %d", &numJobs, &numMachines); // Read number of jobs and machines

    int opCounter = 0;                // Counter for operations
    for (int i = 0; i < numJobs; i++) // For each job
    {
        int previousOpIdx = -1;               // Index of previous operation
        opsDone[i] = 0;                       // Initialize operations done
        jobReadyTime[i] = 0;                  // Initialize job ready time
        for (int j = 0; j < numMachines; j++) // For each operation in the job
        {
            memoryPool[opCounter].nextOpIndex = -1;                                                   // No next operation yet
            fscanf(file, "%d %d", &memoryPool[opCounter].machineID, &memoryPool[opCounter].duration); // Read machine and duration
            if (j == 0)
                firstOpOfJob[i] = opCounter; // Set first operation index
            else
                memoryPool[previousOpIdx].nextOpIndex = opCounter; // Link to previous
            previousOpIdx = opCounter;                             // Update previous
            opCounter++;                                           // Increment counter
        }
    }
    fclose(file); // Close the file

    for (int i = 0; i < numMachines; i++)
        machineFreeTime[i] = 0; // Initialize machine free times

    double start = omp_get_wtime(); // Start timing

    int completedTotal = 0;               // Total completed operations
    int totalOps = numJobs * numMachines; // Total operations

    while (completedTotal < totalOps) // While not all operations done
    {
        int bestJob = -1;               // Best job to schedule
        int earliestStart = 2147483647; // Earliest start time

#pragma omp parallel // Parallel region
        {
            int tid = omp_get_thread_num(); // Thread ID
            if (completedTotal == 0)
                threadStartTimes[tid] = omp_get_wtime(); // Record start time

            int localBestJob = -1;               // Local best job
            int localEarliestStart = 2147483647; // Local earliest start

#pragma omp for                               // Parallel for loop
            for (int j = 0; j < numJobs; j++) // For each job
            {
                if (opsDone[j] < numMachines) // If job has operations left
                {
                    int currentIdx = firstOpOfJob[j];    // Start from first operation
                    for (int s = 0; s < opsDone[j]; s++) // Skip completed operations
                    {
                        currentIdx = memoryPool[currentIdx].nextOpIndex; // Move to next
                    }
                    int mID = memoryPool[currentIdx].machineID;                                                     // Machine ID
                    int pStart = (jobReadyTime[j] > machineFreeTime[mID]) ? jobReadyTime[j] : machineFreeTime[mID]; // Potential start time
                    if (pStart < localEarliestStart)                                                                // If earlier
                    {
                        localEarliestStart = pStart; // Update local
                        localBestJob = j;
                    }
                }
            }

#pragma omp critical // Critical section
            {
                if (localEarliestStart < earliestStart) // Update global best
                {
                    earliestStart = localEarliestStart;
                    bestJob = localBestJob;
                }
            }

            threadEndTimes[tid] = omp_get_wtime(); // Record end time
        }

        if (bestJob != -1) // If a job was selected
        {
            int currentIdx = firstOpOfJob[bestJob]; // Get current operation
            for (int s = 0; s < opsDone[bestJob]; s++)
            {
                currentIdx = memoryPool[currentIdx].nextOpIndex;
            }
            memoryPool[currentIdx].startTime = earliestStart;             // Set start time
            int finish = earliestStart + memoryPool[currentIdx].duration; // Calculate finish time
            machineFreeTime[memoryPool[currentIdx].machineID] = finish;   // Update machine free time
            jobReadyTime[bestJob] = finish;                               // Update job ready time
            opsDone[bestJob]++;                                           // Increment operations done
            completedTotal++;                                             // Increment total completed
        }
    }

    double end = omp_get_wtime(); // End timing

    printf("\n--- Thread Execution Times ---\n");
    int actualThreads = 0;
#pragma omp parallel
    {
#pragma omp master
        actualThreads = omp_get_num_threads(); // Get number of threads
    }

    for (int i = 0; i < actualThreads; i++)
    {
        printf("Thread %d: Start = %.4f, End = %.4f, Duration = %.4f\n",
               i, threadStartTimes[i], threadEndTimes[i], (threadEndTimes[i] - threadStartTimes[i]));
    }
    printf("------------------------------\n");

    printf("\nMakespan: ");
    int makespan = 0;
    for (int i = 0; i < numMachines; i++)
    {
        if (machineFreeTime[i] > makespan)
            makespan = machineFreeTime[i]; // Find maximum finish time
    }
    printf("%d\n", makespan);

    for (int i = 0; i < numJobs; i++) // For each job
    {
        int currentIdx = firstOpOfJob[i];
        while (currentIdx != -1) // Print start times
        {
            printf("%d ", memoryPool[currentIdx].startTime);
            currentIdx = memoryPool[currentIdx].nextOpIndex;
        }
        printf("\n");
    }

    printf("Total Execution Time: %f seconds\n", (end - start));

    return 0;
}