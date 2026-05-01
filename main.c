#include "Aux_Functions/AuxFunctions.h"
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>   // for clock_t, clock()
#include <unistd.h> // for sleep()

#define BILLION 1000000000.0
#define MAX_BUFFER 10000
#define MAX_JOBS 100
#define MAX_OPS 100

int numJobs = 0, numMachines = 0;

void fileToArray(const char *finalString) {

  int num_count = 0;
  int i = 0;
  // Length of the input string
  int len = strlen(finalString);

  // Extract num_jobs and num_machines first
  while (i < len && num_count < 2) {
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      int val = 0;
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }
      if (num_count == 0) {
        numJobs = val;
      } else if (num_count == 1) {
        numMachines = val;
      }
      num_count++;
    } else {
      i++;
    }
  }

  // Arrays to store machine and duration for each operation of each job
  int jobMachines[numJobs][numMachines]; // which machine for each operation
  int operationDurations[numJobs][numMachines]; // duration for each operation

  // Initialize the arrays to zero and ensure they are properly sized based on
  // numJobs and numMachines
  memset(jobMachines, 0, sizeof(jobMachines));
  memset(operationDurations, 0, sizeof(operationDurations));

  // Parse the pairs [machine, duration] for each job
  int job_idx = 0;
  int operation_idx = 0;

  while (i < len && job_idx < numJobs) {
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      int val = 0;
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }

      // Alternate between machine and duration
      if (operation_idx % 2 == 0) {
        // Even: this is a machine ID
        jobMachines[job_idx][operation_idx / 2] = val;
      } else {
        // Odd: this is a duration
        operationDurations[job_idx][operation_idx / 2] = val;
        operation_idx++; // only increment after we get both machine and
                         // duration

        if (operation_idx / 2 == numMachines) {
          job_idx++;
          operation_idx = 0;
        }
        continue;
      }
      operation_idx++;
    } else {
      i++;
    }
  }

  // Auxiliary array to track when each machine becomes free
  int machinesFreeTime[numMachines];
  memset(machinesFreeTime, 0, sizeof(machinesFreeTime));

  // Schedule array to store actual entry and exit times for each job and
  // machine
  int scheduleArray[numJobs][2 * numMachines];
  memset(scheduleArray, 0, sizeof(scheduleArray));

  // Schedule each operation of each job on its respective machine
  for (int j = 0; j < numJobs; j++) {
    for (int op = 0; op < numMachines; op++) {
      int machine = jobMachines[j][op];
      int operation_duration = operationDurations[j][op];

      // Check when this machine becomes free and schedule the operation
      int startTime = machinesFreeTime[machine];
      int endTime = startTime + operation_duration;

      // Store entry time and exit time in schedule array
      scheduleArray[j][2 * op] = startTime;   // entry time
      scheduleArray[j][2 * op + 1] = endTime; // exit time

      // Update machine free time
      machinesFreeTime[machine] = endTime;
    }
  }

  // Print job machines
  printf("\nJob Machines:\n");
  for (int j = 0; j < numJobs; j++) {
    printf("Job %d: ", j);
    for (int op = 0; op < numMachines; op++) {
      printf("%d ", jobMachines[j][op]);
    }
    printf("\n");
  }

  // Print operation durations
  printf("\nOperation Durations:\n");
  for (int j = 0; j < numJobs; j++) {
    printf("Job %d: ", j);
    for (int op = 0; op < numMachines; op++) {
      printf("%d ", operationDurations[j][op]);
    }
    printf("\n");
  }

  // Print auxiliary array with machine free times
  printf("\nMachines Free Time:\n");
  for (int m = 0; m < numMachines; m++) {
    printf("Machine %d: %d\n", m, machinesFreeTime[m]);
  }

  // Print the schedule array with entry and exit times
  printf("\nSchedule Array (Entry and Exit Times):\n");
  for (int j = 0; j < numJobs; j++) {
    printf("Job %d: ", j);
    for (int op = 0; op < numMachines; op++) {
      printf("M%d:[%d-%d] ", jobMachines[j][op], scheduleArray[j][2 * op],
             scheduleArray[j][2 * op + 1]);
    }
    printf("\n");
  }
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________

int main() {

  /* MANTÉM-SE PARA UMA FUTURA IMPLEMENTAÇÃO DE GERAÇÃO DE FICHEIROS DE ENTRADA
    printf("Select the number of jobs and machines for the entry file "
           "generation:\n");

    printf("Number of Jobs:\n ");
    scanf("%d", &numJobs);
    printf("Number of Machines:\n");
    scanf("%d", &numMachines);

    generateData(numJobs, numMachines);
    */

  // Start the timer
  clock_t start_clock = clock();

  // Initialize the string to empty
  char finalString[MAX_BUFFER] = "";

  // Call the function to read the file and store its content in finalString
  readFiles("./Entry_Files/Entry_Files_1.txt", finalString);

  printf("File content:\n%s\n", finalString);

  // Call the function to convert the file content to an array (not implemented
  // yet)
  fileToArray(finalString);

  // Stop the timer
  clock_t end_clock = clock();

  // time_spent = end_clock - start_clock divided by CLOCKS_PER_SEC to get the
  // time in seconds
  double time_spent = (end_clock - start_clock) / 1000.0;

  // Print the time spent in seconds
  printf("Execution time: %f seconds \n", time_spent);

  return 0;
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________