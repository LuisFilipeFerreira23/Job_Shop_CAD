#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_JOBS 750
#define MAX_MACHINES 750
#define MAX_OPS 600000

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

int main(int argc, char *argv[])
{
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

    int totalOps = numJobs * numMachines;
    double totalTime = 0.0;

    double *repTimes = (double *)malloc(numRepetitions * sizeof(double));
    if (!repTimes)
    {
        printf("Error: Memory allocation failed for repTimes.\n");
        return 1;
    }

    for (int rep = 0; rep < numRepetitions; rep++)
    {
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

        double start = omp_get_wtime();

        while (completedTotal < totalOps)
        {
            int globalBestJob = -1;
            int globalEarliestStart = 2147483647;

            #pragma omp parallel for
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

        double end = omp_get_wtime();
        double currentRepTime = end - start;

        repTimes[rep] = currentRepTime;
        totalTime += currentRepTime;
    }

    double averageTime = totalTime / numRepetitions;
    printf("Average execution time (%d repetitions): %f seconds\n", numRepetitions, averageTime);

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

    fprintf(outFile, "\n--- Performance Analysis ---\n");
    fprintf(outFile, "Threads used: %d\n", numThreads);
    for (int r = 0; r < numRepetitions; r++)
    {
        fprintf(outFile, "Repetition %d: %f seconds\n", r + 1, repTimes[r]);
    }
    fprintf(outFile, "Average Execution Time: %f seconds\n", averageTime);

    fclose(outFile);
    free(repTimes);

    return 0;
}